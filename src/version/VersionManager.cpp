#include "VersionManager.h"
#include "MinecraftVersion.h"
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(versionManager, "cryovex.version.manager")

const QString VersionManager::VERSION_MANIFEST_URL = "https://piston-meta.mojang.com/mc/game/version_manifest_v2.json";

VersionManager::VersionManager(QObject *parent)
    : QAbstractListModel(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
}

int VersionManager::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_versions.size();
}

QVariant VersionManager::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_versions.size()) {
        return QVariant();
    }
    
    MinecraftVersion* version = m_versions.at(index.row());
    
    switch (role) {
    case IdRole:
        return version->id();
    case TypeRole:
        return version->type();
    case ReleaseTimeRole:
        return version->releaseTime();
    case UrlRole:
        return version->url();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> VersionManager::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[TypeRole] = "type";
    roles[ReleaseTimeRole] = "releaseTime";
    roles[UrlRole] = "url";
    return roles;
}

void VersionManager::refreshVersions()
{
    qCInfo(versionManager) << "Refreshing versions (stub)";
    setLoading(true);
    
    // Stub: Add some dummy versions for testing
    beginResetModel();
    qDeleteAll(m_versions);
    m_versions.clear();
    
    // Add some test versions
    m_versions.append(new MinecraftVersion("1.20.1", "release", QDateTime::currentDateTime(), QUrl("https://example.com"), this));
    m_versions.append(new MinecraftVersion("1.20.2", "release", QDateTime::currentDateTime(), QUrl("https://example.com"), this));
    m_versions.append(new MinecraftVersion("23w51b", "snapshot", QDateTime::currentDateTime(), QUrl("https://example.com"), this));
    
    endResetModel();
    setLoading(false);
    emit versionsLoaded();
    emit countChanged();
}

MinecraftVersion* VersionManager::getVersion(const QString& versionId) const
{
    for (MinecraftVersion* version : m_versions) {
        if (version->id() == versionId) {
            return version;
        }
    }
    return nullptr;
}

void VersionManager::downloadVersionManifest(const QString& versionId)
{
    qCInfo(versionManager) << "Downloading version manifest for:" << versionId << "(stub)";
    // Stub implementation
}

void VersionManager::onVersionManifestReply()
{
    // Stub implementation
}

void VersionManager::onVersionJsonReply()
{
    // Stub implementation
}

void VersionManager::setLoading(bool loading)
{
    if (m_isLoading != loading) {
        m_isLoading = loading;
        emit loadingStatusChanged();
    }
}

void VersionManager::parseVersionManifest(const QJsonObject& manifest)
{
    // Stub implementation
    Q_UNUSED(manifest)
}