import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import CryovexLauncher 1.0

Item {
    ColumnLayout {
        anchors.centerIn: parent
        spacing: 30
        width: Math.min(400, parent.width - 40)
        
        // Welcome message
        Column {
            Layout.alignment: Qt.AlignHCenter
            spacing: 10
            
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Welcome to Cryovex Launcher"
                font.pixelSize: 24
                font.bold: true
                color: Material.foreground
            }
            
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Sign in with your Microsoft account to get started"
                font.pixelSize: 14
                color: Material.hintTextColor
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
            }
        }
        
        // Login illustration
        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 200
            Layout.preferredHeight: 200
            color: "transparent"
            border.color: Material.accent
            border.width: 2
            radius: 100
            
            Column {
                anchors.centerIn: parent
                spacing: 10
                
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 60
                    height: 60
                    color: Material.accent
                    radius: 30
                    
                    Text {
                        anchors.centerIn: parent
                        text: "👤"
                        font.pixelSize: 30
                    }
                }
                
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "Microsoft Account"
                    font.pixelSize: 12
                    color: Material.hintTextColor
                }
            }
        }
        
        // Login button
        Button {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 250
            Layout.preferredHeight: 50
            
            text: "Sign in with Microsoft"
            font.pixelSize: 16
            font.bold: true
            
            Material.background: Material.accent
            Material.foreground: "white"
            
            onClicked: AuthManager.login()
            enabled: !AuthManager.isLoading
        }
        
        // Status message
        Text {
            Layout.alignment: Qt.AlignHCenter
            Layout.maximumWidth: parent.width
            
            text: AuthManager.isLoading ? "Opening authentication window..." : ""
            font.pixelSize: 12
            color: Material.hintTextColor
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            visible: text.length > 0
        }
    }
}