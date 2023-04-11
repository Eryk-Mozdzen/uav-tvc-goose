import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

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
		anchors.centerIn: parent
    	anchors.margins: 20
		color: "transparent"

		Rectangle {
			id: wingsRect
			anchors.top: parent.top
			anchors.left: parent.left
			color: Style.background
			radius: 15
			width: 0.7*parent.height
			height: 0.7*parent.height
		}

		Rectangle {
			id: model3dRect
			anchors.top: parent.top
			anchors.left: wingsRect.right
			anchors.right: batteryAltmeterCol.left
			height: wingsRect.height
			color: Style.background
			radius: 15
			anchors.leftMargin: 20
			anchors.rightMargin: 20
		}

		ColumnLayout {
			id: batteryAltmeterCol
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

			Rectangle {
				id: altmeterRect
				Layout.fillHeight: true
        		width: parent.width
				color: Style.background
				radius: 15
			}

			Rectangle {
				id: batteryRect
				Layout.fillHeight: true
        		width: parent.width
				color: Style.background
				radius: 15
			}
		}

		Rectangle {
			id: loggerRect
			anchors.top: wingsRect.bottom
			anchors.left: parent.left
			anchors.right: batteryAltmeterCol.left
			anchors.bottom: parent.bottom
			anchors.topMargin: 20
			anchors.rightMargin: 20
			color: Style.background
			radius: 15

			TextArea {
				color: Style.text
				font.family: "Monospace"
				font.pixelSize: 14
				wrapMode: TextArea.Wrap
				anchors.fill: parent

				text: "Welcome to the terminal\nType your commands here:\n> ls\nfile1.txt file2.txt\n> cat file1.txt\nHello world!\n> "
			}
		}

	}

}
