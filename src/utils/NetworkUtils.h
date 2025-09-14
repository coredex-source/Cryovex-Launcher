#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonObject>
#include <QJsonDocument>

class NetworkUtils : public QObject
{
    Q_OBJECT

public:
    static QNetworkRequest createRequest(const QUrl& url);
    static QNetworkRequest createJsonRequest(const QUrl& url);
    static QNetworkRequest createAuthenticatedRequest(const QUrl& url, const QString& accessToken);
    
    static QJsonObject parseJsonResponse(QNetworkReply* reply, bool* ok = nullptr);
    static QString getErrorString(QNetworkReply* reply);
    
    static bool isNetworkError(QNetworkReply::NetworkError error);
    static bool isHttpError(int httpStatus);
    
    static QString getUserAgent();
    static void setUserAgent(const QString& userAgent);
    
    // Minecraft-specific URLs
    static const QString MINECRAFT_VERSION_MANIFEST_URL;
    static const QString MINECRAFT_RESOURCES_URL;
    static const QString MINECRAFT_LIBRARIES_URL;
    
    // Microsoft/Xbox Live URLs
    static const QString MICROSOFT_AUTH_URL;
    static const QString XBOX_LIVE_AUTH_URL;
    static const QString XSTS_AUTH_URL;
    static const QString MINECRAFT_AUTH_URL;
    static const QString MINECRAFT_PROFILE_URL;

private:
    explicit NetworkUtils(QObject *parent = nullptr) : QObject(parent) {}
    static QString s_userAgent;
};