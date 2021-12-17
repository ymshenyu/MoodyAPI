import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QtGraphicalEffects

import client.api.mooody.me

ApplicationWindow {
    width: 392
    height: 815
    visible: true
    title: qsTr("Moody Camera App")
    id: rootWindow
    readonly property double buttonSize: Math.min(width / 2.5, height / 4)

    LinearGradient {
        anchors.fill: parent
        start: Qt.point(0, 0)
        end: Qt.point(rootWindow.width, rootWindow.height)
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: AppCore.DarkMode ? "#1f4042" : "#00d8e9"
            }
            GradientStop {
                position: 1.0
                color: AppCore.DarkMode ? "#003b1b" : "#00c159"
            }
        }
    }

    SvgButton {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 10
        width: 36
        height: 36
        source: "/assets/settings.svg"

        onClicked: {
            AppCore.DarkMode = !AppCore.DarkMode
        }
    }

    component VerticalSpacer: Item {
        Layout.fillHeight: true
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: buttonSize / 4

        VerticalSpacer {}
        VerticalSpacer {}

        Label {
            font.pixelSize: buttonSize / 5
            font.family: "System-ui"
            font.bold: true
            text: "Camera Status"
            color: AppCore.DarkMode ? "#a5a5a5" : "#2d2d2d"
            horizontalAlignment: Qt.AlignHCenter
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            font.pixelSize: buttonSize / 3
            font.family: "System-ui"
            font.bold: true
            text: AppCore.CameraStatus ? "ON" : "OFF"
            color: AppCore.DarkMode ? "#a5a5a5" : "#2d2d2d"
            horizontalAlignment: Qt.AlignHCenter
            Layout.alignment: Qt.AlignHCenter
        }

        VerticalSpacer {}

        GradientButton {
            color1: AppCore.DarkMode ? "#33712d" : "#1adf00"
            borderColor: AppCore.DarkMode ? "#063026" : "#188300"
            Layout.alignment: Qt.AlignHCenter
            text: qsTr("Power On")

            onClicked: {
                AppCore.CameraStatus = true
            }
        }

        GradientButton {
            color1: AppCore.DarkMode ? "#84501d" : "#eb7500"
            borderColor: AppCore.DarkMode ? "#452800" : "#834900"
            Layout.alignment: Qt.AlignHCenter
            text: qsTr("Power Off")

            onClicked: {
                AppCore.CameraStatus = false
            }
        }

        VerticalSpacer {}
        VerticalSpacer {}
        VerticalSpacer {}
    }
}
