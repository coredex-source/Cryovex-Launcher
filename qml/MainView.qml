import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import CryovexLauncher 1.0

Item {
    RowLayout {
        anchors.fill: parent
        spacing: 20
        
        // Left sidebar - Version selection
        Rectangle {
            Layout.preferredWidth: 300
            Layout.fillHeight: true
            color: Material.backgroundDimColor
            radius: 10
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15
                
                Text {
                    text: "Minecraft Versions"
                    font.pixelSize: 18
                    font.bold: true
                    color: Material.foreground
                }
                
                ComboBox {
                    id: versionFilter
                    Layout.fillWidth: true
                    
                    model: ["All Versions", "Release", "Snapshot", "Beta", "Alpha"]
                    currentIndex: 0
                    
                    Material.background: Material.backgroundColor
                }
                
                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    
                    ListView {
                        id: versionList
                        model: [] // Will be populated by VersionManager
                        
                        delegate: ItemDelegate {
                            width: versionList.width
                            height: 60
                            
                            Rectangle {
                                anchors.fill: parent
                                anchors.margins: 2
                                color: parent.hovered ? Material.accent : "transparent"
                                radius: 5
                                opacity: parent.hovered ? 0.3 : 1.0
                                
                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 10
                                    
                                    Column {
                                        Layout.fillWidth: true
                                        
                                        Text {
                                            text: model.id || "Unknown Version"
                                            font.pixelSize: 14
                                            font.bold: true
                                            color: Material.foreground
                                        }
                                        
                                        Text {
                                            text: (model.type || "unknown") + " • " + (model.releaseTime || "")
                                            font.pixelSize: 11
                                            color: Material.hintTextColor
                                        }
                                    }
                                    
                                    Rectangle {
                                        Layout.preferredWidth: 8
                                        Layout.preferredHeight: 8
                                        radius: 4
                                        color: {
                                            switch(model.type) {
                                                case "release": return Material.Green
                                                case "snapshot": return Material.Orange
                                                case "beta": return Material.Purple
                                                case "alpha": return Material.Red
                                                default: return Material.Grey
                                            }
                                        }
                                    }
                                }
                            }
                            
                            onClicked: {
                                selectedVersion = model
                            }
                        }
                    }
                }
                
                Button {
                    Layout.fillWidth: true
                    text: "Refresh Versions"
                    Material.background: Material.accent
                    // onClicked: VersionManager.refreshVersions()
                }
            }
        }
        
        // Main content area
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 20
            
            // Game info card
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 200
                color: Material.backgroundDimColor
                radius: 10
                
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 20
                    
                    // Game icon
                    Rectangle {
                        Layout.preferredWidth: 120
                        Layout.preferredHeight: 120
                        color: Material.accent
                        radius: 10
                        
                        Text {
                            anchors.centerIn: parent
                            text: "🎮"
                            font.pixelSize: 48
                        }
                    }
                    
                    // Game info
                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        
                        Text {
                            text: "Minecraft"
                            font.pixelSize: 24
                            font.bold: true
                            color: Material.foreground
                        }
                        
                        Text {
                            id: selectedVersionText
                            text: selectedVersion ? "Version: " + selectedVersion.id : "No version selected"
                            font.pixelSize: 14
                            color: Material.hintTextColor
                        }
                        
                        Text {
                            text: "Profile: " + (ConfigManager.currentProfile ? ConfigManager.currentProfile.name : "Default")
                            font.pixelSize: 14
                            color: Material.hintTextColor
                        }
                        
                        Item { Layout.fillHeight: true }
                        
                        ProgressBar {
                            Layout.fillWidth: true
                            visible: false // Show when downloading
                            value: 0.0
                        }
                    }
                }
            }
            
            // Action buttons
            RowLayout {
                Layout.fillWidth: true
                spacing: 15
                
                // Play button - main action
                Button {
                    Layout.preferredWidth: 200
                    Layout.preferredHeight: 60
                    
                    text: "PLAY"
                    font.pixelSize: 18
                    font.bold: true
                    
                    Material.background: Material.Green
                    Material.foreground: "white"
                    
                    enabled: selectedVersion !== null && AuthManager.isLoggedIn
                    
                    // onClicked: GameLauncher.launchGame(...)
                }
                
                Item { Layout.fillWidth: true }
                
                // Secondary actions
                Button {
                    text: "Open Game Folder"
                    Material.background: Material.backgroundDimColor
                    // onClicked: Qt.openUrlExternally("file:///" + ConfigManager.gameDirectory)
                }
                
                Button {
                    text: "Mods"
                    Material.background: Material.backgroundDimColor
                    enabled: false // Future feature
                }
            }
            
            // Recent activity / News
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: Material.backgroundDimColor
                radius: 10
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    
                    Text {
                        text: "Recent Activity"
                        font.pixelSize: 16
                        font.bold: true
                        color: Material.foreground
                    }
                    
                    ScrollView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        
                        Column {
                            width: parent.width
                            spacing: 10
                            
                            Repeater {
                                model: 3
                                
                                Rectangle {
                                    width: parent.width
                                    height: 60
                                    color: "transparent"
                                    border.color: Material.dividerColor
                                    border.width: 1
                                    radius: 5
                                    
                                    RowLayout {
                                        anchors.fill: parent
                                        anchors.margins: 10
                                        
                                        Rectangle {
                                            Layout.preferredWidth: 40
                                            Layout.preferredHeight: 40
                                            color: Material.accent
                                            radius: 20
                                            
                                            Text {
                                                anchors.centerIn: parent
                                                text: "📰"
                                                font.pixelSize: 20
                                            }
                                        }
                                        
                                        Column {
                                            Layout.fillWidth: true
                                            
                                            Text {
                                                text: "Minecraft Update " + (1.20 + index)
                                                font.pixelSize: 12
                                                font.bold: true
                                                color: Material.foreground
                                            }
                                            
                                            Text {
                                                text: "New features and improvements"
                                                font.pixelSize: 10
                                                color: Material.hintTextColor
                                            }
                                        }
                                        
                                        Text {
                                            text: "2 days ago"
                                            font.pixelSize: 10
                                            color: Material.hintTextColor
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    property var selectedVersion: null
}