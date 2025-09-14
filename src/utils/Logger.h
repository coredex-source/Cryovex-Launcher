#pragma once

#include <QObject>
#include <QLoggingCategory>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>

Q_DECLARE_LOGGING_CATEGORY(logger)

class Logger : public QObject
{
    Q_OBJECT

public:
    enum LogLevel {
        Debug,
        Info,
        Warning,
        Critical,
        Fatal
    };
    Q_ENUM(LogLevel)

    static void initialize();
    static void setLogLevel(LogLevel level);
    static void setLogToFile(bool enabled);
    static void setLogFilePath(const QString& filePath);
    
    static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message);

private:
    static void writeToFile(const QString& message);
    static QString formatMessage(QtMsgType type, const QMessageLogContext& context, const QString& message);
    
    static LogLevel s_logLevel;
    static bool s_logToFile;
    static QString s_logFilePath;
    static QFile s_logFile;
    static QTextStream s_logStream;
    static QMutex s_logMutex;
};