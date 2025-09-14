#include "Logger.h"
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>
#include <iostream>

Q_LOGGING_CATEGORY(logger, "cryovex.logger")

Logger::LogLevel Logger::s_logLevel = Logger::Info;
bool Logger::s_logToFile = true;
QString Logger::s_logFilePath;
QFile Logger::s_logFile;
QTextStream Logger::s_logStream;
QMutex Logger::s_logMutex;

void Logger::initialize()
{
    // Set default log file path
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(appDataPath);
    s_logFilePath = QDir(appDataPath).filePath("cryovex_launcher.log");
    
    if (s_logToFile) {
        s_logFile.setFileName(s_logFilePath);
        if (s_logFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
            s_logStream.setDevice(&s_logFile);
        }
    }
    
    // Install our custom message handler
    qInstallMessageHandler(messageHandler);
    
    qCInfo(logger) << "Logger initialized, log file:" << s_logFilePath;
}

void Logger::setLogLevel(LogLevel level)
{
    s_logLevel = level;
}

void Logger::setLogToFile(bool enabled)
{
    QMutexLocker locker(&s_logMutex);
    
    if (s_logToFile != enabled) {
        s_logToFile = enabled;
        
        if (enabled && !s_logFile.isOpen()) {
            s_logFile.setFileName(s_logFilePath);
            if (s_logFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
                s_logStream.setDevice(&s_logFile);
            }
        } else if (!enabled && s_logFile.isOpen()) {
            s_logFile.close();
        }
    }
}

void Logger::setLogFilePath(const QString& filePath)
{
    QMutexLocker locker(&s_logMutex);
    
    if (s_logFilePath != filePath) {
        if (s_logFile.isOpen()) {
            s_logFile.close();
        }
        
        s_logFilePath = filePath;
        
        if (s_logToFile) {
            s_logFile.setFileName(s_logFilePath);
            if (s_logFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
                s_logStream.setDevice(&s_logFile);
            }
        }
    }
}

void Logger::messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
    // Check log level
    LogLevel messageLevel;
    switch (type) {
    case QtDebugMsg:
        messageLevel = Debug;
        break;
    case QtInfoMsg:
        messageLevel = Info;
        break;
    case QtWarningMsg:
        messageLevel = Warning;
        break;
    case QtCriticalMsg:
        messageLevel = Critical;
        break;
    case QtFatalMsg:
        messageLevel = Fatal;
        break;
    }
    
    if (messageLevel < s_logLevel) {
        return;
    }
    
    QString formattedMessage = formatMessage(type, context, message);
    
    // Print to console
    std::cout << formattedMessage.toStdString() << std::endl;
    
    // Write to file if enabled
    if (s_logToFile) {
        writeToFile(formattedMessage);
    }
}

void Logger::writeToFile(const QString& message)
{
    QMutexLocker locker(&s_logMutex);
    
    if (s_logFile.isOpen()) {
        s_logStream << message << Qt::endl;
        s_logStream.flush();
    }
}

QString Logger::formatMessage(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
    QString typeStr;
    switch (type) {
    case QtDebugMsg:
        typeStr = "DEBUG";
        break;
    case QtInfoMsg:
        typeStr = "INFO ";
        break;
    case QtWarningMsg:
        typeStr = "WARN ";
        break;
    case QtCriticalMsg:
        typeStr = "ERROR";
        break;
    case QtFatalMsg:
        typeStr = "FATAL";
        break;
    }
    
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString category = context.category ? QString(context.category) : "default";
    
    return QString("[%1] %2 %3: %4")
           .arg(timestamp)
           .arg(typeStr)
           .arg(category)
           .arg(message);
}