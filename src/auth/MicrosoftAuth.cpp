#include "MicrosoftAuth.h"
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QHttpMultiPart>
#include <QLoggingCategory>
#include <QDesktopServices>
#include <QApplication>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QWebEngineSettings>
#include <QWebEngineProfile>

Q_LOGGING_CATEGORY(microsoftAuth, "cryovex.auth.microsoft")

MicrosoftAuth::MicrosoftAuth(QObject *parent)
    : QObject(parent)
    , m_webView(nullptr)
    , m_networkManager(new QNetworkAccessManager(this))
{
    // Debug: Verify environment variables are loaded (without exposing values)
    QString envClientId = qEnvironmentVariable("CRYOVEX_CLIENT_ID");
    QString envRedirectUri = qEnvironmentVariable("CRYOVEX_REDIRECT_URI");
    
    qCInfo(microsoftAuth) << "Client ID loaded:" << (getClientId() != "YOUR_CLIENT_ID_HERE" ? "✓ From environment" : "✗ Using default");
    qCInfo(microsoftAuth) << "Redirect URI:" << getRedirectUri();
}

MicrosoftAuth::~MicrosoftAuth()
{
    qCInfo(microsoftAuth) << "Cleaning up MicrosoftAuth";
    
    // Clean up WebView safely
    if (m_webView) {
        m_webView->hide();
        m_webView->deleteLater();
        m_webView = nullptr;
    }
}

void MicrosoftAuth::startAuthentication()
{
    qCInfo(microsoftAuth) << "Starting Microsoft authentication";
    
    // Initialize WebView safely when needed
    if (!m_webView) {
        initializeWebView();
    }
    
    // Check if WebView initialization was successful
    if (!m_webView) {
        qCCritical(microsoftAuth) << "Failed to initialize WebEngine - cannot proceed with authentication";
        emit authenticationError("Failed to initialize web browser component");
        return;
    }
    
    // Generate PKCE parameters
    m_codeVerifier = generateCodeVerifier();
    QString codeChallenge = generateCodeChallenge(m_codeVerifier);
    
    // Construct Microsoft OAuth2 authorization URL with PKCE
    QUrl authUrl("https://login.microsoftonline.com/consumers/oauth2/v2.0/authorize");
    QUrlQuery query;
    
    query.addQueryItem("client_id", getClientId());
    query.addQueryItem("response_type", "code");
    query.addQueryItem("redirect_uri", getRedirectUri());
    query.addQueryItem("scope", "XboxLive.signin offline_access");
    query.addQueryItem("state", "12345"); // Should be random in production
    query.addQueryItem("code_challenge", codeChallenge);
    query.addQueryItem("code_challenge_method", "S256");
    
    authUrl.setQuery(query);
    
    qCInfo(microsoftAuth) << "Opening auth URL:" << authUrl.toString();
    
    // Load the authentication page
    m_webView->load(authUrl);
    m_webView->show();
    m_webView->raise();
    m_webView->activateWindow();
}

void MicrosoftAuth::onUrlChanged(const QUrl& url)
{
    qCDebug(microsoftAuth) << "URL changed to:" << url.toString();
    
    // Check if this is the redirect callback
    if (url.toString().startsWith(getRedirectUri())) {
        qCInfo(microsoftAuth) << "Received redirect callback";
        
        // Hide the web view safely
        if (m_webView) {
            m_webView->hide();
        }
        
        // Extract the authorization code
        QUrlQuery query(url);
        QString authCode = query.queryItemValue("code");
        QString error = query.queryItemValue("error");
        
        if (!error.isEmpty()) {
            QString errorDescription = query.queryItemValue("error_description");
            qCWarning(microsoftAuth) << "OAuth error:" << error << errorDescription;
            emit authenticationError("Authentication failed: " + errorDescription);
            return;
        }
        
        if (authCode.isEmpty()) {
            qCWarning(microsoftAuth) << "No authorization code received";
            emit authenticationError("No authorization code received");
            return;
        }
        
        qCInfo(microsoftAuth) << "Received authorization code, exchanging for tokens";
        exchangeCodeForTokens(authCode);
    }
}

void MicrosoftAuth::initializeWebView()
{
    qCInfo(microsoftAuth) << "Initializing WebEngine view safely";
    
    try {
        m_webView = new QWebEngineView();
        
        // Configure web view for better performance
        m_webView->setWindowTitle("Microsoft Account Login");
        m_webView->resize(600, 700);
        
        // Basic settings only - avoid complex configurations that might cause heap issues
        auto* settings = m_webView->settings();
        if (settings) {
            settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
            settings->setAttribute(QWebEngineSettings::LocalStorageEnabled, false);
            settings->setAttribute(QWebEngineSettings::AutoLoadImages, true);
            settings->setAttribute(QWebEngineSettings::PluginsEnabled, false);
        }
        
        // Basic profile settings only
        auto* page = m_webView->page();
        if (page) {
            auto* profile = page->profile();
            if (profile) {
                profile->setPersistentCookiesPolicy(QWebEngineProfile::NoPersistentCookies);
                profile->setHttpCacheType(QWebEngineProfile::MemoryHttpCache);
            }
        }
        
        // Connect signals
        connect(m_webView, &QWebEngineView::urlChanged,
                this, &MicrosoftAuth::onUrlChanged);
        
        qCInfo(microsoftAuth) << "WebEngine view initialized successfully";
        
    } catch (const std::exception& e) {
        qCWarning(microsoftAuth) << "Failed to initialize WebEngine view:" << e.what();
        m_webView = nullptr;
    } catch (...) {
        qCWarning(microsoftAuth) << "Failed to initialize WebEngine view: unknown error";
        m_webView = nullptr;
    }
}

void MicrosoftAuth::exchangeCodeForTokens(const QString& authCode)
{
    qCInfo(microsoftAuth) << "Exchanging authorization code for access token";
    
    QUrl tokenUrl("https://login.microsoftonline.com/consumers/oauth2/v2.0/token");
    QNetworkRequest request(tokenUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    
    QUrlQuery postData;
    postData.addQueryItem("client_id", getClientId());
    postData.addQueryItem("code", authCode);
    postData.addQueryItem("grant_type", "authorization_code");
    postData.addQueryItem("redirect_uri", getRedirectUri());
    postData.addQueryItem("scope", "XboxLive.signin offline_access");
    postData.addQueryItem("code_verifier", m_codeVerifier); // PKCE verifier
    
    // Add explicit public client parameter
    postData.addQueryItem("client_assertion_type", "");
    postData.addQueryItem("client_assertion", "");
    
    qCInfo(microsoftAuth) << "Token request data:" << postData.toString();
    
    QNetworkReply* reply = m_networkManager->post(request, postData.toString().toUtf8());
    connect(reply, &QNetworkReply::finished, this, &MicrosoftAuth::onTokenExchangeFinished);
}

void MicrosoftAuth::onTokenExchangeFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();
    
    if (reply->error() != QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        qCWarning(microsoftAuth) << "Token exchange failed:" << reply->errorString();
        qCWarning(microsoftAuth) << "HTTP Status:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qCWarning(microsoftAuth) << "Response body:" << responseData;
        emit authenticationError("Failed to exchange authorization code: " + reply->errorString());
        return;
    }
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qCWarning(microsoftAuth) << "Failed to parse token response:" << error.errorString();
        emit authenticationError("Invalid token response");
        return;
    }
    
    QJsonObject tokenData = doc.object();
    m_accessToken = tokenData["access_token"].toString();
    m_refreshToken = tokenData["refresh_token"].toString();
    
    if (m_accessToken.isEmpty()) {
        qCWarning(microsoftAuth) << "No access token in response";
        emit authenticationError("No access token received");
        return;
    }
    
    qCInfo(microsoftAuth) << "Successfully obtained Microsoft access token";
    authenticateWithXboxLive(m_accessToken);
}

void MicrosoftAuth::authenticateWithXboxLive(const QString& accessToken)
{
    qCInfo(microsoftAuth) << "Authenticating with Xbox Live";
    
    QUrl xblUrl("https://user.auth.xboxlive.com/user/authenticate");
    QNetworkRequest request(xblUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "application/json");
    
    QJsonObject xblData;
    QJsonObject properties;
    properties["AuthMethod"] = "RPS";
    properties["SiteName"] = "user.auth.xboxlive.com";
    properties["RpsTicket"] = "d=" + accessToken;
    
    xblData["Properties"] = properties;
    xblData["RelyingParty"] = "http://auth.xboxlive.com";
    xblData["TokenType"] = "JWT";
    
    QJsonDocument doc(xblData);
    QNetworkReply* reply = m_networkManager->post(request, doc.toJson());
    connect(reply, &QNetworkReply::finished, this, &MicrosoftAuth::onXboxLiveAuthFinished);
}

void MicrosoftAuth::onXboxLiveAuthFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();
    
    if (reply->error() != QNetworkReply::NoError) {
        qCWarning(microsoftAuth) << "Xbox Live auth failed:" << reply->errorString();
        emit authenticationError("Xbox Live authentication failed: " + reply->errorString());
        return;
    }
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qCWarning(microsoftAuth) << "Failed to parse Xbox Live response:" << error.errorString();
        emit authenticationError("Invalid Xbox Live response");
        return;
    }
    
    QJsonObject xblData = doc.object();
    m_xblToken = xblData["Token"].toString();
    
    if (m_xblToken.isEmpty()) {
        qCWarning(microsoftAuth) << "No Xbox Live token received";
        emit authenticationError("No Xbox Live token received");
        return;
    }
    
    qCInfo(microsoftAuth) << "Successfully obtained Xbox Live token";
    authenticateWithXsts(m_xblToken);
}

void MicrosoftAuth::authenticateWithXsts(const QString& xblToken)
{
    qCInfo(microsoftAuth) << "Authenticating with XSTS";
    
    QUrl xstsUrl("https://xsts.auth.xboxlive.com/xsts/authorize");
    QNetworkRequest request(xstsUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "application/json");
    
    QJsonObject xstsData;
    QJsonObject properties;
    properties["SandboxId"] = "RETAIL";
    QJsonArray userTokens;
    userTokens.append(xblToken);
    properties["UserTokens"] = userTokens;
    
    xstsData["Properties"] = properties;
    xstsData["RelyingParty"] = "rp://api.minecraftservices.com/";
    xstsData["TokenType"] = "JWT";
    
    QJsonDocument doc(xstsData);
    QNetworkReply* reply = m_networkManager->post(request, doc.toJson());
    connect(reply, &QNetworkReply::finished, this, &MicrosoftAuth::onXstsAuthFinished);
}

void MicrosoftAuth::onXstsAuthFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();
    
    if (reply->error() != QNetworkReply::NoError) {
        qCWarning(microsoftAuth) << "XSTS auth failed:" << reply->errorString();
        emit authenticationError("XSTS authentication failed: " + reply->errorString());
        return;
    }
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qCWarning(microsoftAuth) << "Failed to parse XSTS response:" << error.errorString();
        emit authenticationError("Invalid XSTS response");
        return;
    }
    
    QJsonObject xstsData = doc.object();
    m_xstsToken = xstsData["Token"].toString();
    
    // Extract user hash from DisplayClaims
    QJsonObject displayClaims = xstsData["DisplayClaims"].toObject();
    QJsonArray xui = displayClaims["xui"].toArray();
    if (!xui.isEmpty()) {
        m_userHash = xui[0].toObject()["uhs"].toString();
    }
    
    if (m_xstsToken.isEmpty() || m_userHash.isEmpty()) {
        qCWarning(microsoftAuth) << "Missing XSTS token or user hash";
        emit authenticationError("Incomplete XSTS response");
        return;
    }
    
    qCInfo(microsoftAuth) << "Successfully obtained XSTS token";
    authenticateWithMinecraft(m_xstsToken, m_userHash);
}

void MicrosoftAuth::authenticateWithMinecraft(const QString& xstsToken, const QString& userHash)
{
    qCInfo(microsoftAuth) << "Authenticating with Minecraft";
    
    QUrl mcUrl("https://api.minecraftservices.com/authentication/login_with_xbox");
    QNetworkRequest request(mcUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "application/json");
    
    QJsonObject mcData;
    QString identityToken = "XBL3.0 x=" + userHash + ";" + xstsToken;
    mcData["identityToken"] = identityToken;
    
    qCInfo(microsoftAuth) << "Minecraft request body:" << QJsonDocument(mcData).toJson(QJsonDocument::Compact);
    
    QJsonDocument doc(mcData);
    QNetworkReply* reply = m_networkManager->post(request, doc.toJson());
    connect(reply, &QNetworkReply::finished, this, &MicrosoftAuth::onMinecraftAuthFinished);
}

void MicrosoftAuth::onMinecraftAuthFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();
    
    if (reply->error() != QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        qCWarning(microsoftAuth) << "Minecraft auth failed:" << reply->errorString();
        qCWarning(microsoftAuth) << "HTTP Status:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qCWarning(microsoftAuth) << "Response body:" << responseData;
        emit authenticationError("Minecraft authentication failed: " + reply->errorString());
        return;
    }
    
    QByteArray responseData = reply->readAll();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(responseData, &error);
    if (error.error != QJsonParseError::NoError) {
        qCWarning(microsoftAuth) << "Failed to parse Minecraft response:" << error.errorString();
        qCWarning(microsoftAuth) << "Raw response:" << responseData;
        emit authenticationError("Invalid Minecraft response");
        return;
    }
    
    QJsonObject mcData = doc.object();
    m_mcAccessToken = mcData["access_token"].toString();
    
    if (m_mcAccessToken.isEmpty()) {
        qCWarning(microsoftAuth) << "No Minecraft access token received";
        qCWarning(microsoftAuth) << "Full response:" << mcData;
        emit authenticationError("No Minecraft access token received");
        return;
    }
    
    qCInfo(microsoftAuth) << "Successfully obtained Minecraft access token";
    fetchMinecraftProfile(m_mcAccessToken);
}

void MicrosoftAuth::fetchMinecraftProfile(const QString& mcAccessToken)
{
    qCInfo(microsoftAuth) << "Fetching Minecraft profile";
    
    QUrl profileUrl("https://api.minecraftservices.com/minecraft/profile");
    QNetworkRequest request(profileUrl);
    request.setRawHeader("Authorization", ("Bearer " + mcAccessToken).toUtf8());
    
    QNetworkReply* reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &MicrosoftAuth::onProfileFetchFinished);
}

void MicrosoftAuth::onProfileFetchFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();
    
    if (reply->error() != QNetworkReply::NoError) {
        qCWarning(microsoftAuth) << "Profile fetch failed:" << reply->errorString();
        emit authenticationError("Failed to fetch Minecraft profile: " + reply->errorString());
        return;
    }
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        qCWarning(microsoftAuth) << "Failed to parse profile response:" << error.errorString();
        emit authenticationError("Invalid profile response");
        return;
    }
    
    QJsonObject profileData = doc.object();
    QString username = profileData["name"].toString();
    QString uuid = profileData["id"].toString();
    
    if (username.isEmpty() || uuid.isEmpty()) {
        qCWarning(microsoftAuth) << "Incomplete profile data";
        emit authenticationError("Incomplete profile data");
        return;
    }
    
    qCInfo(microsoftAuth) << "Successfully fetched profile for user:" << username;
    
    // Create the final authentication data
    QJsonObject authData;
    authData["access_token"] = m_mcAccessToken;
    authData["refresh_token"] = m_refreshToken;
    authData["username"] = username;
    authData["uuid"] = uuid;
    
    emit authenticationCompleted(authData);
}

QString MicrosoftAuth::generateCodeVerifier()
{
    // Generate a random 128-character string for PKCE
    const QString chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
    QString verifier;
    
    for (int i = 0; i < 128; ++i) {
        int index = QRandomGenerator::global()->bounded(chars.length());
        verifier.append(chars[index]);
    }
    
    return verifier;
}

QString MicrosoftAuth::generateCodeChallenge(const QString& verifier)
{
    // Create SHA256 hash of the verifier and encode as base64url
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(verifier.toUtf8());
    QByteArray hashResult = hash.result();
    
    // Convert to base64url (replace + with -, / with _, remove padding =)
    QString base64 = hashResult.toBase64();
    base64.replace('+', '-');
    base64.replace('/', '_');
    base64.remove('=');
    
    return base64;
}

QString MicrosoftAuth::getClientId() const
{
    return qEnvironmentVariable("CRYOVEX_CLIENT_ID", "YOUR_CLIENT_ID_HERE");
}

QString MicrosoftAuth::getRedirectUri() const
{
    return qEnvironmentVariable("CRYOVEX_REDIRECT_URI", "http://localhost:8080/auth/callback");
}