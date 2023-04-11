import QtQuick 2.10
import QtQuick.Controls 2.10
import QtQuick.Window 2.10
import QtQuick.Layouts 1.15

ApplicationWindow {
	id: mainWindow
	visible: true
	width: 1366
	height: 728
	color: palette.secondary
	flags: Qt.FramelessWindowHint | Qt.Window

	QtObject {
		id: palette
		property color primary: "#00ADB5"
		property color secondary: "#393E46"
		property color background: "#222831"
		property color text: "#EEEEEE"
	}

    MouseArea {
		anchors.fill: parent
        hoverEnabled: true

        property int edges: 0;
        property int edgeOffest: 10;

        function setEdges(x, y) {
            edges = 0;

            if(x<edgeOffest) {
				edges |=Qt.LeftEdge;
			}

            if(x>(width - edgeOffest)) {
				edges |=Qt.RightEdge;
			}

            if(y<edgeOffest) {
				edges |=Qt.TopEdge;
			}

            if(y>(height - edgeOffest)) {
				edges |=Qt.BottomEdge;
			}
        }

        cursorShape: {
            return !containsMouse ? Qt.ArrowCursor:
                   edges == 3 || edges == 12 ? Qt.SizeFDiagCursor :
                   edges == 5 || edges == 10 ? Qt.SizeBDiagCursor :
                   edges & 9 ? Qt.SizeVerCursor :
                   edges & 6 ? Qt.SizeHorCursor : Qt.ArrowCursor;
        }

        onPositionChanged: {
			setEdges(mouseX, mouseY);
		}

        onPressed: {
			setEdges(mouseX, mouseY);

            if(edges && containsMouse) {
                startSystemResize(edges);
            }
        }
    }

	MouseArea {
		anchors.fill: parent
		anchors.margins: 100
		z: 100

		onPressed: {
			startSystemMove();
		}
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
			color: palette.background
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
			color: palette.background
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
						icon.color: palette.text
						icon.source: "images/minus.png"

						background: Rectangle {
							color: palette.secondary
						}

						onClicked: mainWindow.showMinimized()
					}

					Button {
						Layout.fillWidth: true
						Layout.fillHeight: true
						icon.color: palette.text
						icon.source: "images/fullscreen.png"

						background: Rectangle {
							color: palette.secondary
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
						icon.color: palette.text
						icon.source: "images/close.png"

						background: Rectangle {
							color: palette.secondary
						}

						onClicked: Qt.quit()
					}
				}
			}

			Rectangle {
				id: altmeterRect
				Layout.fillHeight: true
        		width: parent.width
				color: palette.background
				radius: 15
			}

			Rectangle {
				id: batteryRect
				Layout.fillHeight: true
        		width: parent.width
				color: palette.background
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
			color: palette.background
			radius: 15

			TextArea {
				color: palette.text
				font.family: "Monospace"
				font.pixelSize: 14
				wrapMode: TextArea.Wrap
				anchors.fill: parent

				text: "Welcome to the terminal\nType your commands here:\n> ls\nfile1.txt file2.txt\n> cat file1.txt\nHello world!\n> "
			}
		}

	}

}
