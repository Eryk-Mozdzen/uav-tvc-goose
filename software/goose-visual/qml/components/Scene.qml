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
        property real state_roll: 0.0
        property real state_pitch: 0.0
        property real state_yaw: 0.0

        property real cmd_roll: 0.0
        property real cmd_pitch: 0.0
        property real cmd_yaw: 0.0

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
                                        Qt.vector3d(0, 1, 0), scene3d.state_roll,
                                        Qt.vector3d(1, 0, 1), scene3d.state_pitch,
                                        Qt.vector3d(0, 0, 1), scene3d.state_yaw)
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
                                        Qt.vector3d(0, 1, 0), scene3d.cmd_roll,
                                        Qt.vector3d(1, 0, 1), scene3d.cmd_pitch,
                                        Qt.vector3d(0, 0, 1), scene3d.cmd_yaw)
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
        height: 6*step

        RowLayout{
            anchors.fill: parent
            spacing: step*0.5
            anchors.margins: step*0.5

            Rectangle{
                color: "transparent"
                Layout.fillWidth: true
                Layout.fillHeight: true
                Text {
                    text: "roll:\npitch:\nyaw:"
                    color: Style.text
                    font.pixelSize: step*0.8
                    anchors.fill: parent
                    anchors.rightMargin: 0.5*step
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter
                }
            }
            

            // Text {
            //     text: "State"
            //     color: Style.text
            //     font.pixelSize: step*0.8
            //     // anchors.fill: parent
            //     // anchors.rightMargin: 0.5*step
            //     // verticalAlignment: Text.AlignVCenter
            //     // horizontalAlignment: Text.AlignHCenter
            // }

            Rectangle{
                color: "transparent"
                Layout.fillWidth: true
                Layout.fillHeight: true
                Rectangle{
                    height: width
                    radius: step*0.5
                    anchors.left: parent.left
                    anchors.right: parent.right
                    color: "Black"
                    anchors.verticalCenter: parent.verticalCenter
                    border.color: Style.primary
                    border.width: step*0.1
                    Text {
                        id: sceneStateText
                        text: "???\n???\n???"
                        color: Style.text
                        font.pixelSize: step*0.8
                        anchors.fill: parent
                        anchors.rightMargin: 0.5*step
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
                    height: width
                    radius: step*0.5
                    anchors.left: parent.left
                    anchors.right: parent.right
                    color: "Black"
                    anchors.verticalCenter: parent.verticalCenter
                    border.color: "Grey"
                    border.width: step*0.1
                    Text {
                        id: sceneCmdText
                        text: "???\n???\n???"
                        color: Style.text
                        font.pixelSize: step*0.8
                        anchors.fill: parent
                        anchors.rightMargin: 0.5*step
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
        width: 10*step
        height: 12*step

        ColumnLayout{
            anchors.fill: parent
            spacing: step*0.5
            anchors.margins: step*0.5

            Rectangle{
                color: "Red"
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            Rectangle{
                color: "Green"
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            Rectangle{
                color: "Blue"
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }

    Timer {
        interval: 10; running: true; repeat: true
        property double time: 0
        onTriggered: {
            time += 0.002
            scene3d.state_yaw = 30* Math.sin(time*10)
            scene3d.state_pitch = 30* Math.sin(time*5)
            scene3d.state_roll = 30* Math.sin(time*30)
            scene3d.cmd_yaw = 30* Math.sin(time*10+0.5)
            scene3d.cmd_pitch = 30* Math.sin(time*5+0.5)
            scene3d.cmd_roll = 30* Math.sin(time*30+.05)
        }
    }
}
