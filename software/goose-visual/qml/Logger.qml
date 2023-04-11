import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
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
