import QtQuick 2.10
import QtQuick.Controls 2.10
import QtQuick.Window 2.10

ApplicationWindow {
	id: applicationWindow

	visible: true
	width: 640
	height: 480
	signal menuClicked(string str)

	Column {
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter

		spacing: 10

		BusyIndicator {
			anchors.horizontalCenter: parent.horizontalCenter
		}

		Button {
			text: "Wygeneruj losowy błąd"
			highlighted: true
			anchors.horizontalCenter: parent.horizontalCenter

			onClicked: {
				applicationWindow.menuClicked(text)
				dialog.open()
			}
		}
	}

	Dialog {
		id: dialog

		property int error: 0

		anchors.centerIn: parent
		width: parent.width * 0.7

		modal: true
		title: "Uwaga!"
		closePolicy: Dialog.NoAutoClose

		onAboutToShow: {
			checkBox.checked = false
			error = Math.round(Math.random() * 1000)
		}

		Column {
			id: column

			spacing: 20
			anchors.fill: parent

			Label {
				id: label

				text: "Nieoczekiwanie wystąpił błąd numer <b>" + dialog.error + "</b>. Potwierdź przeczytanie informacji."
				wrapMode: Text.WordWrap
				textFormat: Text.RichText
				width: parent.width
			}

			CheckBox {
				id: checkBox

				text: "Zrozumiałem"
				anchors.right: parent.right
			}

			Row {
				anchors.right: parent.right
				spacing: 20

				DialogButtonBox {
					id: ok

					enabled: checkBox.checked
					standardButtons: Dialog.Ok

					onClicked: {
						dialog.accepted()
						dialog.close()
					}
				}
			}
		}
	}
}