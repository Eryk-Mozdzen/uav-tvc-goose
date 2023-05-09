import QtQuick 2.15
import Qt3D.Core 2.15
import Qt3D.Extras 2.12
import Qt3D.Render 2.15
import Qt3D.Input 2.0
import QtQuick.Scene3D 2.0

import "qrc:/styles"

Rectangle {
    color: Style.background
    radius: 15
    Scene3D {
        id: scene3d
        property real roll: 0.0
        property real pitch: 0.0
        property real yaw: 0.0

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
                viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
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
                id: gooseEntity
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
                        id: gooseTransform
                        rotation:{
                            fromAxesAndAngles(
                                        Qt.vector3d(0, 1, 0), scene3d.roll,
                                        Qt.vector3d(1, 0, 1), scene3d.pitch,
                                        Qt.vector3d(0, 0, 1), scene3d.yaw)
                        }
                    }
                ]
            }
        }
    }

    Timer {
        interval: 10; running: true; repeat: true
        property double time: 0
        onTriggered: {
            time += 0.002
            scene3d.yaw = 30* Math.sin(time*10)
            scene3d.pitch = 30* Math.sin(time*5)
            scene3d.roll = 30* Math.sin(time*30)
        }
    }
}
