import QtQuick 2.10
import QtQuick.Controls 2.10
import QtQuick.Window 2.10

ApplicationWindow {
	id: applicationWindow

	visible: true
	width: 640
	height: 480
	signal buttonClick(string str)
	
	function doSomething() {
		console.log("doing something")
	}

	Column {
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		spacing: 10

		Button {
			anchors.horizontalCenter: parent.horizontalCenter
			text: "QML fajny"
			width: 200

			onClicked: {
				applicationWindow.buttonClick(text)
			}
		}

		Button {
			anchors.horizontalCenter: parent.horizontalCenter
			text: "ale C i tak lepsze"
			width: 200

			onClicked: {
				applicationWindow.buttonClick(text)
			}
		}

		Button {
			anchors.horizontalCenter: parent.horizontalCenter
			text: "generate"
			width: 200

			onClicked: {
				textBox.text = shared.getData()
			}
		}

		Text {
			id: textBox

			anchors.horizontalCenter: parent.horizontalCenter
			text: "reset"
		}
	}
}