import QtQuick 2.15
import Qt3D.Core 2.15
import Qt3D.Extras 2.12
import Qt3D.Render 2.15
import Qt3D.Input 2.0
import QtQuick.Layouts 1.15
import QtQuick.Scene3D 2.0

import "qrc:/styles"

Rectangle {
    color: "Black"
    readonly property real step: 0.04*height
    radius: 15
    Scene3D {
        id: scene3d
        anchors.fill: parent
        focus: true

        Entity {
            id: sceneRoot

            Camera {
                id: camera
                property int xPosition: 0
                property int yPosition: 0
                property int zPosition: 20

                projectionType: CameraLens.PerspectiveProjection
                fieldOfView: 60
                nearPlane : 0.1
                farPlane : 1000.0
                position: Qt.vector3d( xPosition, yPosition, zPosition )
                upVector: Qt.vector3d( 0.0, 0.0, 0.0)
                viewCenter: Qt.vector3d( 0.0, 3.0, 0.0 )
            }

            components: [
                RenderSettings {
                    id: renderSettings
                    activeFrameGraph: ForwardRenderer {
                        camera: camera
                        clearColor: "transparent"
                    }
                },
                InputSettings { }
            ]

            // Obj3D
            Entity {
                id: stateGooseEntity
                components: [
                    Mesh{
                        source: "qrc:/models/rocket.obj"
                    },
                    PhongMaterial {
                        ambient:  Style.primary
                        diffuse:   Style.primary
                        specular: "white"
                    },

                    Transform {
                        id: stateGooseTransform
                        rotation:{
                            fromAxesAndAngles(
                                        Qt.vector3d(0, 1, 0), 0,
                                        Qt.vector3d(1, 0, 1), 0,
                                        Qt.vector3d(0, 0, 1), 0)
                        }
                    }
                ]
            }

            Entity {
                id: cmdGooseEntity
                components: [
                    Mesh{
                        source: "qrc:/models/rocket.obj"
                    },
                    PhongAlphaMaterial {
                        ambient:  "white"
                        diffuse:   "white"
                        specular: "white"
                        alpha: 0.5
                    },

                    Transform {
                        id: cmdGooseTransform
                        rotation:{
                            fromAxesAndAngles(
                                        Qt.vector3d(0, 1, 0), 0,
                                        Qt.vector3d(1, 0, 1), 0,
                                        Qt.vector3d(0, 0, 1), 0)
                        }
                    }
                ]
            }
        }
    }


    Rectangle{
        color: Style.background
        radius: 0.5*step
        x: step
        y: step
        width: Math.min(14*step, parent.width/2 - 2*step)
        height: Math.min(7*step, parent.width/5.2 )
        RowLayout{
            anchors.fill: parent
            spacing: step*0.5
            anchors.rightMargin: step*0.5
            anchors.leftMargin: step*0.5

        }
        RowLayout{
            anchors.fill: parent
            spacing: step*0.5
            anchors.margins: step*0.5

            Rectangle{
                color: "transparent"
                Layout.fillWidth: true
                Layout.fillHeight: true
                // width: parent.width*0.2
                Text {
                    width: parent.width
                    text: ""
                    anchors.top: parent.top
                    color: Style.text
                    font.pixelSize: step*0.7

                    horizontalAlignment: Text.AlignHCenter

                    Rectangle{
                        height: width
                        radius: step*0.5
                        color: "transparent"

                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.bottom

                        Text {
                            text: "roll:\npitch:\nyaw:"
                            color: Style.text
                            font.pixelSize: step*0.7
                            anchors.fill: parent
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }
            }
            Rectangle{
                color: "transparent"
                // width: parent.width*0.3
                Layout.fillWidth: true
                Layout.fillHeight: true
                Text {
                    width: parent.width
                    text: "state"
                    anchors.top: parent.top
                    color: Style.text
                    font.pixelSize: step*0.7
                    horizontalAlignment: Text.AlignHCenter

                    Rectangle{
                        height: width
                        radius: step*0.5

                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.bottom
                        color: "Black"
                        border.color: Style.primary
                        border.width: step*0.1

                        Text {
                            id: sceneStateText
                            text: "???\n???\n???"
                            color: Style.text
                            font.pixelSize: step*0.7
                            anchors.fill: parent
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }
            }
            Rectangle{
                color: "transparent"
                // width: parent.width*0.35
                Layout.fillWidth: true
                Layout.fillHeight: true
                Text {
                    width: parent.width
                    text: "command"
                    anchors.top: parent.top
                    color: Style.text
                    font.pixelSize: step*0.7
                    horizontalAlignment: Text.AlignHCenter

                    Rectangle{
                        height: width
                        radius: step*0.5

                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.bottom
                        color: "Black"
                        border.color: "Grey"
                        border.width: step*0.1

                        Text {
                            id: sceneCmdText
                            text: "???\n???\n???"
                            color: Style.text
                            font.pixelSize: step*0.7
                            anchors.fill: parent
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }
                }
            }
        }
    }

    Rectangle{
        color: Style.background
        radius: 0.5*step
        x: parent.width - step - width
        y: step
        width:  Math.min(13*step, parent.width/2 - 2*step)
        height: 12*step

        ColumnLayout{
            anchors.fill: parent
            spacing: step*0.5
            anchors.margins: step*0.5

            // Acceleration
            Rectangle{
                color: "transparent"
                Layout.fillWidth: true
                Layout.fillHeight: true
                RowLayout{
                    anchors.fill: parent
                    spacing: step*0.5
                    // anchors.margins: step*0.5

                    Rectangle{
                        color: "transparent"
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Rectangle{
                            height: parent.height
                            radius: step*0.5
                            anchors.left: parent.left
                            anchors.right: parent.right
                            color: "transparent"
                            anchors.bottom: parent.bottom
                            Text {
                                text: "acc:"
                                color: Style.text
                                font.pixelSize: step*0.8
                                anchors.fill: parent
                                anchors.rightMargin: 0.5*step
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignRight
                            }
                        }
                    }
                    Rectangle{
                        color: "transparent"
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Rectangle{
                            height: width
                            radius: step*0.5

                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.verticalCenter: parent.verticalCenter
                            color: "Black"

                            Text {
                                id: sceneAccelerationText
                                text: "???\n???\n???"
                                color: Style.text
                                font.pixelSize: step*0.7
                                anchors.fill: parent
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }
                    }
                    Rectangle{
                        color: "transparent"
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Rectangle{
                            height: parent.height
                            radius: step*0.5
                            anchors.left: parent.left
                            anchors.right: parent.right
                            color: "transparent"
                            anchors.bottom: parent.bottom
                            Text {
                                text: "m/s^2"
                                color: Style.text
                                font.pixelSize: step*0.8
                                anchors.fill: parent
                                anchors.rightMargin: 0.5*step
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignRight
                            }
                        }
                    }
                }
            }

            // gyration
            Rectangle{
                color: "transparent"
                Layout.fillWidth: true
                Layout.fillHeight: true
                RowLayout{
                    anchors.fill: parent
                    spacing: step*0.5
                    // anchors.margins: step*0.5

                    Rectangle{
                        color: "transparent"
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Rectangle{
                            height: parent.height
                            radius: step*0.5
                            anchors.left: parent.left
                            anchors.right: parent.right
                            color: "transparent"
                            anchors.bottom: parent.bottom
                            Text {
                                text: "gyro:"
                                color: Style.text
                                font.pixelSize: step*0.8
                                anchors.fill: parent
                                anchors.rightMargin: 0.5*step
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignRight
                            }
                        }
                    }
                    Rectangle{
                        color: "transparent"
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Rectangle{
                            height: width
                            radius: step*0.5

                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.verticalCenter: parent.verticalCenter
                            color: "Black"

                            Text {
                                id: sceneGyrationText
                                text: "???\n???\n???"
                                color: Style.text
                                font.pixelSize: step*0.7
                                anchors.fill: parent
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }
                    }
                    Rectangle{
                        color: "transparent"
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Rectangle{
                            height: parent.height
                            radius: step*0.5
                            anchors.left: parent.left
                            anchors.right: parent.right
                            color: "transparent"
                            anchors.bottom: parent.bottom
                            Text {
                                text: "rad/s"
                                color: Style.text
                                font.pixelSize: step*0.8
                                anchors.fill: parent
                                anchors.rightMargin: 0.5*step
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignRight
                            }
                        }
                    }
                }
            }
            // magnetometer
            Rectangle{
                color: "transparent"
                Layout.fillWidth: true
                Layout.fillHeight: true
                RowLayout{
                    anchors.fill: parent
                    spacing: step*0.5

                    Rectangle{
                        color: "transparent"
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Rectangle{
                            height: parent.height
                            radius: step*0.5
                            anchors.left: parent.left
                            anchors.right: parent.right
                            color: "transparent"
                            anchors.bottom: parent.bottom
                            Text {
                                text: "mag:"
                                color: Style.text
                                font.pixelSize: step*0.8
                                anchors.fill: parent
                                anchors.rightMargin: 0.5*step
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignRight
                            }
                        }
                    }
                    Rectangle{
                        color: "transparent"
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Rectangle{
                            height: width
                            radius: step*0.5

                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.verticalCenter: parent.verticalCenter
                            color: "Black"

                            Text {
                                id: sceneMagnitudeText
                                text: "???\n???\n???"
                                color: Style.text
                                font.pixelSize: step*0.7
                                anchors.fill: parent
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }
                    }
                    Rectangle{
                        color: "transparent"
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Rectangle{
                            height: parent.height
                            radius: step*0.5
                            anchors.left: parent.left
                            anchors.right: parent.right
                            color: "transparent"
                            anchors.bottom: parent.bottom
                            Text {
                                text: "mGa"
                                color: Style.text
                                font.pixelSize: step*0.8
                                anchors.fill: parent
                                anchors.rightMargin: 0.5*step
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignRight
                            }
                        }
                    }
                }
            }
        }
    }

    Timer {
        id: sceneAccelerationTimeout
        interval: 100
        onTriggered: sceneAccelerationText.text = "???\n???\n???"
    }

    Timer {
        id: sceneGyrationTimeout
        interval: 100
        onTriggered: sceneGyrationText.text = "???\n???\n???"
    }

    Timer {
        id: sceneMagnitudeTimeout
        interval: 100
        onTriggered: sceneMagnitudeText.text = "???\n???\n???"
    }

    Timer {
        id: sceneStateTimeout
        interval: 100
        onTriggered: {
            sceneStateText.text = "???\n???\n???"
            stateGooseTransform.rotationX = 0
            stateGooseTransform.rotationY = 0
            stateGooseTransform.rotationZ = 0
        }
    }

    Timer {
        id: sceneCmdTimeout
        interval: 100
        onTriggered: {
            sceneCmdText.text = "???\n???\n???"
            cmdGooseTransform.rotationX = 0
            cmdGooseTransform.rotationY = 0
            cmdGooseTransform.rotationZ = 0
        }
    }

    function setAcceleration(acceleration) {
        sceneAccelerationTimeout.restart()
        var x = acceleration.x >= 0.0 ? '+' + acceleration.x.toFixed(3) : acceleration.x.toFixed(3)
        var y = acceleration.y >= 0.0 ? '+' + acceleration.y.toFixed(3) : acceleration.y.toFixed(3)
        var z = acceleration.z >= 0.0 ? '+' + acceleration.z.toFixed(3) : acceleration.z.toFixed(3)
        sceneAccelerationText.text = x + "\n" + y + "\n" + z
    }

    function setGyration(gyration) {
        sceneGyrationTimeout.restart()
        var x = gyration.x >= 0.0 ? '+' + gyration.x.toFixed(3) : gyration.x.toFixed(3)
        var y = gyration.y >= 0.0 ? '+' + gyration.y.toFixed(3) : gyration.y.toFixed(3)
        var z = gyration.z >= 0.0 ? '+' + gyration.z.toFixed(3) : gyration.z.toFixed(3)
        sceneGyrationText.text = x + "\n" + y + "\n" + z
    }

    function setMagnitude(magnitude) {
        sceneMagnitudeTimeout.restart()
        var x = magnitude.x >= 0.0 ? '+' + magnitude.x.toFixed(3) : magnitude.x.toFixed(3)
        var y = magnitude.y >= 0.0 ? '+' + magnitude.y.toFixed(3) : magnitude.y.toFixed(3)
        var z = magnitude.z >= 0.0 ? '+' + magnitude.z.toFixed(3) : magnitude.z.toFixed(3)
        sceneMagnitudeText.text = x + "\n" + y + "\n" + z
    }

    function setState(state) {
        sceneStateTimeout.restart()
        var x = state.x >= 0.0 ? '+' + state.x.toFixed(0) : state.x.toFixed(0)
        var y = state.y >= 0.0 ? '+' + state.y.toFixed(0) : state.y.toFixed(0)
        var z = state.z >= 0.0 ? '+' + state.z.toFixed(0) : state.z.toFixed(0)
        sceneStateText.text = x + "\n" + y + "\n" + z

        stateGooseTransform.rotationX = state.x
        stateGooseTransform.rotationY = state.y
        stateGooseTransform.rotationZ = state.z
    }

    function setCommand(command) {
        sceneCmdTimeout.restart()
        var x = command.x >= 0.0 ? '+' + command.x.toFixed(0) : command.x.toFixed(0)
        var y = command.y >= 0.0 ? '+' + command.y.toFixed(0) : command.y.toFixed(0)
        var z = command.z >= 0.0 ? '+' + command.z.toFixed(0) : command.z.toFixed(0)
        sceneCmdText.text = x + "\n" + y + "\n" + z
        cmdGooseTransform.rotationX = command.x
        cmdGooseTransform.rotationY = command.y
        cmdGooseTransform.rotationZ = command.z
    }
}
