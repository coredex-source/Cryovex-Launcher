cmake_minimum_required(VERSION 3.16)

project(CryovexLauncher VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Quick QuickControls2 Network WebEngineWidgets)

# Enable automatic generation for Qt's meta-object system
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Create the executable
add_executable(CryovexLauncher
    src/main.cpp
)

# Link Qt6 libraries
target_link_libraries(CryovexLauncher
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Network
    Qt6::WebEngineWidgets
)

# Set the startup project for Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT CryovexLauncher)

# Add resource file for QML
qt6_add_resources(CryovexLauncher "qml"
    PREFIX "/qml"
    FILES
        qml/main.qml
        qml/LoginView.qml
        qml/MainView.qml
        qml/SettingsView.qml
)

# Add subdirectories for different modules
add_subdirectory(src/auth)
add_subdirectory(src/version)
add_subdirectory(src/download)
add_subdirectory(src/launcher)
add_subdirectory(src/config)
add_subdirectory(src/utils)

# Include directories
target_include_directories(CryovexLauncher PRIVATE
    src/
    src/auth/
    src/version/
    src/download/
    src/launcher/
    src/config/
    src/utils/
)