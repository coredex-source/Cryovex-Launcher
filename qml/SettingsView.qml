import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs
import CryovexLauncher 1.0

Item {
    ColumnLayout {
        anchors.fill: parent
        spacing: 20
        
        // Header
        RowLayout {
            Layout.fillWidth: true
            
            Button {
                text: "← Back"
                Material.background: Material.backgroundDimColor
                onClicked: stackView.pop()
            }
            
            Text {
                text: "Settings"
                font.pixelSize: 24
                font.bold: true
                color: Material.foreground
            }
            
            Item { Layout.fillWidth: true }
        }
        
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            ColumnLayout {
                width: Math.min(600, parent.width)
                spacing: 30
                
                // Game Settings
                GroupBox {
                    Layout.fillWidth: true
                    title: "Game Settings"
                    
                    Material.background: Material.backgroundDimColor
                    
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 15
                        
                        // Game Directory
                        RowLayout {
                            Layout.fillWidth: true
                            
                            Label {
                                Layout.preferredWidth: 120
                                text: "Game Directory:"
                                color: Material.foreground
                            }
                            
                            TextField {
                                id: gameDirectoryField
                                Layout.fillWidth: true
                                text: ConfigManager.gameDirectory
                                placeholderText: "Select Minecraft directory..."
                                readOnly: true
                                
                                Material.background: Material.backgroundColor
                            }
                            
                            Button {
                                text: "Browse"
                                Material.background: Material.accent
                                onClicked: gameDirectoryDialog.open()
                            }
                        }
                        
                        // Java Path
                        RowLayout {
                            Layout.fillWidth: true
                            
                            Label {
                                Layout.preferredWidth: 120
                                text: "Java Path:"
                                color: Material.foreground
                            }
                            
                            TextField {
                                id: javaPathField
                                Layout.fillWidth: true
                                text: ConfigManager.javaPath
                                placeholderText: "java (auto-detect)"
                                
                                Material.background: Material.backgroundColor
                                
                                onTextChanged: {
                                    if (text !== ConfigManager.javaPath) {
                                        ConfigManager.javaPath = text
                                    }
                                }
                            }
                            
                            Button {
                                text: "Browse"
                                Material.background: Material.accent
                                onClicked: javaPathDialog.open()
                            }
                        }
                        
                        // Memory Settings
                        RowLayout {
                            Layout.fillWidth: true
                            
                            Label {
                                Layout.preferredWidth: 120
                                text: "Memory (MB):"
                                color: Material.foreground
                            }
                            
                            SpinBox {
                                id: memorySpinBox
                                from: 512
                                to: 16384
                                stepSize: 256
                                value: ConfigManager.memoryMB
                                
                                Material.background: Material.backgroundColor
                                
                                onValueChanged: {
                                    if (value !== ConfigManager.memoryMB) {
                                        ConfigManager.memoryMB = value
                                    }
                                }
                            }
                            
                            Item { Layout.fillWidth: true }
                            
                            Label {
                                text: "(" + (memorySpinBox.value / 1024).toFixed(1) + " GB)"
                                color: Material.hintTextColor
                            }
                        }
                    }
                }
                
                // Profiles
                GroupBox {
                    Layout.fillWidth: true
                    title: "Profiles"
                    
                    Material.background: Material.backgroundDimColor
                    
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 15
                        
                        ListView {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 200
                            
                            model: ConfigManager
                            
                            delegate: ItemDelegate {
                                width: parent.width
                                height: 50
                                
                                Rectangle {
                                    anchors.fill: parent
                                    anchors.margins: 2
                                    color: parent.hovered ? Material.accent : "transparent"
                                    radius: 5
                                    opacity: parent.hovered ? 0.3 : 1.0
                                    
                                    RowLayout {
                                        anchors.fill: parent
                                        anchors.margins: 10
                                        
                                        Rectangle {
                                            Layout.preferredWidth: 30
                                            Layout.preferredHeight: 30
                                            color: Material.accent
                                            radius: 15
                                            
                                            Text {
                                                anchors.centerIn: parent
                                                text: model.username ? model.username.charAt(0).toUpperCase() : "?"
                                                font.pixelSize: 14
                                                font.bold: true
                                                color: "white"
                                            }
                                        }
                                        
                                        Column {
                                            Layout.fillWidth: true
                                            
                                            Text {
                                                text: model.name || "Unnamed Profile"
                                                font.pixelSize: 12
                                                font.bold: true
                                                color: Material.foreground
                                            }
                                            
                                            Text {
                                                text: (model.username || "Unknown") + " • Last played: " + (model.lastPlayed || "Never")
                                                font.pixelSize: 10
                                                color: Material.hintTextColor
                                            }
                                        }
                                        
                                        Button {
                                            text: "Remove"
                                            Material.background: Material.Red
                                            onClicked: ConfigManager.removeProfile(model.uuid)
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                
                // Appearance
                GroupBox {
                    Layout.fillWidth: true
                    title: "Appearance"
                    
                    Material.background: Material.backgroundDimColor
                    
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 15
                        
                        RowLayout {
                            Layout.fillWidth: true
                            
                            Label {
                                Layout.preferredWidth: 120
                                text: "Theme:"
                                color: Material.foreground
                            }
                            
                            ComboBox {
                                Layout.preferredWidth: 200
                                model: ["Dark", "Light", "System"]
                                currentIndex: 0
                                
                                Material.background: Material.backgroundColor
                            }
                            
                            Item { Layout.fillWidth: true }
                        }
                        
                        RowLayout {
                            Layout.fillWidth: true
                            
                            Label {
                                Layout.preferredWidth: 120
                                text: "Accent Color:"
                                color: Material.foreground
                            }
                            
                            ComboBox {
                                Layout.preferredWidth: 200
                                model: ["Blue", "Purple", "Green", "Orange", "Red"]
                                currentIndex: 0
                                
                                Material.background: Material.backgroundColor
                            }
                            
                            Item { Layout.fillWidth: true }
                        }
                    }
                }
                
                // About
                GroupBox {
                    Layout.fillWidth: true
                    title: "About"
                    
                    Material.background: Material.backgroundDimColor
                    
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 10
                        
                        Text {
                            text: "Cryovex Launcher v1.0.0"
                            font.pixelSize: 16
                            font.bold: true
                            color: Material.foreground
                        }
                        
                        Text {
                            text: "A modern Minecraft launcher built with Qt 6 and C++"
                            font.pixelSize: 12
                            color: Material.hintTextColor
                            wrapMode: Text.WordWrap
                        }
                        
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 10
                            
                            Button {
                                text: "GitHub"
                                Material.background: Material.backgroundDimColor
                                onClicked: Qt.openUrlExternally("https://github.com/cryovex/launcher")
                            }
                            
                            Button {
                                text: "Report Bug"
                                Material.background: Material.backgroundDimColor
                                onClicked: Qt.openUrlExternally("https://github.com/cryovex/launcher/issues")
                            }
                            
                            Item { Layout.fillWidth: true }
                        }
                    }
                }
            }
        }
    }
    
    // File dialogs
    FolderDialog {
        id: gameDirectoryDialog
        title: "Select Minecraft Directory"
        currentFolder: "file:///" + ConfigManager.gameDirectory
        
        onAccepted: {
            ConfigManager.gameDirectory = selectedFolder.toString().replace("file:///", "")
        }
    }
    
    FileDialog {
        id: javaPathDialog
        title: "Select Java Executable"
        nameFilters: ["Java Executable (java.exe)", "All Files (*)"]
        
        onAccepted: {
            ConfigManager.javaPath = selectedFile.toString().replace("file:///", "")
        }
    }
}