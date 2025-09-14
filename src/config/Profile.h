#pragma once

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QJsonObject>

class Profile : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString uuid READ uuid CONSTANT)
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString lastVersion READ lastVersion WRITE setLastVersion NOTIFY lastVersionChanged)
    Q_PROPERTY(QDateTime lastPlayed READ lastPlayed WRITE setLastPlayed NOTIFY lastPlayedChanged)
    Q_PROPERTY(QString javaPath READ javaPath WRITE setJavaPath NOTIFY javaPathChanged)
    Q_PROPERTY(int memoryMB READ memoryMB WRITE setMemoryMB NOTIFY memoryMBChanged)

public:
    explicit Profile(QObject *parent = nullptr);
    Profile(const QString& username, const QString& uuid, QObject *parent = nullptr);
    
    QString name() const { return m_name; }
    QString uuid() const { return m_uuid; }
    QString username() const { return m_username; }
    QString lastVersion() const { return m_lastVersion; }
    QDateTime lastPlayed() const { return m_lastPlayed; }
    QString javaPath() const { return m_javaPath; }
    int memoryMB() const { return m_memoryMB; }
    
    void setName(const QString& name);
    void setUsername(const QString& username);
    void setLastVersion(const QString& version);
    void setLastPlayed(const QDateTime& dateTime);
    void setJavaPath(const QString& path);
    void setMemoryMB(int memory);
    
    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);

signals:
    void nameChanged();
    void usernameChanged();
    void lastVersionChanged();
    void lastPlayedChanged();
    void javaPathChanged();
    void memoryMBChanged();

private:
    QString m_name;
    QString m_uuid;
    QString m_username;
    QString m_lastVersion;
    QDateTime m_lastPlayed;
    QString m_javaPath;
    int m_memoryMB = 2048;
};