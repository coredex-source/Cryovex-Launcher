QT += core widgets quick quickcontrols2 network webenginewidgets

CONFIG += c++17

TARGET = CryovexLauncher
TEMPLATE = app

# Version info
VERSION = 1.0.0
QMAKE_TARGET_DESCRIPTION = "Cryovex Minecraft Launcher"
QMAKE_TARGET_COPYRIGHT = "Copyright (c) 2025 Cryovex"

# Directories
DESTDIR = $$PWD/build
OBJECTS_DIR = $$PWD/build/obj
MOC_DIR = $$PWD/build/moc
RCC_DIR = $$PWD/build/rcc
UI_DIR = $$PWD/build/ui

# Include paths
INCLUDEPATH += src/
INCLUDEPATH += src/auth/
INCLUDEPATH += src/version/
INCLUDEPATH += src/download/
INCLUDEPATH += src/launcher/
INCLUDEPATH += src/config/
INCLUDEPATH += src/utils/

# Source files
SOURCES += \
    src/main.cpp \
    src/auth/AuthManager.cpp \
    src/auth/MicrosoftAuth.cpp \
    src/version/VersionManager.cpp \
    src/version/MinecraftVersion.cpp \
    src/download/DownloadManager.cpp \
    src/download/DownloadTask.cpp \
    src/launcher/GameLauncher.cpp \
    src/launcher/JvmArgumentBuilder.cpp \
    src/config/ConfigManager.cpp \
    src/config/Profile.cpp \
    src/utils/Logger.cpp \
    src/utils/FileUtils.cpp \
    src/utils/NetworkUtils.cpp

# Header files
HEADERS += \
    src/auth/AuthManager.h \
    src/auth/MicrosoftAuth.h \
    src/version/VersionManager.h \
    src/version/MinecraftVersion.h \
    src/download/DownloadManager.h \
    src/download/DownloadTask.h \
    src/launcher/GameLauncher.h \
    src/launcher/JvmArgumentBuilder.h \
    src/config/ConfigManager.h \
    src/config/Profile.h \
    src/utils/Logger.h \
    src/utils/FileUtils.h \
    src/utils/NetworkUtils.h

# QML files
RESOURCES += qml.qrc

# Platform-specific settings
win32 {
    RC_ICONS = resources/icon.ico
    QMAKE_TARGET_PRODUCT = "Cryovex Launcher"
    QMAKE_TARGET_COMPANY = "Cryovex"
}
