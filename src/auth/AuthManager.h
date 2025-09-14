#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QWebEngineView>
#include <QTimer>

class MicrosoftAuth;

class AuthManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isLoggedIn READ isLoggedIn NOTIFY loginStatusChanged)
    Q_PROPERTY(QString username READ username NOTIFY userInfoChanged)
    Q_PROPERTY(QString uuid READ uuid NOTIFY userInfoChanged)
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY loadingStatusChanged)

public:
    static AuthManager& instance();
    
    void initialize();
    
    bool isLoggedIn() const { return m_isLoggedIn; }
    QString username() const { return m_username; }
    QString uuid() const { return m_uuid; }
    QString accessToken() const { return m_accessToken; }
    bool isLoading() const { return m_isLoading; }

public slots:
    void login();
    void logout();
    void refreshToken();

signals:
    void loginStatusChanged();
    void userInfoChanged();
    void loadingStatusChanged();
    void loginSuccess();
    void loginError(const QString& error);

private slots:
    void onAuthenticationCompleted(const QJsonObject& authData);
    void onAuthenticationError(const QString& error);

private:
    explicit AuthManager(QObject *parent = nullptr);
    ~AuthManager() = default;
    
    void setLoggedIn(bool loggedIn);
    void setUserInfo(const QString& username, const QString& uuid);
    void setLoading(bool loading);
    
    void loadSavedAuth();
    void saveAuth();
    void clearAuth();

    bool m_isLoggedIn = false;
    QString m_username;
    QString m_uuid;
    QString m_accessToken;
    QString m_refreshToken;
    bool m_isLoading = false;
    
    MicrosoftAuth* m_microsoftAuth;
};