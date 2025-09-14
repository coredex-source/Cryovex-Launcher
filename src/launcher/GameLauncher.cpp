#include "GameLauncher.h"
#include "JvmArgumentBuilder.h"
#include <QLoggingCategory>
#include <QTimer>

Q_LOGGING_CATEGORY(gameLauncher, "cryovex.launcher.game")

GameLauncher::GameLauncher(QObject *parent)
    : QObject(parent)
    , m_gameProcess(new QProcess(this))
    , m_argumentBuilder(new JvmArgumentBuilder(this))
{
    connect(m_gameProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &GameLauncher::onGameFinished);
    connect(m_gameProcess, &QProcess::errorOccurred,
            this, &GameLauncher::onGameError);
    connect(m_gameProcess, &QProcess::readyReadStandardOutput,
            this, &GameLauncher::onGameReadyReadStandardOutput);
    connect(m_gameProcess, &QProcess::readyReadStandardError,
            this, &GameLauncher::onGameReadyReadStandardError);
}

GameLauncher::~GameLauncher()
{
    if (m_gameProcess && m_gameProcess->state() != QProcess::NotRunning) {
        m_gameProcess->kill();
        m_gameProcess->waitForFinished(3000);
    }
}

void GameLauncher::launchGame(const QString& versionId, const QJsonObject& versionManifest,
                             const QString& username, const QString& uuid, 
                             const QString& accessToken)
{
    qCInfo(gameLauncher) << "Launching game version:" << versionId << "for user:" << username << "(stub)";
    
    Q_UNUSED(versionManifest)
    Q_UNUSED(uuid)
    Q_UNUSED(accessToken)
    
    if (!validateLaunchParameters(versionId, versionManifest, username, uuid, accessToken)) {
        emit launchError("Invalid launch parameters");
        return;
    }
    
    // Stub implementation - just simulate a successful launch
    setRunning(true);
    setCurrentVersion(versionId);
    emit gameStarted();
    
    // Simulate the game running for a short time
    QTimer::singleShot(5000, this, [this]() {
        setRunning(false);
        setCurrentVersion(QString());
        emit gameFinished(0);
    });
}

void GameLauncher::killGame()
{
    qCInfo(gameLauncher) << "Killing game process";
    
    if (m_gameProcess && m_gameProcess->state() != QProcess::NotRunning) {
        m_gameProcess->kill();
    }
}

void GameLauncher::onGameFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qCInfo(gameLauncher) << "Game finished with exit code:" << exitCode;
    Q_UNUSED(exitStatus)
    
    setRunning(false);
    setCurrentVersion(QString());
    emit gameFinished(exitCode);
}

void GameLauncher::onGameError(QProcess::ProcessError error)
{
    qCWarning(gameLauncher) << "Game process error:" << error;
    
    setRunning(false);
    setCurrentVersion(QString());
    emit gameError("Process error: " + QString::number(error));
}

void GameLauncher::onGameReadyReadStandardOutput()
{
    QByteArray data = m_gameProcess->readAllStandardOutput();
    emit gameOutput(QString::fromUtf8(data));
}

void GameLauncher::onGameReadyReadStandardError()
{
    QByteArray data = m_gameProcess->readAllStandardError();
    emit gameError(QString::fromUtf8(data));
}

void GameLauncher::setRunning(bool running)
{
    if (m_isRunning != running) {
        m_isRunning = running;
        emit runningStatusChanged();
    }
}

void GameLauncher::setCurrentVersion(const QString& version)
{
    if (m_currentVersion != version) {
        m_currentVersion = version;
        emit currentVersionChanged();
    }
}

bool GameLauncher::validateLaunchParameters(const QString& versionId, const QJsonObject& versionManifest,
                                           const QString& username, const QString& uuid, 
                                           const QString& accessToken)
{
    Q_UNUSED(versionManifest)
    
    if (versionId.isEmpty()) {
        qCWarning(gameLauncher) << "Version ID is empty";
        return false;
    }
    
    if (username.isEmpty()) {
        qCWarning(gameLauncher) << "Username is empty";
        return false;
    }
    
    if (uuid.isEmpty()) {
        qCWarning(gameLauncher) << "UUID is empty";
        return false;
    }
    
    if (accessToken.isEmpty()) {
        qCWarning(gameLauncher) << "Access token is empty";
        return false;
    }
    
    return true;
}