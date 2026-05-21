import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Shotcut.Controls as Shotcut
import io.bossa.ai 1.0

Rectangle {
    id: root
    color: "#000000"
    border.color: "#1a1a1a"

    BossaMissionControl {
        id: aiController
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 15

        // BMB Header Style
        Text {
            text: "BOSSA MISSION CONTROL"
            font.family: "Cinzel Decorative"
            font.weight: Font.Bold
            font.pixelSize: 20
            Layout.alignment: Qt.AlignHCenter
            color: "#00e5ff" // Soda Cyan
        }

        Text {
            text: "IA AGENT STATUS: READY"
            font.family: "Segoe UI"
            font.pixelSize: 10
            font.letterSpacing: 2
            color: "#00e5ff"
            Layout.alignment: Qt.AlignHCenter
        }

        // Mission Input
        TextArea {
            id: missionInput
            Layout.fillWidth: true
            Layout.preferredHeight: 120
            placeholderText: "Enter mission list...\n1. Magic Cut (Silence removal)\n2. Add Bossa Subtitles\n3. Finalize"
            color: "white"
            font.pixelSize: 14
            wrapMode: TextEdit.Wrap
            background: Rectangle {
                color: "#050505"
                border.color: "#1a1a1a"
                radius: 4
                Rectangle {
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: 1
                    color: missionInput.activeFocus ? "#00e5ff" : "#1a1a1a"
                }
            }
        }

        // Execute Button
        Button {
            id: executeButton
            text: "EXECUTE MISSIONS"
            Layout.fillWidth: true
            Layout.preferredHeight: 45
            
            contentItem: Text {
                text: executeButton.text
                font.bold: true
                font.pixelSize: 16
                color: "black"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            background: Rectangle {
                color: executeButton.pressed ? "#00e5ff" : "#e040fb" // Magenta to Cyan
                radius: 4
                border.color: "#ffffff"
                border.width: executeButton.hovered ? 1 : 0
            }

            onClicked: {
                aiController.processMissions(missionInput.text)
            }
        }

        // Mission Log
        Text {
            text: "MISSION LOG"
            font.bold: true
            font.pixelSize: 12
            color: "#546e7a" // Soda Muted
        }

        ListView {
            id: logView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: aiController.missionLog
            delegate: RowLayout {
                width: logView.width
                height: 30
                Text { text: "> " + (modelData ? modelData.desc : ""); color: "white"; Layout.fillWidth: true; font.pixelSize: 11 }
                Text { text: (modelData ? modelData.status : ""); color: (modelData ? modelData.color : "white"); font.bold: true; font.pixelSize: 11 }
            }
        }
    }
}
