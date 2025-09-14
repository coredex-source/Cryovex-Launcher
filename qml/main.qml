import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import CryovexLauncher 1.0

ApplicationWindow {
    id: mainWindow
    
    width: 1200
    height: 800
    minimumWidth: 900
    minimumHeight: 600
    visible: true
    
    title: "Cryovex Launcher"
    
    Material.theme: Material.Dark
    Material.accent: Material.Blue
    
    // Enable translucent background for Windows 11 Mica effect
    color: "transparent"
    
    // Background with blur effect
    Rectangle {
        anchors.fill: parent
        color: Material.backgroundColor
        opacity: 0.95
        
        // Gradient overlay for modern look
        Rectangle {
            anchors.fill: parent
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#20000000" }
                GradientStop { position: 1.0; color: "#40000000" }
            }
        }
    }
    
    // Main layout
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20
        
        // Header
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 80
            color: "transparent"
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 20
                
                // Logo area
                Rectangle {
                    Layout.preferredWidth: 60
                    Layout.preferredHeight: 60
                    color: Material.accent
                    radius: 10
                    
                    Text {
                        anchors.centerIn: parent
                        text: "C"
                        font.pixelSize: 32
                        font.bold: true
                        color: "white"
                    }
                }
                
                // Title
                Column {
                    Layout.leftMargin: 15
                    
                    Text {
                        text: "Cryovex Launcher"
                        font.pixelSize: 28
                        font.bold: true
                        color: Material.foreground
                    }
                    
                    Text {
                        text: AuthManager.isLoggedIn ? "Welcome, " + AuthManager.username : "Please log in to continue"
                        font.pixelSize: 14
                        color: Material.hintTextColor
                    }
                }
                
                Item { Layout.fillWidth: true }
                
                // User info and controls
                RowLayout {
                    spacing: 15
                    
                    Button {
                        text: AuthManager.isLoggedIn ? "Logout" : "Login"
                        Material.background: AuthManager.isLoggedIn ? Material.Red : Material.accent
                        onClicked: {
                            if (AuthManager.isLoggedIn) {
                                AuthManager.logout()
                            } else {
                                AuthManager.login()
                            }
                        }
                        enabled: !AuthManager.isLoading
                    }
                    
                    Button {
                        text: "Settings"
                        Material.background: Material.backgroundDimColor
                        onClicked: stackView.push(settingsComponent)
                    }
                }
            }
        }
        
        // Main content area
        StackView {
            id: stackView
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            initialItem: AuthManager.isLoggedIn ? mainViewComponent : loginViewComponent
            
            Connections {
                target: AuthManager
                function onLoginStatusChanged() {
                    if (AuthManager.isLoggedIn) {
                        stackView.replace(mainViewComponent)
                    } else {
                        stackView.replace(loginViewComponent)
                    }
                }
            }
        }
    }
    
    // Components
    Component {
        id: loginViewComponent
        LoginView {}
    }
    
    Component {
        id: mainViewComponent
        MainView {}
    }
    
    Component {
        id: settingsComponent
        SettingsView {}
    }
    
    // Loading overlay
    Rectangle {
        anchors.fill: parent
        color: "#80000000"
        visible: AuthManager.isLoading
        
        BusyIndicator {
            anchors.centerIn: parent
            running: AuthManager.isLoading
            Material.accent: Material.Blue
        }
    }
}