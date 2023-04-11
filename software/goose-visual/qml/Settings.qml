import QtQuick 2.15
import QtQuick.Controls 2.15

Popup {
    width: 600
    height: 300
    anchors.centerIn: parent

    modal: true
    focus: true
    visible: false

    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    background: Rectangle {
        color: Style.background
        border.color: Style.primary
        radius: 15
    }
}
