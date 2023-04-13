import QtQuick 2.15
import QtQuick.Controls 2.15

import "qrc:/styles"

Rectangle {
    color: Style.background
    radius: 15
    width: height

    Rectangle {
        id: downText
        x: parent.width/2 - width/2 - parent.width/4.2
        y: parent.height/2 - height/2 + parent.height/3.2
        width: 75
        height: 25
        radius: 10
        color: Style.secondary
        Text{
            anchors.centerIn: parent
            font.pointSize: 12
            color: Style.text
            text: "69 deg"
            Timer {
                interval: 100; running: true; repeat: true
                onTriggered: parent.text = Math.floor(Style.angle_down) + " deg"
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
            width: 0
            height: parent.height - parent.height/6
            y: parent.height/2 - height/2
            x: parent.height/2 - height/2
            color: Style.third
            radius: parent.width/20
        }

        Rectangle {
            width: parent.width/10
            height: parent.width - parent.width/6
            anchors.centerIn: parent
            color: Style.primary
            radius: parent.width/10

            Rectangle {
                width: 0
                height: parent.height
                color: Style.third
                radius: parent.width/20

                Timer {
                    property double counter: 0
                    interval: 100; running: true; repeat: true
                    onTriggered: {
                        if (Style.angle_down >= 0.0){
                            parent.anchors.left = parent.parent.right
                            parent.anchors.right = parent.parent.center
                        }
                        else{
                            parent.anchors.left = parent.parent.center
                            parent.anchors.right = parent.parent.left
                        }
                        parent.width = Math.abs(Style.angle_down) / 30 * parent.parent.height/2 - parent.height/6
                        Style.angle_down = 30*Math.sin(counter-0.5)
                        counter += 0.1
                    }
                }
            }
        }
    }

    Rectangle {
        id: upText
        x: parent.width/2 - width/2 + parent.width/4.2
        y: parent.height/2 - height/2 - parent.height/3.2
        width: 75
        height: 25
        radius: 10
        color: Style.secondary
        Text{
            anchors.centerIn: parent
            font.pointSize: 12
            color: Style.text
            text: "69 deg"
            Timer {
                interval: 100; running: true; repeat: true
                onTriggered: parent.text = Math.floor(Style.angle_up) + " deg"
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
            width: parent.width/10
            height: parent.width - parent.width/6
            anchors.centerIn: parent
            color: Style.primary
            radius: parent.width/10

            Rectangle {
                width: 0
                height: parent.height
                color: Style.third
                radius: parent.width/20

                Timer {
                    property double counter: 0
                    interval: 100; running: true; repeat: true
                    onTriggered: {
                        if (Style.angle_up >= 0.0){
                            parent.anchors.left = parent.parent.right
                            parent.anchors.right = parent.parent.center
                        }
                        else{
                            parent.anchors.left = parent.parent.center
                            parent.anchors.right = parent.parent.left
                        }
                        parent.width = Math.abs(Style.angle_up) / 30 * parent.parent.height/2 - parent.height/6
                        Style.angle_up = 30*Math.sin(counter-0.5)
                        counter += 0.1
                    }
                }
            }
        }
    }

    Rectangle {
        id: leftText
        x: parent.width/2 - width/2 - parent.width/3.2
        y: parent.height/2 - height/2 - parent.height/5
        width: 75
        height: 25
        radius: 10
        color: Style.secondary
        Text{
            anchors.centerIn: parent
            font.pointSize: 12
            color: Style.text
            text: "69 deg"
            Timer {
                interval: 100; running: true; repeat: true
                onTriggered: parent.text = Math.floor(Style.angle_left) + " deg"
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
            width: parent.width - parent.width/6
            height: parent.width/10
            anchors.centerIn: parent
            color: Style.primary
            radius: parent.width/10

            Rectangle {
                width: parent.width
                height: 0
                color: Style.third
                radius: parent.width/20

                Timer {
                    property double counter: 0
                    interval: 100; running: true; repeat: true
                    onTriggered: {
                        if (Style.angle_left >= 0.0){
                            parent.anchors.bottom = parent.parent.top
                            parent.anchors.top = parent.parent.center
                        }
                        else{
                            parent.anchors.bottom =parent.parent.center
                            parent.anchors.top = parent.parent.bottom
                        }
                        parent.height = Math.abs(Style.angle_left) / 30 * parent.parent.parent.height/2 - parent.width/6
                        Style.angle_left = 30*Math.sin(counter+.4)
                        counter += 0.2
                    }
                }
            }
        }
    }

    Rectangle {
        id: rightText
        x: parent.width/2 - width/2 + parent.width/3.2
        y: parent.height/2 - height/2 + parent.height/5
        width: 75
        height: 25
        radius: 10
        color: Style.secondary
        Text{
            anchors.centerIn: parent
            font.pointSize: 12
            color: Style.text
            text: "69 deg"
            Timer {
                property double counter: 0
                interval: 100; running: true; repeat: true
                onTriggered: parent.text = Math.floor(Style.angle_right) + " deg"
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
            width: parent.width - parent.width/6
            height: parent.width/10
            anchors.centerIn: parent
            color: Style.primary
            radius: parent.width/10

            Rectangle {
                width: parent.width
                height: 0
                color: Style.third
                radius: parent.width/20

                Timer {
                    property double counter: 0
                    interval: 100; running: true; repeat: true
                    onTriggered: {
                        if (Style.angle_right >= 0.0){
                            parent.anchors.bottom = parent.parent.top
                            parent.anchors.top = parent.parent.center
                        }
                        else{
                            parent.anchors.bottom =parent.parent.center
                            parent.anchors.top = parent.parent.bottom
                        }
                        parent.height = Math.abs(Style.angle_right) / 30 * parent.parent.parent.height/2 - parent.width/6
                        Style.angle_right = 30*Math.sin(counter+.4)
                        counter += 0.5
                    }
                }
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
                        font.pointSize: 6
                        color: Style.text
                        text: "motor throttle:\n\n\n\n\n"
                        fontSizeMode: Text.Fit
                    }

                    Text {
                        id: throttle
                        anchors.centerIn: parent
                        font.pointSize: 20
                        color: Style.text
                        text: "100%"
                        fontSizeMode: Text.Fit
                        font.bold: true

                        property int throttle: 0
                        Timer {
                            interval: 500; running: true; repeat: true
                            onTriggered: {
                                parent.throttle = Math.floor((parent.throttle+1) % 101 )
                                parent.text = parent.throttle + "%"
                            }
                        }
                    }
                }
            }
        }
    }
}
