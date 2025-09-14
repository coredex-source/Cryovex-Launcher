#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QJSEngine>
#include <QQuickStyle>
#include <QDir>
#include <QStandardPaths>
#include <QLoggingCategory>
#include <QFile>
#include <QTextStream>

// Module headers
#include "auth/AuthManager.h"
#include "config/ConfigManager.h"
#include "utils/Logger.h"

Q_LOGGING_CATEGORY(appMain, "cryovex.main")

void loadEnvironmentFile(const QString& filePath)
{
    // Try multiple possible locations for .env file
    QStringList possiblePaths = {
        filePath,                                          // Current directory
        QDir::currentPath() + "/" + filePath,             // Explicit current path
        QCoreApplication::applicationDirPath() + "/" + filePath, // App directory
        QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("../../" + filePath) // Source directory
    };
    
    QFile file;
    QString foundPath;
    
    for (const QString& path : possiblePaths) {
        file.setFileName(path);
        if (file.exists()) {
            foundPath = path;
            break;
        }
    }
    
    if (foundPath.isEmpty()) {
        qCWarning(appMain) << "No .env file found. Tried paths:";
        for (const QString& path : possiblePaths) {
            qCWarning(appMain) << " -" << QDir::toNativeSeparators(path);
        }
        return;
    }
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCWarning(appMain) << "Failed to open .env file at:" << foundPath;
        return;
    }
    
    QTextStream in(&file);
    qCInfo(appMain) << "Loading environment variables from:" << QDir::toNativeSeparators(foundPath);
    
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        
        // Skip empty lines and comments
        if (line.isEmpty() || line.startsWith('#')) {
            continue;
        }
        
        // Parse KEY=VALUE format
        int equalIndex = line.indexOf('=');
        if (equalIndex != -1) {
            QString key = line.left(equalIndex).trimmed();
            QString value = line.mid(equalIndex + 1).trimmed();
            
            // Remove quotes if present
            if (value.startsWith('"') && value.endsWith('"')) {
                value = value.mid(1, value.length() - 2);
            }
            
            qputenv(key.toUtf8(), value.toUtf8());
            qCInfo(appMain) << "Loaded env var:" << key << "=" << (key.contains("SECRET") || key.contains("KEY") || key.contains("ID") ? "[HIDDEN]" : value);
            
            // Debug: Verify the variable was actually set
            QString verifyValue = qEnvironmentVariable(key.toUtf8());
            qCInfo(appMain) << "Verification - env var" << key << "is now:" << (verifyValue.isEmpty() ? "EMPTY" : "SET");
        }
    }
}

int main(int argc, char *argv[])
{
    // Enable GPU acceleration for WebEngine
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--enable-gpu --enable-gpu-compositing --enable-gpu-rasterization --enable-zero-copy --ignore-gpu-blacklist --disable-gpu-sandbox");
    
    QApplication app(argc, argv);
    
    // Debug: Show current working directory and app path
    qCInfo(appMain) << "Current working directory:" << QDir::toNativeSeparators(QDir::currentPath());
    qCInfo(appMain) << "Application directory:" << QDir::toNativeSeparators(QCoreApplication::applicationDirPath());
    
    // Load environment variables from .env file (if it exists)
    loadEnvironmentFile(".env");
    
    // Set application properties
    app.setApplicationName("Cryovex Launcher");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Cryovex");
    
    // Set the Qt Quick style (Material Design)
    QQuickStyle::setStyle("Material");
    
    // Initialize logging
    Logger::initialize();
    
    qCInfo(appMain) << "Starting Cryovex Launcher v1.0.0";
    
    // Create application data directory
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(appDataPath);
    qCInfo(appMain) << "App data directory:" << appDataPath;
    
    // Initialize configuration manager
    ConfigManager::instance().initialize();
    
    // Initialize authentication manager
    AuthManager::instance().initialize();
    
    // Set up QML engine
    QQmlApplicationEngine engine;
    
    // Register custom types for QML
    qmlRegisterSingletonType<AuthManager>("CryovexLauncher", 1, 0, "AuthManager", 
        [](QQmlEngine*, QJSEngine*) -> QObject* {
            return &AuthManager::instance();
        });
    
    qmlRegisterSingletonType<ConfigManager>("CryovexLauncher", 1, 0, "ConfigManager",
        [](QQmlEngine*, QJSEngine*) -> QObject* {
            return &ConfigManager::instance();
        });
    
    // Load the main QML file
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    
    engine.load(url);
    
    qCInfo(appMain) << "Application started successfully";
    
    return app.exec();
}