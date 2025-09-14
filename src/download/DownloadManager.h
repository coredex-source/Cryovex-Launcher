#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QQueue>
#include <QTimer>
#include <QAbstractListModel>

class DownloadTask;

class DownloadManager : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int activeDownloads READ activeDownloads NOTIFY activeDownloadsChanged)
    Q_PROPERTY(int queuedDownloads READ queuedDownloads NOTIFY queuedDownloadsChanged)
    Q_PROPERTY(double totalProgress READ totalProgress NOTIFY totalProgressChanged)
    Q_PROPERTY(bool isDownloading READ isDownloading NOTIFY downloadingStatusChanged)

public:
    enum DownloadRoles {
        UrlRole = Qt::UserRole + 1,
        FilePathRole,
        ProgressRole,
        StatusRole,
        SpeedRole,
        SizeRole
    };

    explicit DownloadManager(QObject *parent = nullptr);
    
    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    
    int activeDownloads() const { return m_activeDownloads.size(); }
    int queuedDownloads() const { return m_queuedDownloads.size(); }
    double totalProgress() const;
    bool isDownloading() const { return !m_activeDownloads.isEmpty() || !m_queuedDownloads.isEmpty(); }
    
    Q_INVOKABLE void addDownload(const QString& url, const QString& filePath, 
                                const QString& expectedSha1 = QString());
    Q_INVOKABLE void pauseAll();
    Q_INVOKABLE void resumeAll();
    Q_INVOKABLE void cancelAll();
    Q_INVOKABLE void setMaxConcurrentDownloads(int max);

signals:
    void activeDownloadsChanged();
    void queuedDownloadsChanged();
    void totalProgressChanged();
    void downloadingStatusChanged();
    void downloadCompleted(const QString& filePath);
    void downloadFailed(const QString& url, const QString& error);
    void allDownloadsCompleted();

private slots:
    void onDownloadFinished();
    void onDownloadProgress();
    void processQueue();

private:
    void startNextDownload();
    void removeCompletedDownloads();
    
    QNetworkAccessManager* m_networkManager;
    QList<DownloadTask*> m_activeDownloads;
    QQueue<DownloadTask*> m_queuedDownloads;
    QList<DownloadTask*> m_allTasks; // For model interface
    
    int m_maxConcurrentDownloads = 4;
    QTimer* m_progressTimer;
};