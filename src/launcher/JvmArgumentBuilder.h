#pragma once

#include <QObject>
#include <QJsonObject>
#include <QStringList>
#include <QDir>

class JvmArgumentBuilder : public QObject
{
    Q_OBJECT

public:
    explicit JvmArgumentBuilder(QObject *parent = nullptr);
    
    QStringList buildArguments(const QJsonObject& versionManifest,
                              const QString& username,
                              const QString& uuid,
                              const QString& accessToken,
                              const QString& gameDirectory,
                              const QString& assetsDirectory,
                              const QString& versionName,
                              int memoryMB = 2048,
                              const QString& javaPath = "java");

private:
    QString replacePlaceholders(const QString& argument,
                               const QString& username,
                               const QString& uuid,
                               const QString& accessToken,
                               const QString& gameDirectory,
                               const QString& assetsDirectory,
                               const QString& versionName,
                               const QString& libraryDirectory,
                               const QString& classpath);
    
    QString buildClasspath(const QJsonObject& versionManifest, 
                          const QString& gameDirectory,
                          const QString& versionName);
    
    QStringList getJvmArguments(const QJsonObject& versionManifest, int memoryMB);
    QStringList getGameArguments(const QJsonObject& versionManifest);
    
    bool checkRule(const QJsonObject& rule);
    QString getNativesDirectory(const QString& gameDirectory, const QString& versionName);
    
    static const QStringList DEFAULT_JVM_ARGS;
};