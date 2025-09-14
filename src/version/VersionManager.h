#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QAbstractListModel>
#include <QDateTime>

class MinecraftVersion;

class VersionManager : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY loadingStatusChanged)
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    enum VersionRoles {
        IdRole = Qt::UserRole + 1,
        TypeRole,
        ReleaseTimeRole,
        UrlRole
    };

    explicit VersionManager(QObject *parent = nullptr);
    
    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    
    bool isLoading() const { return m_isLoading; }
    
    Q_INVOKABLE void refreshVersions();
    Q_INVOKABLE MinecraftVersion* getVersion(const QString& versionId) const;
    Q_INVOKABLE void downloadVersionManifest(const QString& versionId);

signals:
    void loadingStatusChanged();
    void countChanged();
    void versionsLoaded();
    void versionManifestDownloaded(const QString& versionId, const QJsonObject& manifest);
    void errorOccurred(const QString& error);

private slots:
    void onVersionManifestReply();
    void onVersionJsonReply();

private:
    void setLoading(bool loading);
    void parseVersionManifest(const QJsonObject& manifest);
    
    QNetworkAccessManager* m_networkManager;
    QList<MinecraftVersion*> m_versions;
    bool m_isLoading = false;
    
    static const QString VERSION_MANIFEST_URL;
};