#pragma once

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QUrl>
#include <QJsonObject>

class MinecraftVersion : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString type READ type CONSTANT)
    Q_PROPERTY(QDateTime releaseTime READ releaseTime CONSTANT)
    Q_PROPERTY(QUrl url READ url CONSTANT)

public:
    enum VersionType {
        Release,
        Snapshot,
        Beta,
        Alpha,
        Unknown
    };
    Q_ENUM(VersionType)

    explicit MinecraftVersion(QObject *parent = nullptr);
    MinecraftVersion(const QString& id, const QString& type, 
                    const QDateTime& releaseTime, const QUrl& url, 
                    QObject *parent = nullptr);

    QString id() const { return m_id; }
    QString type() const { return m_typeString; }
    VersionType versionType() const { return m_type; }
    QDateTime releaseTime() const { return m_releaseTime; }
    QUrl url() const { return m_url; }
    
    QJsonObject manifest() const { return m_manifest; }
    void setManifest(const QJsonObject& manifest) { m_manifest = manifest; }
    
    bool hasManifest() const { return !m_manifest.isEmpty(); }

private:
    static VersionType parseVersionType(const QString& typeString);

    QString m_id;
    QString m_typeString;
    VersionType m_type;
    QDateTime m_releaseTime;
    QUrl m_url;
    QJsonObject m_manifest;
};