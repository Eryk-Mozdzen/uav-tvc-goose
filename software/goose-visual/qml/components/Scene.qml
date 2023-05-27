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
        width: 14*step
        height: 6.5*step
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
                Rectangle{
                    height: width
                    radius: step*0.5
                    anchors.left: parent.left
                    anchors.right: parent.right
                    color: "transparent"
                    anchors.bottom: parent.bottom
                    Text {
                        text: "roll:\npitch:\nyaw:"
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
                Text {
                    width:parent.width
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    text: "state"
                    color: Style.text
                    font.pixelSize: step*0.8
                    horizontalAlignment: Text.AlignHCenter
                }
                Rectangle{
                    height: width
                    radius: step*0.5
                    anchors.left: parent.left
                    anchors.right: parent.right
                    color: "Black"
                    anchors.bottom: parent.bottom
                    border.color: Style.primary
                    border.width: step*0.1
                    Text {
                        id: sceneStateText
                        text: "???\n???\n???"
                        color: Style.text
                        font.pixelSize: step*0.8
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
                Text {
                    width:parent.width
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    text: "command"
                    color: Style.text
                    font.pixelSize: step*0.8
                    horizontalAlignment: Text.AlignHCenter
                }
                Rectangle{
                    height: width
                    radius: step*0.5
                    anchors.left: parent.left
                    anchors.right: parent.right
                    color: "Black"
                    anchors.bottom: parent.bottom
                    border.color: "Grey"
                    border.width: step*0.1
                    Text {
                        id: sceneCmdText
                        text: "???\n???\n???"
                        color: Style.text
                        font.pixelSize: step*0.8
                        anchors.fill: parent
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
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
        width: 12*step
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
        onTriggered: sceneStateText.text = "???\n???\n???"
    }

    Timer {
        id: sceneCmdTimeout
        interval: 100
        onTriggered: sceneCmdText.text = "???\n???\n???"
    }

    function setAcceleration(acceleration) {
        sceneAccelerationTimeout.restart()
        sceneAccelerationText.text = acceleration.x.toFixed(2) + "\n" + acceleration.y.toFixed(2) + "\n" + acceleration.z.toFixed(2)
    }

    function setGyration(gyration) {
        sceneAccelerationTimeout.restart()
        sceneGyrationText.text = gyration.x.toFixed(2) + "\n" + gyration.y.toFixed(2) + "\n" + gyration.z.toFixed(2)
    }

    function setMagnitude(magnitude) {
        sceneAccelerationTimeout.restart()
        sceneMagnitudeText.text = magnitude.x.toFixed(2) + "\n" + magnitude.y.toFixed(2) + "\n" + magnitude.z.toFixed(2)
    }

    function setState(state) {
        sceneStateTimeout.restart()
        sceneStateText.text = state.x.toFixed(2) + "\n" + state.y.toFixed(2) + "\n" + state.z.toFixed(2)
        stateGooseTransform.rotationX = state.x
        stateGooseTransform.rotationY = state.y
        stateGooseTransform.rotationZ = state.z
    }

    function setCommand(command) {
        sceneCmdTimeout.restart()
        sceneCmdText.text = command.x.toFixed(2) + "\n" + command.y.toFixed(2) + "\n" + command.z.toFixed(2)
        cmdGooseTransform.rotationX = command.x
        cmdGooseTransform.rotationY = command.y
        cmdGooseTransform.rotationZ = command.z
    }
}
