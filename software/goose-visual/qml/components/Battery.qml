import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.15

import "qrc:/styles"

Rectangle {
    color: Style.background
    radius: 15

    readonly property real step: 0.04*height

    Rectangle {
        color: "transparent"
        anchors.fill: parent
        anchors.margins: 20

        Rectangle {
            id: batteryRect
            color: "black"
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
            id: batteryContent
            width: 0.6*parent.width
            height: 0.6*parent.height
            color: "black"
            radius: 1.5*step
            anchors.top: batteryRect.bottom
            anchors.horizontalCenter: parent.horizontalCenter

            Rectangle {
                id: batteryLevel
                color: Style.primary
                radius: 0.5*step
                width: parent.width - 2*step
                height: parent.height - step
                anchors.margins: step
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

        Text {
            id: batteryText
            text: "???"
            color: Style.text
            font.pixelSize: 3*step
            anchors.top: batteryContent.bottom
            anchors.topMargin: step
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    function mixColors(color1, color2, weight) {
        const c1 = color1;
        const c2 = color2;

        const r = (c1.r * weight + c2.r * (1 - weight));
        const g = (c1.g * weight + c2.g * (1 - weight));
        const b = (c1.b * weight + c2.b * (1 - weight));
        const a = (c1.a * weight + c2.a * (1 - weight));

        return Qt.rgba(r, g, b, a);
    }

    function setLevel(level) {
        const max = batteryContent.height - 3*step

        if(level>0.5)
            batteryLevel.color = mixColors(Style.primary, Style.batteryMedium, (level-0.5)*2)
        else
            batteryLevel.color = mixColors(Style.batteryMedium, Style.batteryLow, level*2)

        batteryLevel.height = max*level + step
        batteryText.text = Math.round(level*100) + "%"
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
            time +=0.002

            const value = 0.5*Math.sin(time) + 0.5

            setLevel(value)
        }
    }
}
