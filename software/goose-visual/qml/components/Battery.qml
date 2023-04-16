import QtQuick 2.15
import QtQuick.Layouts 1.15

import "qrc:/styles"

Rectangle {
    color: Style.background
    radius: 15

    readonly property real step: 0.05*height

    Rectangle {
        color: "transparent"
        anchors.fill: parent
        anchors.margins: 20

        Rectangle {
            id: batteryRect
            color: "white"
            radius: 0.5*step
            width: 0.25*parent.width
            height: step
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter

            Rectangle {
                color: batteryRect.color
                height: batteryRect.radius
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
            }
        }

        Rectangle {
            width: 0.65*parent.width
            height: 0.65*parent.height
            color: "white"
            radius: 1.5*step
            anchors.top: batteryRect.bottom
            anchors.horizontalCenter: parent.horizontalCenter

            Rectangle {
                id: batteryContent
                color: "black"
                radius: step
                anchors.fill: parent
                anchors.margins: step

                Rectangle {
                    id: batteryLevel
                    color: Style.primary
                    radius: 0.5*step
                    width: parent.width - step
                    height: parent.height
                    anchors.margins: 0.5*step
                    anchors.bottom: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }

    function setLevel(level) {
        const max = batteryContent.height - 2*step

        batteryLevel.height = max*level + step
    }

    function setVoltage(voltage) {

    }

    function setCurrent(current) {

    }

    // TEST
    Timer {
        interval: 10
        running: true
        repeat: true

        property real time: 0

        onTriggered: {
            time +=0.01

            const value = 0.5*Math.sin(time) + 0.5

            setLevel(value)
        }
    }
}
