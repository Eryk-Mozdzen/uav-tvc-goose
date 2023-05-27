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
	title: "TVC Goose Monitor"
	minimumHeight: 300
	minimumWidth: 600

	Rectangle {
		anchors.fill: parent
    	anchors.margins: 20
		color: "transparent"

		Actuators {
			id: actuators
			anchors.top: parent.top
			anchors.left: parent.left
			anchors.bottom: scene.bottom
		}

		Scene {
			id: scene
			objectName: "scene"
			height: Math.min(0.65*parent.height, 0.5*(parent.width - batterAltimeterCol.width) - 20)
			anchors.top: parent.top
			anchors.left: actuators.right
			anchors.right: batterAltimeterCol.left
			anchors.leftMargin: 20
			anchors.rightMargin: 20
		}

		ColumnLayout {
			id: batterAltimeterCol
			objectName: "batterAltimeterCol"
			spacing: 20
			width: (1/1.618)*0.5*parent.height
			anchors.top: parent.top
			anchors.right: parent.right
			anchors.bottom: parent.bottom

			Altimeter {
				id: altimeter
				objectName: "altimeter"
				Layout.fillWidth: true
				Layout.fillHeight: true
			}

			Battery {
				id: battery
				objectName: "battery"
				Layout.fillWidth: true
				Layout.fillHeight: true
			}
		}

		Logger {
			id: logger
			objectName: "logger"
			anchors.top: actuators.bottom
			anchors.left: parent.left
			anchors.right: batterAltimeterCol.left
			anchors.bottom: parent.bottom
			anchors.topMargin: 20
			anchors.rightMargin: 20
		}

	}

}
