#include "MinecraftVersion.h"

MinecraftVersion::MinecraftVersion(QObject *parent)
    : QObject(parent)
    , m_type(Unknown)
{
}

MinecraftVersion::MinecraftVersion(const QString& id, const QString& type, 
                                  const QDateTime& releaseTime, const QUrl& url, 
                                  QObject *parent)
    : QObject(parent)
    , m_id(id)
    , m_typeString(type)
    , m_type(parseVersionType(type))
    , m_releaseTime(releaseTime)
    , m_url(url)
{
}

MinecraftVersion::VersionType MinecraftVersion::parseVersionType(const QString& typeString)
{
    if (typeString == "release") {
        return Release;
    } else if (typeString == "snapshot") {
        return Snapshot;
    } else if (typeString == "beta") {
        return Beta;
    } else if (typeString == "alpha") {
        return Alpha;
    }
    return Unknown;
}