import QtQuick 2.15
import QtQuick.Layouts 1.15

import "qrc:/styles"

Rectangle {
    color: Style.background
    radius: 15

    Rectangle {
        color: "transparent"
        anchors.fill: parent
        anchors.margins: 20

        Rectangle {
            id: batteryRect
            color: "white"
            radius: 10
            width: 0.25*parent.width
            height: 20
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
            radius: 35
            anchors.top: batteryRect.bottom
            anchors.horizontalCenter: parent.horizontalCenter

            Rectangle {
                id: batteryContent
                color: "black"
                radius: 20
                anchors.fill: parent
                anchors.margins: 20

                Rectangle {
                    id: batteryLevel
                    color: Style.primary
                    radius: 10
                    width: parent.width - 20
                    height: parent.height
                    anchors.margins: 10
                    anchors.bottom: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }

    function setLevel(level) {
        const max = batteryContent.height - 40

        batteryLevel.height = max*level + 20
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
