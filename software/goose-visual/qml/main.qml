import QtQuick 2.10
import QtQuick.Controls 2.10
import QtQuick.Window 2.10
import QtQuick.Layouts 1.15

ApplicationWindow {
	visible: true
	width: 1366
	height: 728
	title: "TVC Goose visualization"
	color: "#424549"

	Rectangle {
		anchors.fill: parent
		anchors.centerIn: parent
    	anchors.margins: 20
		color: "transparent"

		Rectangle {
			id: wingsRect
			anchors.top: parent.top
			anchors.left: parent.left
			color: "#282b30"
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
			color: "black"
			radius: 15
			anchors.leftMargin: 20
			anchors.rightMargin: 20
		}

		ColumnLayout {
			id: batteryAltmeterCol
			spacing: 20
			width: 200
			height: parent.height
			anchors.right: parent.right

			Rectangle {
				id: altmeterRect
				Layout.fillHeight: true
        		width: parent.width
				color: "#282b30"
				radius: 15
			}

			Rectangle {
				id: batteryRect
				Layout.fillHeight: true
        		width: parent.width
				color: "#282b30"
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
			color: "black"
			radius: 15
		}

	}
}
