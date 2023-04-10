import QtQuick 2.10
import QtQuick.Controls 2.10
import QtQuick.Window 2.10

ApplicationWindow {
	id: applicationWindow

	visible: true
	width: 640
	height: 480

	Column {
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		spacing: 10

		Button {
			anchors.horizontalCenter: parent.horizontalCenter
			text: "generate"
			width: 200

			onClicked: {
				textBox.text = textBox.text + 1
			}
		}

		Text {
			id: textBox

			anchors.horizontalCenter: parent.horizontalCenter
			text: "reset"
		}
	}
}