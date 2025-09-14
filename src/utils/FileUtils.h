#pragma once

#include <QObject>
#include <QString>
#include <QDir>
#include <QFileInfo>
#include <QCryptographicHash>

class FileUtils : public QObject
{
    Q_OBJECT

public:
    static bool ensureDirectoryExists(const QString& dirPath);
    static bool copyFile(const QString& sourcePath, const QString& destPath, bool overwrite = true);
    static bool moveFile(const QString& sourcePath, const QString& destPath);
    static bool deleteFile(const QString& filePath);
    static bool deleteDirectory(const QString& dirPath, bool recursive = true);
    
    static QString calculateSha1(const QString& filePath);
    static bool verifySha1(const QString& filePath, const QString& expectedSha1);
    
    static qint64 getFileSize(const QString& filePath);
    static qint64 getDirectorySize(const QString& dirPath, bool recursive = true);
    
    static QString getMinecraftDirectory();
    static QString getApplicationDataDirectory();
    static QString joinPath(const QString& path1, const QString& path2);
    static QString joinPaths(const QStringList& paths);
    
    static bool isValidFileName(const QString& fileName);
    static QString sanitizeFileName(const QString& fileName);
    
    static QStringList findFiles(const QString& dirPath, const QStringList& nameFilters, 
                                bool recursive = true);

private:
    explicit FileUtils(QObject *parent = nullptr) : QObject(parent) {}
};