#include "NetworkUtils.h"
#include <QNetworkRequest>
#include <QJsonParseError>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(networkUtils, "cryovex.utils.network")

QString NetworkUtils::s_userAgent = "CryovexLauncher/1.0.0";

// Minecraft URLs
const QString NetworkUtils::MINECRAFT_VERSION_MANIFEST_URL = "https://piston-meta.mojang.com/mc/game/version_manifest_v2.json";
const QString NetworkUtils::MINECRAFT_RESOURCES_URL = "https://resources.download.minecraft.net";
const QString NetworkUtils::MINECRAFT_LIBRARIES_URL = "https://libraries.minecraft.net";

// Microsoft/Xbox Live URLs
const QString NetworkUtils::MICROSOFT_AUTH_URL = "https://login.microsoftonline.com/consumers/oauth2/v2.0";
const QString NetworkUtils::XBOX_LIVE_AUTH_URL = "https://user.auth.xboxlive.com/user/authenticate";
const QString NetworkUtils::XSTS_AUTH_URL = "https://xsts.auth.xboxlive.com/xsts/authorize";
const QString NetworkUtils::MINECRAFT_AUTH_URL = "https://api.minecraftservices.com/authentication/login_with_xbox";
const QString NetworkUtils::MINECRAFT_PROFILE_URL = "https://api.minecraftservices.com/minecraft/profile";

QNetworkRequest NetworkUtils::createRequest(const QUrl& url)
{
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", s_userAgent.toUtf8());
    return request;
}

QNetworkRequest NetworkUtils::createJsonRequest(const QUrl& url)
{
    QNetworkRequest request = createRequest(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "application/json");
    return request;
}

QNetworkRequest NetworkUtils::createAuthenticatedRequest(const QUrl& url, const QString& accessToken)
{
    QNetworkRequest request = createRequest(url);
    request.setRawHeader("Authorization", ("Bearer " + accessToken).toUtf8());
    return request;
}

QJsonObject NetworkUtils::parseJsonResponse(QNetworkReply* reply, bool* ok)
{
    if (ok) *ok = false;
    
    if (!reply) {
        qCWarning(networkUtils) << "Null reply";
        return QJsonObject();
    }
    
    if (reply->error() != QNetworkReply::NoError) {
        qCWarning(networkUtils) << "Network error:" << reply->errorString();
        return QJsonObject();
    }
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &error);
    
    if (error.error != QJsonParseError::NoError) {
        qCWarning(networkUtils) << "JSON parse error:" << error.errorString();
        return QJsonObject();
    }
    
    if (ok) *ok = true;
    return doc.object();
}

QString NetworkUtils::getErrorString(QNetworkReply* reply)
{
    if (!reply) {
        return "Null reply";
    }
    
    if (reply->error() == QNetworkReply::NoError) {
        return QString();
    }
    
    QString errorString = reply->errorString();
    
    // Add HTTP status code if available
    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (httpStatus > 0) {
        errorString += QString(" (HTTP %1)").arg(httpStatus);
    }
    
    return errorString;
}

bool NetworkUtils::isNetworkError(QNetworkReply::NetworkError error)
{
    return error != QNetworkReply::NoError;
}

bool NetworkUtils::isHttpError(int httpStatus)
{
    return httpStatus >= 400;
}

QString NetworkUtils::getUserAgent()
{
    return s_userAgent;
}

void NetworkUtils::setUserAgent(const QString& userAgent)
{
    s_userAgent = userAgent;
}