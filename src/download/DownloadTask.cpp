#include "DownloadTask.h"
#include <QNetworkAccessManager>
#include <QDir>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(downloadTask, "cryovex.download.task")

DownloadTask::DownloadTask(const QUrl& url, const QString& filePath, 
                          const QString& expectedSha1, QObject *parent)
    : QObject(parent)
    , m_url(url)
    , m_filePath(filePath)
    , m_expectedSha1(expectedSha1)
{
}

DownloadTask::~DownloadTask()
{
    cleanup();
}

double DownloadTask::downloadSpeed() const
{
    return m_currentSpeed;
}

void DownloadTask::start(QNetworkAccessManager* manager)
{
    if (m_status == Downloading) {
        return;
    }
    
    qCInfo(downloadTask) << "Starting download:" << m_url.toString();
    
    // Create directory if it doesn't exist
    QDir().mkpath(QFileInfo(m_filePath).absolutePath());
    
    // Open file for writing
    m_file = new QFile(m_filePath, this);
    if (!m_file->open(QIODevice::WriteOnly)) {
        qCWarning(downloadTask) << "Failed to open file for writing:" << m_filePath;
        setStatus(Failed);
        emit error("Failed to open file for writing: " + m_filePath);
        return;
    }
    
    // Initialize hash if expected SHA1 is provided
    if (!m_expectedSha1.isEmpty()) {
        m_hash = new QCryptographicHash(QCryptographicHash::Sha1);
    }
    
    // Create network request
    QNetworkRequest request(m_url);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    
    // Start download
    m_reply = manager->get(request);
    
    // Connect signals
    connect(m_reply, &QNetworkReply::readyRead, this, &DownloadTask::onReadyRead);
    connect(m_reply, &QNetworkReply::finished, this, &DownloadTask::onFinished);
    connect(m_reply, &QNetworkReply::downloadProgress, this, &DownloadTask::onDownloadProgress);
    connect(m_reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred),
            this, &DownloadTask::onError);
    
    setStatus(Downloading);
    m_speedTimer.start();
    m_lastBytes = 0;
}

void DownloadTask::pause()
{
    if (m_status == Downloading && m_reply) {
        m_reply->abort();
        setStatus(Paused);
    }
}

void DownloadTask::resume()
{
    // For simplicity, restart the download
    // In a real implementation, you'd support HTTP range requests
    if (m_status == Paused) {
        cleanup();
        // Note: This would need the QNetworkAccessManager reference
        // For now, just change status
        setStatus(Queued);
    }
}

void DownloadTask::cancel()
{
    if (m_reply) {
        m_reply->abort();
    }
    cleanup();
    setStatus(Cancelled);
}

void DownloadTask::onReadyRead()
{
    if (!m_file || !m_reply) {
        return;
    }
    
    QByteArray data = m_reply->readAll();
    if (m_hash) {
        m_hash->addData(data);
    }
    
    qint64 written = m_file->write(data);
    if (written != data.size()) {
        qCWarning(downloadTask) << "Failed to write all data to file";
        onError(QNetworkReply::UnknownContentError);
    }
}

void DownloadTask::onFinished()
{
    if (!m_reply) {
        return;
    }
    
    if (m_reply->error() != QNetworkReply::NoError) {
        qCWarning(downloadTask) << "Download finished with error:" << m_reply->errorString();
        return; // Error will be handled by onError
    }
    
    // Read any remaining data
    onReadyRead();
    
    if (m_file) {
        m_file->close();
    }
    
    // Verify SHA1 if expected
    if (!m_expectedSha1.isEmpty() && !verifySha1()) {
        qCWarning(downloadTask) << "SHA1 verification failed for:" << m_filePath;
        setStatus(Failed);
        emit error("SHA1 verification failed");
        return;
    }
    
    qCInfo(downloadTask) << "Download completed successfully:" << m_filePath;
    setStatus(Completed);
    emit finished();
    
    cleanup();
}

void DownloadTask::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    m_downloadedBytes = bytesReceived;
    m_totalBytes = bytesTotal;
    
    if (bytesTotal > 0) {
        double newProgress = static_cast<double>(bytesReceived) / bytesTotal;
        setProgress(newProgress);
    }
    
    // Calculate download speed
    qint64 elapsed = m_speedTimer.elapsed();
    if (elapsed > 1000) { // Update speed every second
        qint64 bytesDiff = bytesReceived - m_lastBytes;
        m_currentSpeed = (bytesDiff * 1000.0) / elapsed; // bytes per second
        m_lastBytes = bytesReceived;
        m_speedTimer.restart();
    }
    
    emit progressChanged();
}

void DownloadTask::onError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error)
    
    if (!m_reply) {
        return;
    }
    
    QString errorString = m_reply->errorString();
    qCWarning(downloadTask) << "Download error:" << errorString;
    
    setStatus(Failed);
    emit this->error(errorString);
    
    cleanup();
}

void DownloadTask::setStatus(Status status)
{
    if (m_status != status) {
        m_status = status;
        emit statusChanged();
    }
}

void DownloadTask::setProgress(double progress)
{
    if (qAbs(m_progress - progress) > 0.001) { // Avoid too frequent updates
        m_progress = progress;
        emit progressChanged();
    }
}

bool DownloadTask::verifySha1()
{
    if (!m_hash || m_expectedSha1.isEmpty()) {
        return true; // No verification needed
    }
    
    QString actualSha1 = m_hash->result().toHex().toLower();
    QString expectedSha1 = m_expectedSha1.toLower();
    
    return actualSha1 == expectedSha1;
}

void DownloadTask::cleanup()
{
    if (m_reply) {
        m_reply->deleteLater();
        m_reply = nullptr;
    }
    
    if (m_file) {
        m_file->close();
        m_file->deleteLater();
        m_file = nullptr;
    }
    
    if (m_hash) {
        delete m_hash;
        m_hash = nullptr;
    }
}