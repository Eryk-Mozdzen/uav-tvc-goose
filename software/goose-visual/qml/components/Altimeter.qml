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
            id: altimeterContent
            width: 3*step
            height: 0.65*parent.height
            color: "black"
            radius: 1.5*step
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter

            Rectangle {
                id: altimeterLevel
                color: Style.primary
                radius: 0.5*step
                width: parent.width - 2*step
                height: parent.height - 2*step
                anchors.margins: step
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

        Text {
            id: altimeterText
            text: "???"
            color: Style.text
            font.pixelSize: 3*step
            anchors.top: altimeterContent.bottom
            anchors.topMargin: 0.5*step
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Item {
            id: altimeterPressureRow
            width: parent.width
            height: altimeterPressureRect.height
            anchors.top: altimeterText.bottom
            anchors.topMargin: 0.5*step

            Text {
                text: "pressure:"
                color: Style.text
                height: parent.height
                font.pixelSize: step
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                anchors.rightMargin: step
                anchors.right: altimeterPressureRect.left
            }

            Rectangle {
                id: altimeterPressureRect
                color: "black"
                width: 5*step
                height: 1.5*step
                radius: 5
                anchors.left: parent.horizontalCenter

                Text {
                    id: altimeterPressureText
                    text: "???"
                    color: Style.text
                    font.pixelSize: step
                    anchors.fill: parent
                    anchors.rightMargin: 0.5*step
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Text {
                text: "hPa"
                color: Style.text
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: step
                anchors.leftMargin: 0.5*step
                anchors.left: altimeterPressureRect.right
            }
        }

        Item {
            width: parent.width
            height: altimeterDistanceRect.height
            anchors.top: altimeterPressureRow.bottom
            anchors.topMargin: 0.5*step

            Text {
                text: "distance:"
                color: Style.text
                height: parent.height
                font.pixelSize: step
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                anchors.rightMargin: step
                anchors.right: altimeterDistanceRect.left
            }

            Rectangle {
                id: altimeterDistanceRect
                color: "black"
                width: 5*step
                height: 1.5*step
                radius: 5
                anchors.left: parent.horizontalCenter

                Text {
                    id: altimeterDistanceText
                    text: "???"
                    color: Style.text
                    font.pixelSize: step
                    anchors.fill: parent
                    anchors.rightMargin: 0.5*step
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Text {
                text: "m"
                color: Style.text
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: step
                anchors.leftMargin: 0.5*step
                anchors.left: altimeterDistanceRect.right
            }
        }
    }

    Timer {
        id: altimeterPressureTimeout
        interval: 100
        onTriggered: altimeterPressureText.text = "???"
    }

    Timer {
        id: altimeterDistanceTimeout
        interval: 100
        onTriggered: altimeterDistanceText.text = "???"
    }

    Timer {
        id: altimeteraltitudeTimeout
        interval: 100
        onTriggered: altimeterText.text = "???"
    }

    function setAltitude(altitude) {
        altimeteraltitudeTimeout.restart()

        if(altitude<0)
            altitude = 0

        altimeterText.text = altitude.toFixed(2) + "m"

        const maxAltitude = 2.5

        if(altitude>maxAltitude)
            altitude = maxAltitude

        const max = altimeterContent.height - 3*step
        altimeterLevel.height = max*altitude/maxAltitude + step
    }

    function setPressure(pressure) {
        altimeterPressureTimeout.restart()
        altimeterPressureText.text = pressure.toFixed(3)
    }

    function setDistance(distance) {
        altimeterDistanceTimeout.restart()
        altimeterDistanceText.text = distance.toFixed(3)
    }
}
