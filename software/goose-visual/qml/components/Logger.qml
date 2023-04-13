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
            width: logger.width - 60
            height: childrenRect.height

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

            Button {
                id: logType
                anchors.left: parent.left

                background: Rectangle {
                    color: "transparent"
                }

                icon.width: 14
                icon.height: 14
                icon.color: logRect.textColor
                icon.source: {
                    switch(logRect.type) {
                        case "DEBUG":   return "qrc:/icons/debug.png";          break
                        case "INFO":    return "qrc:/icons/information.png";    break
                        case "WARNING": return "qrc:/icons/alert.png";          break
                        case "ERROR":   return "qrc:/icons/close.png";          break
                    }
                }
            }

            Text {
                id: logText
                color: logRect.textColor
                text: logRect.text
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                font.family: "Monospace"
                font.pixelSize: 14
                anchors.left: logType.right
                anchors.leftMargin: 20
            }

            Text {
                id: logCounter
                color: logRect.textColor
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                text: {
                    if(logRect.counter==1)
                        return ""
                    return logRect.counter.toString()
                }
                font.family: "Monospace"
                font.pixelSize: 14
                anchors.right: parent.right
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
                last.counter++
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
        interval: 500
        running: true
        repeat: true
        onTriggered: {
            let types = ["DEBUG", "INFO", "WARNING", "ERROR"]
            let texts = ["witajcie w mojej kuchni", "sztosiwo kabanosiwo"]
            let rand1 = Math.floor(Math.random()*types.length)
            let rand2 = Math.floor(Math.random()*texts.length)

            add(types[rand1], texts[rand2])
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
            padding: 20
            anchors.fill: parent
        }
    }
}
