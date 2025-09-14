#include "Profile.h"
#include <QJsonObject>

Profile::Profile(QObject *parent)
    : QObject(parent)
{
}

Profile::Profile(const QString& username, const QString& uuid, QObject *parent)
    : QObject(parent)
    , m_name(username)
    , m_uuid(uuid)
    , m_username(username)
{
}

void Profile::setName(const QString& name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged();
    }
}

void Profile::setUsername(const QString& username)
{
    if (m_username != username) {
        m_username = username;
        emit usernameChanged();
    }
}

void Profile::setLastVersion(const QString& version)
{
    if (m_lastVersion != version) {
        m_lastVersion = version;
        emit lastVersionChanged();
    }
}

void Profile::setLastPlayed(const QDateTime& dateTime)
{
    if (m_lastPlayed != dateTime) {
        m_lastPlayed = dateTime;
        emit lastPlayedChanged();
    }
}

void Profile::setJavaPath(const QString& path)
{
    if (m_javaPath != path) {
        m_javaPath = path;
        emit javaPathChanged();
    }
}

void Profile::setMemoryMB(int memory)
{
    if (m_memoryMB != memory) {
        m_memoryMB = memory;
        emit memoryMBChanged();
    }
}

QJsonObject Profile::toJson() const
{
    QJsonObject json;
    json["name"] = m_name;
    json["uuid"] = m_uuid;
    json["username"] = m_username;
    json["lastVersion"] = m_lastVersion;
    json["lastPlayed"] = m_lastPlayed.toString(Qt::ISODate);
    json["javaPath"] = m_javaPath;
    json["memoryMB"] = m_memoryMB;
    return json;
}

void Profile::fromJson(const QJsonObject& json)
{
    setName(json["name"].toString());
    m_uuid = json["uuid"].toString(); // UUID shouldn't change
    setUsername(json["username"].toString());
    setLastVersion(json["lastVersion"].toString());
    setLastPlayed(QDateTime::fromString(json["lastPlayed"].toString(), Qt::ISODate));
    setJavaPath(json["javaPath"].toString());
    setMemoryMB(json["memoryMB"].toInt(2048));
}