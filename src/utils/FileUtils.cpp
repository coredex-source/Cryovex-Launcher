#include "FileUtils.h"
#include <QStandardPaths>
#include <QCryptographicHash>
#include <QDirIterator>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(fileUtils, "cryovex.utils.file")

bool FileUtils::ensureDirectoryExists(const QString& dirPath)
{
    QDir dir;
    return dir.mkpath(dirPath);
}

bool FileUtils::copyFile(const QString& sourcePath, const QString& destPath, bool overwrite)
{
    if (QFile::exists(destPath) && !overwrite) {
        return false;
    }
    
    if (QFile::exists(destPath)) {
        QFile::remove(destPath);
    }
    
    return QFile::copy(sourcePath, destPath);
}

bool FileUtils::moveFile(const QString& sourcePath, const QString& destPath)
{
    return QFile::rename(sourcePath, destPath);
}

bool FileUtils::deleteFile(const QString& filePath)
{
    return QFile::remove(filePath);
}

bool FileUtils::deleteDirectory(const QString& dirPath, bool recursive)
{
    QDir dir(dirPath);
    
    if (recursive) {
        return dir.removeRecursively();
    } else {
        return dir.rmdir(dirPath);
    }
}

QString FileUtils::calculateSha1(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qCWarning(fileUtils) << "Failed to open file for SHA1 calculation:" << filePath;
        return QString();
    }
    
    QCryptographicHash hash(QCryptographicHash::Sha1);
    if (!hash.addData(&file)) {
        qCWarning(fileUtils) << "Failed to calculate SHA1 for file:" << filePath;
        return QString();
    }
    
    return hash.result().toHex();
}

bool FileUtils::verifySha1(const QString& filePath, const QString& expectedSha1)
{
    QString actualSha1 = calculateSha1(filePath);
    return actualSha1.compare(expectedSha1, Qt::CaseInsensitive) == 0;
}

qint64 FileUtils::getFileSize(const QString& filePath)
{
    QFileInfo info(filePath);
    return info.size();
}

qint64 FileUtils::getDirectorySize(const QString& dirPath, bool recursive)
{
    qint64 totalSize = 0;
    
    QDirIterator it(dirPath, QDir::Files, recursive ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags);
    while (it.hasNext()) {
        it.next();
        totalSize += it.fileInfo().size();
    }
    
    return totalSize;
}

QString FileUtils::getMinecraftDirectory()
{
    QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return QDir(appData).filePath(".minecraft");
}

QString FileUtils::getApplicationDataDirectory()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

QString FileUtils::joinPath(const QString& path1, const QString& path2)
{
    return QDir(path1).filePath(path2);
}

QString FileUtils::joinPaths(const QStringList& paths)
{
    if (paths.isEmpty()) {
        return QString();
    }
    
    QString result = paths.first();
    for (int i = 1; i < paths.size(); ++i) {
        result = joinPath(result, paths[i]);
    }
    
    return result;
}

bool FileUtils::isValidFileName(const QString& fileName)
{
    // Check for invalid characters on Windows
    const QString invalidChars = "<>:\"/\\|?*";
    for (const QChar& ch : invalidChars) {
        if (fileName.contains(ch)) {
            return false;
        }
    }
    
    // Check for reserved names on Windows
    const QStringList reservedNames = {
        "CON", "PRN", "AUX", "NUL",
        "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
        "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"
    };
    
    return !reservedNames.contains(fileName.toUpper());
}

QString FileUtils::sanitizeFileName(const QString& fileName)
{
    QString result = fileName;
    const QString invalidChars = "<>:\"/\\|?*";
    
    for (const QChar& ch : invalidChars) {
        result.replace(ch, '_');
    }
    
    return result;
}

QStringList FileUtils::findFiles(const QString& dirPath, const QStringList& nameFilters, bool recursive)
{
    QStringList files;
    
    QDirIterator it(dirPath, nameFilters, QDir::Files, 
                   recursive ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags);
    
    while (it.hasNext()) {
        files << it.next();
    }
    
    return files;
}