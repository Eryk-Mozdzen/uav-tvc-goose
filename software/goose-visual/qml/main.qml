import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

import "qrc:/styles"
import "qrc:/components"

ApplicationWindow {
	id: mainWindow
	visible: true
	width: 1366
	height: 728
	color: Style.secondary
	flags: Qt.FramelessWindowHint | Qt.Window

	DragHandler {
		id: resizeHandler
		grabPermissions: TapHandler.TakeOverForbidden
		target: null

		property int border: 20

		onActiveChanged: if (active) {
			const position = resizeHandler.centroid.position;
			let edge = 0;
			if(position.x < border) edge |= Qt.LeftEdge;
			if(position.x >= width - border) edge |= Qt.RightEdge;
			if(position.y < border) edge |= Qt.TopEdge;
			if(position.y >= height - border) edge |= Qt.BottomEdge;
			if(edge !== 0){
				mainWindow.startSystemResize(edge);
			} else{
				mainWindow.startSystemMove()
			}
		}
	}

	Settings {
		id: settingsPopout
	}

	Rectangle {
		anchors.fill: parent
    	anchors.margins: 20
		color: "transparent"

		Actuators {
			id: actuators
			height: 0.7*parent.height
			anchors.top: parent.top
			anchors.left: parent.left
		}

		Scene {
			id: scene
			anchors.top: parent.top
			anchors.left: actuators.right
			anchors.right: batterAltimeterCol.left
			anchors.bottom: actuators.bottom
			anchors.leftMargin: 20
			anchors.rightMargin: 20
		}

		ColumnLayout {
			id: batterAltimeterCol
			spacing: 20
			width: 220
			height: parent.height
			anchors.right: parent.right

			Rectangle {
				width: parent.width
				height: 40
				color: "transparent"

				RowLayout {
					anchors.fill: parent
					spacing: 20

					Button {
						Layout.fillWidth: true
						Layout.fillHeight: true
						icon.color: Style.text
						icon.source: "images/setting.png"

						background: Rectangle {
							color: Style.secondary
						}

						onClicked: {
							settingsPopout.visible = true
						}
					}

					Button {
						Layout.fillWidth: true
						Layout.fillHeight: true
						icon.color: Style.text
						icon.source: "images/minus.png"

						background: Rectangle {
							color: Style.secondary
						}

						onClicked: mainWindow.showMinimized()
					}

					Button {
						Layout.fillWidth: true
						Layout.fillHeight: true
						icon.color: Style.text
						icon.source: "images/fullscreen.png"

						background: Rectangle {
							color: Style.secondary
						}

						property bool toggle: true

						onClicked: {
							if(toggle) {
								mainWindow.showMaximized()
							} else {
								mainWindow.showNormal()
							}

							toggle = !toggle
						}
					}

					Button {
						Layout.fillWidth: true
						Layout.fillHeight: true
						icon.color: Style.text
						icon.source: "images/close.png"

						background: Rectangle {
							color: Style.secondary
						}

						onClicked: Qt.quit()
					}
				}
			}

			Altimeter {
				id: altmeterRect
				Layout.fillHeight: true
        		width: parent.width
			}

			Battery {
				id: batteryRect
				Layout.fillHeight: true
        		width: parent.width
			}
		}

		Logger {
			id: logger
			anchors.top: actuators.bottom
			anchors.left: parent.left
			anchors.right: batterAltimeterCol.left
			anchors.bottom: parent.bottom
			anchors.topMargin: 20
			anchors.rightMargin: 20
		}

	}

}
