import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import "qrc:/styles"

Rectangle {
    color: Style.background
    radius: 15

    Component {
        id: log

        Rectangle {
            id: logRect
            color: "transparent"
            width: 700
            height: 20

            property string type
            property string text
            property int counter: 1
            property color textColor: {
                switch(type) {
                    case "DEBUG":   return Style.loggerDebug;      break
                    case "INFO":    return Style.loggerInfo;       break
                    case "WARNING": return Style.loggerWarning;    break
                    case "ERROR":   return Style.loggerError;      break
                }
            }

            RowLayout {
                anchors.fill: parent

                Text {
                    id: logType
                    color: logRect.textColor
                    text: logRect.type
                    font.family: "Monospace"
                    font.pixelSize: 14
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft
                }

                Text {
                    id: logText
                    color: logRect.textColor
                    text: logRect.text
                    font.family: "Monospace"
                    font.pixelSize: 14
                    Layout.fillWidth: true
                }

                Text {
                    id: logCounter
                    color: logRect.textColor
                    text: {
                        if(logRect.counter==1)
                            return ""
                        return logRect.counter.toString()
                    }
                    font.family: "Monospace"
                    font.pixelSize: 14
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignRight
                }
            }
        }
    }

    function add(type, text) {

        if(loggerColumn.children.length>=50) {
            loggerColumn.children[0].destroy()
        }

        if(loggerColumn.children.length>0) {
            let last = loggerColumn.children[loggerColumn.children.length-1]

            if(text==last.text && type==last.type) {
                last.counter = last.counter + 1
                return
            }
        }

        log.createObject(loggerColumn, {
            type: type,
            text: text
        })
    }

    // TEST
    Timer {
        interval: 200
        running: true
        repeat: true
        onTriggered: {
            let types = ["DEBUG", "INFO", "WARNING", "ERROR"]
            let rand = Math.floor(Math.random()*types.length)

            add(types[rand], "witajcie w mojej kuchni")
        }
    }

    ScrollView {
        id: scrollView
        clip: true
        anchors.fill: parent

        ScrollBar.vertical: ScrollBar {
            width: 20
            height: parent.height
            anchors.right: parent.right

            contentItem: Rectangle {
                color: Style.primary
                radius: 15
            }
        }

        Column {
            id: loggerColumn
            spacing: 2
            padding: 20
            anchors.fill: parent
        }
    }
}
