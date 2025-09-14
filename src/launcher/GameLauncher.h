#pragma once

#include <QObject>
#include <QProcess>
#include <QJsonObject>
#include <QStringList>

class JvmArgumentBuilder;

class GameLauncher : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isRunning READ isRunning NOTIFY runningStatusChanged)
    Q_PROPERTY(QString currentVersion READ currentVersion NOTIFY currentVersionChanged)

public:
    explicit GameLauncher(QObject *parent = nullptr);
    ~GameLauncher();
    
    bool isRunning() const { return m_isRunning; }
    QString currentVersion() const { return m_currentVersion; }
    
    Q_INVOKABLE void launchGame(const QString& versionId, const QJsonObject& versionManifest,
                               const QString& username, const QString& uuid, 
                               const QString& accessToken);
    Q_INVOKABLE void killGame();

signals:
    void runningStatusChanged();
    void currentVersionChanged();
    void gameStarted();
    void gameFinished(int exitCode);
    void gameOutput(const QString& output);
    void gameError(const QString& error);
    void launchError(const QString& error);

private slots:
    void onGameFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onGameError(QProcess::ProcessError error);
    void onGameReadyReadStandardOutput();
    void onGameReadyReadStandardError();

private:
    void setRunning(bool running);
    void setCurrentVersion(const QString& version);
    bool validateLaunchParameters(const QString& versionId, const QJsonObject& versionManifest,
                                 const QString& username, const QString& uuid, 
                                 const QString& accessToken);
    
    QProcess* m_gameProcess;
    JvmArgumentBuilder* m_argumentBuilder;
    bool m_isRunning = false;
    QString m_currentVersion;
};