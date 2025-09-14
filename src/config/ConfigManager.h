#pragma once

#include <QObject>
#include <QJsonObject>
#include <QAbstractListModel>
#include "Profile.h"

class ConfigManager : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(Profile* currentProfile READ currentProfile NOTIFY currentProfileChanged)
    Q_PROPERTY(QString gameDirectory READ gameDirectory WRITE setGameDirectory NOTIFY gameDirectoryChanged)
    Q_PROPERTY(QString javaPath READ javaPath WRITE setJavaPath NOTIFY javaPathChanged)
    Q_PROPERTY(int memoryMB READ memoryMB WRITE setMemoryMB NOTIFY memoryMBChanged)

public:
    enum ProfileRoles {
        NameRole = Qt::UserRole + 1,
        UuidRole,
        UsernameRole,
        LastVersionRole,
        LastPlayedRole
    };

    static ConfigManager& instance();
    
    void initialize();
    void save();
    
    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    
    Profile* currentProfile() const { return m_currentProfile; }
    QString gameDirectory() const { return m_gameDirectory; }
    QString javaPath() const { return m_javaPath; }
    int memoryMB() const { return m_memoryMB; }
    
    Q_INVOKABLE void addProfile(const QString& username, const QString& uuid);
    Q_INVOKABLE void removeProfile(const QString& uuid);
    Q_INVOKABLE void setCurrentProfile(const QString& uuid);
    Q_INVOKABLE Profile* getProfile(const QString& uuid) const;

public slots:
    void setGameDirectory(const QString& directory);
    void setJavaPath(const QString& path);
    void setMemoryMB(int memory);

signals:
    void currentProfileChanged();
    void gameDirectoryChanged();
    void javaPathChanged();
    void memoryMBChanged();
    void profileAdded(Profile* profile);
    void profileRemoved(const QString& uuid);

private:
    explicit ConfigManager(QObject *parent = nullptr);
    ~ConfigManager() = default;
    
    void loadSettings();
    void saveSettings();
    void loadProfiles();
    void saveProfiles();
    
    QString getConfigDirectory() const;
    QString getSettingsFilePath() const;
    QString getProfilesFilePath() const;
    
    QList<Profile*> m_profiles;
    Profile* m_currentProfile = nullptr;
    QString m_gameDirectory;
    QString m_javaPath;
    int m_memoryMB = 2048;
};