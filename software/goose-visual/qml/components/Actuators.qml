import QtQuick 2.15
import QtQuick.Controls 2.15

import "qrc:/styles"

Rectangle {
    color: Style.background
    radius: 15
    width: height
    readonly property real step: 0.04*height
    Rectangle {
        x: parent.width/2 - width/2 - parent.width/3.8
        y: parent.height/2 - height/2 + parent.height/3.2
        width: 4*step
        height: 1.5*step
        radius: 10
        color: Style.background
        Rectangle {
            width: 2.2*step
            height: 1.5*step
            radius: 10
            color: "black"
            Text{
                id: downText
                anchors.fill: parent
                font.pixelSize: step
                anchors.rightMargin: 0.5*step
                color: Style.text
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: "???"
            }
            Text{
                anchors.fill: parent
                font.pixelSize: step
                color: Style.text
                verticalAlignment: Text.AlignVCenter
                text: "deg"
                anchors.leftMargin: 2.5*step
            }
        }
    }

    Rectangle {
        id: downWing
        width: parent.width/4
        height: width
        x: parent.width/2 - width/2
        y: parent.width/2 - width/2 + parent.width/3.2
        color: "black"
        radius: 10

        Rectangle {
            id: downWingBar
            width: parent.width/10
            height: parent.width - parent.width/6
            anchors.centerIn: parent
            color: Style.primary
            radius: parent.width/10

            Rectangle {
                id: downWingSlide
                width: 0
                height: parent.height - 10
                y: 10/2
                color: Style.third
            }
        }
    }

    Rectangle {
        x: parent.width/2 - width/2 + parent.width/4.2
        y: parent.height/2 - height/2 - parent.height/3.2
        width: 4*step
        height: 1.5*step
        radius: 10
        color: Style.background
        Rectangle {
            width: 2.2*step
            height: 1.5*step
            radius: 10
            color: "black"
            Text{
                id: upText
                anchors.fill: parent
                font.pixelSize: step
                anchors.rightMargin: 0.5*step
                color: Style.text
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: "???"
            }
            Text{
                anchors.fill: parent
                font.pixelSize: step
                color: Style.text
                verticalAlignment: Text.AlignVCenter
                text: "deg"
                anchors.leftMargin: 2.5*step
            }
        }
    }

    Rectangle {
        id: upWing
        width: parent.width/4
        height: width
        x: parent.width/2 - width/2
        y: parent.width/2 - width/2 - parent.width/3.2
        color: "black"
        radius: 10

        Rectangle {
            id: upWingBar
            width: parent.width/10
            height: parent.width - parent.width/6
            anchors.centerIn: parent
            color: Style.primary
            radius: parent.width/10

            Rectangle {
                id: upWingSlide
                width: 0
                height: parent.height - 10
                y: 10/2
                color: Style.third
                radius: parent.width/20
            }
        }
    }

    Rectangle {
        x: parent.width/2 - width/2 - parent.width/3.2
        y: parent.height/2 - height/2 - parent.height/5
        width: 4*step
        height: 1.5*step
        radius: 10
        color: Style.background
        Rectangle {
            width: 2.2*step
            height: 1.5*step
            radius: 10
            color: "black"
            Text{
                id: leftText
                anchors.fill: parent
                font.pixelSize: step
                anchors.rightMargin: 0.5*step
                color: Style.text
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: "???"
            }
            Text{
                anchors.fill: parent
                font.pixelSize: step
                color: Style.text
                verticalAlignment: Text.AlignVCenter
                text: "deg"
                anchors.leftMargin: 2.5*step
            }
        }
    }

    Rectangle {
        id: leftWing
        width: parent.width/4
        height: width
        x: parent.width/2 - width/2 - parent.width/3.2
        y: parent.width/2 - width/2
        color: "black"
        radius: 10

        Rectangle {
            id: leftWingBar
            width: parent.width - parent.width/6
            height: parent.width/10
            anchors.centerIn: parent
            color: Style.primary
            radius: parent.width/10

            Rectangle {
                id: leftWingSlide
                x: 10/2
                width: parent.width - 10
                height: 0
                color: Style.third
            }
        }
    }

    Rectangle {
        x: parent.width/2 - width/2 + parent.width/3.2
        y: parent.height/2 - height/2 + parent.height/5
        width: 4*step
        height: 1.5*step
        radius: 10
        color: Style.background
        Rectangle {
            width: 2.2*step
            height: 1.5*step
            radius: 10
            color: "black"
            Text{
                id: rightText
                anchors.fill: parent
                font.pixelSize: step
                anchors.rightMargin: 0.5*step
                color: Style.text
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                text: "???"
            }
            Text{
                anchors.fill: parent
                font.pixelSize: step
                color: Style.text
                verticalAlignment: Text.AlignVCenter
                text: "deg"
                anchors.leftMargin: 2.5*step
            }
        }
    }

    Rectangle {
        id: rightWing
        width: parent.width/4
        height: width
        x: parent.width/2 - width/2 + parent.width/3.2
        y: parent.width/2 - width/2
        color: "black"
        radius: 10

        Rectangle {
            id: rightWingBar
            width: parent.width - parent.width/6
            height: parent.width/10
            anchors.centerIn: parent
            color: Style.primary
            radius: parent.width/10

            Rectangle {
                id: rightWingSlide
                x: 10/2
                width: parent.width - 10
                height: 0
                color: Style.third
            }
        }

    }

    Rectangle {
        id: circle
        width: parent.width/3
        height: width
        anchors.centerIn: parent
        color: "black"
        radius: width/2

        Rectangle {
            width: parent.width/1.2
            height: width
            anchors.centerIn: parent
            color: Style.primary
            radius: width/1.2

            Rectangle {
                id: rotor1
                anchors.centerIn: parent
                width: parent.height+5
                height: 10
                color: "black"
                rotation: 45
            }

            Rectangle {
                id: rotor2
                anchors.centerIn: parent
                width: parent.height+5
                height: 10
                color: "black"
                rotation: -45
            }


            Rectangle {
                width: parent.width/1.2
                height: width
                anchors.centerIn: parent

                color: "black"
                radius: width/1.2

                Rectangle {
                    width: parent.width/1.2
                    height: width
                    anchors.centerIn: parent

                    color: Style.secondary
                    radius: width/1.2
                    Text {
                        anchors.centerIn: parent
                        font.pixelSize: step*0.45
                        color: Style.text
                        text: "motor throttle:\n\n\n\n\n"
                        fontSizeMode: Text.Fit
                    }

                    Text {
                        id: throttleText
                        anchors.centerIn: parent
                        font.pixelSize: step*1.5
                        color: Style.text
                        text: "???%"
                        fontSizeMode: Text.Fit
                        font.bold: true
                    }
                }
            }
        }
    }


    function setThrottle(throttle) {
        throttleText.text = throttle + "%"
        rotor1.rotation +=  throttle / 4
        rotor2.rotation +=  throttle / 4
    }

    function setLeftWing(angle) {
        if (angle >= 0.0){
            leftWingSlide.anchors.bottom = leftWingBar.top
            leftWingSlide.anchors.top =leftWingBar.center
        }
        else{
            leftWingSlide.anchors.bottom = leftWingBar.center
            leftWingSlide.anchors.top = leftWingBar.bottom
        }
        leftWingSlide.height = Math.abs(angle) / 30 * leftWing.height/2
        leftWingSlide.height *= 0.75
        leftText.text = Math.floor(angle)
    }

    function setRightWing(angle) {
        if (angle >= 0.0){
            rightWingSlide.anchors.bottom = rightWingBar.top
            rightWingSlide.anchors.top =rightWingBar.center
        }
        else{
            rightWingSlide.anchors.bottom = rightWingBar.center
            rightWingSlide.anchors.top = rightWingBar.bottom
        }
        rightWingSlide.height = Math.abs(angle) / 30 * rightWing.height/2
        rightWingSlide.height *= 0.75
        rightText.text = Math.floor(angle)
    }

    function setUpWing(angle) {
        if (angle >= 0.0){
            upWingSlide.anchors.left = upWingBar.right
            upWingSlide.anchors.right =upWingBar.center
        }
        else{
            upWingSlide.anchors.left = upWingBar.center
            upWingSlide.anchors.right = upWingBar.left
        }
        upWingSlide.width = Math.abs(angle) / 30 * upWing.height/2
        upWingSlide.width *= 0.75
        upText.text = Math.floor(angle)
    }

    function setDownWing(angle) {
        if (angle >= 0.0){
            downWingSlide.anchors.left = downWingBar.right
            downWingSlide.anchors.right =downWingBar.center
        }
        else{
            downWingSlide.anchors.left = downWingBar.center
            downWingSlide.anchors.right = downWingBar.left
        }
        downWingSlide.width = Math.abs(angle) / 30 * downWing.height/2
        downWingSlide.width *= 0.75
        downText.text = Math.floor(angle)
    }

    Timer {
        interval: 100; running: true; repeat: true
        property double time: 0
        onTriggered: {
            time += 0.002
            setThrottle(Math.floor(50 * Math.sin(time*10) + 50))
            setRightWing(30*Math.sin(50*time+.4))
            setLeftWing(30*Math.sin(30*time+.4))
            setDownWing(30*Math.sin(-50*time+.4))
            setUpWing(30*Math.sin(-30*time+.4))
        }
    }
}
