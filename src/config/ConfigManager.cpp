#include "ConfigManager.h"
#include "Profile.h"
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(configManager, "cryovex.config.manager")

ConfigManager& ConfigManager::instance()
{
    static ConfigManager instance;
    return instance;
}

ConfigManager::ConfigManager(QObject *parent)
    : QAbstractListModel(parent)
{
    // Set default values
    m_gameDirectory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/.minecraft";
    m_javaPath = "java"; // Auto-detect
    m_memoryMB = 2048;
}

void ConfigManager::initialize()
{
    qCInfo(configManager) << "Initializing ConfigManager";
    loadSettings();
    loadProfiles();
}

void ConfigManager::save()
{
    saveSettings();
    saveProfiles();
}

int ConfigManager::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_profiles.size();
}

QVariant ConfigManager::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_profiles.size()) {
        return QVariant();
    }
    
    Profile* profile = m_profiles.at(index.row());
    
    switch (role) {
    case NameRole:
        return profile->name();
    case UuidRole:
        return profile->uuid();
    case UsernameRole:
        return profile->username();
    case LastVersionRole:
        return profile->lastVersion();
    case LastPlayedRole:
        return profile->lastPlayed();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> ConfigManager::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[UuidRole] = "uuid";
    roles[UsernameRole] = "username";
    roles[LastVersionRole] = "lastVersion";
    roles[LastPlayedRole] = "lastPlayed";
    return roles;
}

void ConfigManager::addProfile(const QString& username, const QString& uuid)
{
    beginInsertRows(QModelIndex(), m_profiles.size(), m_profiles.size());
    Profile* profile = new Profile(username, uuid, this);
    m_profiles.append(profile);
    endInsertRows();
    
    emit profileAdded(profile);
    saveProfiles();
}

void ConfigManager::removeProfile(const QString& uuid)
{
    for (int i = 0; i < m_profiles.size(); ++i) {
        if (m_profiles[i]->uuid() == uuid) {
            beginRemoveRows(QModelIndex(), i, i);
            Profile* profile = m_profiles.takeAt(i);
            if (profile == m_currentProfile) {
                m_currentProfile = nullptr;
                emit currentProfileChanged();
            }
            profile->deleteLater();
            endRemoveRows();
            
            emit profileRemoved(uuid);
            saveProfiles();
            break;
        }
    }
}

void ConfigManager::setCurrentProfile(const QString& uuid)
{
    Profile* profile = getProfile(uuid);
    if (profile && profile != m_currentProfile) {
        m_currentProfile = profile;
        emit currentProfileChanged();
        saveSettings();
    }
}

Profile* ConfigManager::getProfile(const QString& uuid) const
{
    for (Profile* profile : m_profiles) {
        if (profile->uuid() == uuid) {
            return profile;
        }
    }
    return nullptr;
}

void ConfigManager::setGameDirectory(const QString& directory)
{
    if (m_gameDirectory != directory) {
        m_gameDirectory = directory;
        emit gameDirectoryChanged();
        saveSettings();
    }
}

void ConfigManager::setJavaPath(const QString& path)
{
    if (m_javaPath != path) {
        m_javaPath = path;
        emit javaPathChanged();
        saveSettings();
    }
}

void ConfigManager::setMemoryMB(int memory)
{
    if (m_memoryMB != memory) {
        m_memoryMB = memory;
        emit memoryMBChanged();
        saveSettings();
    }
}

void ConfigManager::loadSettings()
{
    QString settingsPath = getSettingsFilePath();
    QFile file(settingsPath);
    
    if (!file.exists()) {
        qCInfo(configManager) << "No settings file found, using defaults";
        return;
    }
    
    if (!file.open(QIODevice::ReadOnly)) {
        qCWarning(configManager) << "Failed to open settings file:" << file.errorString();
        return;
    }
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qCWarning(configManager) << "Failed to parse settings:" << error.errorString();
        return;
    }
    
    QJsonObject settings = doc.object();
    m_gameDirectory = settings["gameDirectory"].toString(m_gameDirectory);
    m_javaPath = settings["javaPath"].toString(m_javaPath);
    m_memoryMB = settings["memoryMB"].toInt(m_memoryMB);
    
    QString currentProfileUuid = settings["currentProfile"].toString();
    if (!currentProfileUuid.isEmpty()) {
        // Will be set after profiles are loaded
    }
}

void ConfigManager::saveSettings()
{
    QJsonObject settings;
    settings["gameDirectory"] = m_gameDirectory;
    settings["javaPath"] = m_javaPath;
    settings["memoryMB"] = m_memoryMB;
    if (m_currentProfile) {
        settings["currentProfile"] = m_currentProfile->uuid();
    }
    
    QString settingsPath = getSettingsFilePath();
    QDir().mkpath(QFileInfo(settingsPath).dir().path());
    
    QFile file(settingsPath);
    if (!file.open(QIODevice::WriteOnly)) {
        qCWarning(configManager) << "Failed to save settings:" << file.errorString();
        return;
    }
    
    QJsonDocument doc(settings);
    file.write(doc.toJson());
}

void ConfigManager::loadProfiles()
{
    // Stub implementation
    qCInfo(configManager) << "Loading profiles (stub)";
}

void ConfigManager::saveProfiles()
{
    // Stub implementation
    qCInfo(configManager) << "Saving profiles (stub)";
}

QString ConfigManager::getConfigDirectory() const
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

QString ConfigManager::getSettingsFilePath() const
{
    return QDir(getConfigDirectory()).filePath("settings.json");
}

QString ConfigManager::getProfilesFilePath() const
{
    return QDir(getConfigDirectory()).filePath("profiles.json");
}