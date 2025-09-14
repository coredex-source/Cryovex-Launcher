#include "AuthManager.h"
#include "MicrosoftAuth.h"
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(authManager, "cryovex.auth.manager")

AuthManager& AuthManager::instance()
{
    static AuthManager instance;
    return instance;
}

AuthManager::AuthManager(QObject *parent)
    : QObject(parent)
    , m_microsoftAuth(new MicrosoftAuth(this))
{
    connect(m_microsoftAuth, &MicrosoftAuth::authenticationCompleted,
            this, &AuthManager::onAuthenticationCompleted);
    connect(m_microsoftAuth, &MicrosoftAuth::authenticationError,
            this, &AuthManager::onAuthenticationError);
}

void AuthManager::initialize()
{
    qCInfo(authManager) << "Initializing AuthManager";
    loadSavedAuth();
}

void AuthManager::login()
{
    qCInfo(authManager) << "Starting login process";
    setLoading(true);
    m_microsoftAuth->startAuthentication();
}

void AuthManager::logout()
{
    qCInfo(authManager) << "Logging out user:" << m_username;
    clearAuth();
    setLoggedIn(false);
    setUserInfo(QString(), QString());
}

void AuthManager::refreshToken()
{
    if (m_refreshToken.isEmpty()) {
        qCWarning(authManager) << "No refresh token available";
        emit loginError("No refresh token available");
        return;
    }
    
    qCInfo(authManager) << "Refreshing access token";
    setLoading(true);
    
    // TODO: Implement token refresh logic
    // For now, just re-authenticate
    login();
}

void AuthManager::onAuthenticationCompleted(const QJsonObject& authData)
{
    qCInfo(authManager) << "Authentication completed successfully";
    
    m_accessToken = authData["access_token"].toString();
    m_refreshToken = authData["refresh_token"].toString();
    m_username = authData["username"].toString();
    m_uuid = authData["uuid"].toString();
    
    setUserInfo(m_username, m_uuid);
    setLoggedIn(true);
    setLoading(false);
    
    saveAuth();
    emit loginSuccess();
}

void AuthManager::onAuthenticationError(const QString& error)
{
    qCWarning(authManager) << "Authentication error:" << error;
    setLoading(false);
    emit loginError(error);
}

void AuthManager::setLoggedIn(bool loggedIn)
{
    if (m_isLoggedIn != loggedIn) {
        m_isLoggedIn = loggedIn;
        emit loginStatusChanged();
    }
}

void AuthManager::setUserInfo(const QString& username, const QString& uuid)
{
    bool changed = false;
    if (m_username != username) {
        m_username = username;
        changed = true;
    }
    if (m_uuid != uuid) {
        m_uuid = uuid;
        changed = true;
    }
    if (changed) {
        emit userInfoChanged();
    }
}

void AuthManager::setLoading(bool loading)
{
    if (m_isLoading != loading) {
        m_isLoading = loading;
        emit loadingStatusChanged();
    }
}

void AuthManager::loadSavedAuth()
{
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString authFilePath = QDir(appDataPath).filePath("auth.json");
    
    QFile authFile(authFilePath);
    if (!authFile.exists()) {
        qCInfo(authManager) << "No saved authentication found";
        return;
    }
    
    if (!authFile.open(QIODevice::ReadOnly)) {
        qCWarning(authManager) << "Failed to open auth file:" << authFile.errorString();
        return;
    }
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(authFile.readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qCWarning(authManager) << "Failed to parse auth file:" << error.errorString();
        return;
    }
    
    QJsonObject authData = doc.object();
    m_accessToken = authData["access_token"].toString();
    m_refreshToken = authData["refresh_token"].toString();
    m_username = authData["username"].toString();
    m_uuid = authData["uuid"].toString();
    
    if (!m_accessToken.isEmpty() && !m_username.isEmpty()) {
        setUserInfo(m_username, m_uuid);
        setLoggedIn(true);
        qCInfo(authManager) << "Loaded saved authentication for user:" << m_username;
    }
}

void AuthManager::saveAuth()
{
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(appDataPath);
    QString authFilePath = QDir(appDataPath).filePath("auth.json");
    
    QJsonObject authData;
    authData["access_token"] = m_accessToken;
    authData["refresh_token"] = m_refreshToken;
    authData["username"] = m_username;
    authData["uuid"] = m_uuid;
    
    QFile authFile(authFilePath);
    if (!authFile.open(QIODevice::WriteOnly)) {
        qCWarning(authManager) << "Failed to save auth file:" << authFile.errorString();
        return;
    }
    
    QJsonDocument doc(authData);
    authFile.write(doc.toJson());
    qCInfo(authManager) << "Saved authentication data";
}

void AuthManager::clearAuth()
{
    m_accessToken.clear();
    m_refreshToken.clear();
    
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString authFilePath = QDir(appDataPath).filePath("auth.json");
    QFile::remove(authFilePath);
    
    qCInfo(authManager) << "Cleared authentication data";
}