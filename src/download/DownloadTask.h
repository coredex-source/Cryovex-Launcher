#pragma once

#include <QObject>
#include <QNetworkReply>
#include <QFile>
#include <QCryptographicHash>
#include <QElapsedTimer>
#include <QUrl>

class DownloadTask : public QObject
{
    Q_OBJECT

public:
    enum Status {
        Queued,
        Downloading,
        Paused,
        Completed,
        Failed,
        Cancelled
    };
    Q_ENUM(Status)

    explicit DownloadTask(const QUrl& url, const QString& filePath, 
                         const QString& expectedSha1 = QString(), 
                         QObject *parent = nullptr);
    ~DownloadTask();
    
    QUrl url() const { return m_url; }
    QString filePath() const { return m_filePath; }
    QString expectedSha1() const { return m_expectedSha1; }
    Status status() const { return m_status; }
    double progress() const { return m_progress; }
    qint64 downloadedBytes() const { return m_downloadedBytes; }
    qint64 totalBytes() const { return m_totalBytes; }
    double downloadSpeed() const; // bytes per second
    
    void start(QNetworkAccessManager* manager);
    void pause();
    void resume();
    void cancel();

signals:
    void progressChanged();
    void statusChanged();
    void finished();
    void error(const QString& errorString);

private slots:
    void onReadyRead();
    void onFinished();
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onError(QNetworkReply::NetworkError error);

private:
    void setStatus(Status status);
    void setProgress(double progress);
    bool verifySha1();
    void cleanup();
    
    QUrl m_url;
    QString m_filePath;
    QString m_expectedSha1;
    Status m_status = Queued;
    double m_progress = 0.0;
    qint64 m_downloadedBytes = 0;
    qint64 m_totalBytes = 0;
    
    QNetworkReply* m_reply = nullptr;
    QFile* m_file = nullptr;
    QCryptographicHash* m_hash = nullptr;
    QElapsedTimer m_speedTimer;
    qint64 m_lastBytes = 0;
    double m_currentSpeed = 0.0;
};