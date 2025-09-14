#pragma once

#include <QObject>
#include <QWebEngineView>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QUrl>
#include <QUrlQuery>
#include <QTimer>

class MicrosoftAuth : public QObject
{
    Q_OBJECT

public:
    explicit MicrosoftAuth(QObject *parent = nullptr);
    ~MicrosoftAuth();
    
    void startAuthentication();

signals:
    void authenticationCompleted(const QJsonObject& authData);
    void authenticationError(const QString& error);

private slots:
    void onUrlChanged(const QUrl& url);
    void onTokenExchangeFinished();
    void onXboxLiveAuthFinished();
    void onXstsAuthFinished();
    void onMinecraftAuthFinished();
    void onProfileFetchFinished();

private:
    void exchangeCodeForTokens(const QString& authCode);
    void authenticateWithXboxLive(const QString& accessToken);
    void authenticateWithXsts(const QString& xblToken);
    void authenticateWithMinecraft(const QString& xstsToken, const QString& userHash);
    void fetchMinecraftProfile(const QString& mcAccessToken);
    void initializeWebView(); // Safe WebEngine initialization
    
    // PKCE helper methods
    QString generateCodeVerifier();
    QString generateCodeChallenge(const QString& verifier);
    
    QWebEngineView* m_webView;
    QNetworkAccessManager* m_networkManager;
    
    QString m_accessToken;
    QString m_refreshToken;
    QString m_xblToken;
    QString m_xstsToken;
    QString m_mcAccessToken;
    QString m_userHash;
    QString m_codeVerifier; // For PKCE
    
    // Remove static constants, use methods instead
    QString getClientId() const;
    QString getRedirectUri() const;
    static const QString REDIRECT_URI;
};