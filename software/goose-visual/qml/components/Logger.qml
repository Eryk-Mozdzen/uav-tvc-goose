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
            width: logListView.width - loggerSliderBar.width - 20
            height: logText.height

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
                anchors.verticalCenter: logText.verticalCenter
                background: Rectangle {
                    color: "transparent"
                }

                icon.width: 14
                icon.height: 14
                icon.color: logRect.textColor
                icon.source: {
                    switch(type) {
                        case "DEBUG":   return "qrc:/icons/bug.png";          break
                        case "INFO":    return "qrc:/icons/information.png";    break
                        case "WARNING": return "qrc:/icons/alert.png";          break
                        case "ERROR":   return "qrc:/icons/close.png";          break
                    }
                }
            }

            Text {
                id: logText
                color: logRect.textColor
                text: logTextContent
                height: contentHeight
                font.family: "Monospace"
                font.pixelSize: 14
                anchors.left: logType.right
                anchors.right: logCounter.left
                anchors.leftMargin: 20
                wrapMode: Text.Wrap
            }

            Text {
                id: logCounter
                color: logRect.textColor
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                text: {
                    if(counter==1)
                        return ""
                    return counter
                }
                font.family: "Monospace"
                font.pixelSize: 14
                anchors.right: parent.right
            }
        }
    }

    function add(type, text) {

        if(loggerColumn.count>=50) {
            loggerColumn.remove(0)
        }

        if(loggerColumn.count) {
            let last = loggerColumn.get(loggerColumn.count-1)

            if(text==last.logTextContent && type==last.type) {
                last.counter++
                return
            }
        }

        loggerColumn.append({"type": type, "logTextContent": text, "counter": 1})

        if(!loggerSliderBar.pressed) {
            logListView.positionViewAtEnd()
        }
    }

    // TEST
    Timer {
        interval: 500
        running: true
        repeat: true
        onTriggered: {
            let types = ["DEBUG", "INFO", "WARNING", "ERROR"]
            let texts = ["witajcie w mojej kuchni", "sztosiwo kabanosiwo sztosiwo kabanosiwo sztosiwo kabanosiwo sztosiwo kabanosiwo sztosiwo kabanosiwo sztosiwo kabanosiwo"]
            let rand1 = Math.floor(Math.random()*types.length)
            let rand2 = Math.floor(Math.random()*texts.length)

            add(types[rand1], texts[rand2])
        }
    }

    ListView {
        id: logListView
        delegate: log
        clip: true
        spacing: 10

        anchors {
            fill: parent
            margins: 20
        }

        model: ListModel {
            id: loggerColumn
        }

        ScrollBar.vertical: ScrollBar {
            id: loggerSliderBar
            width: 20
            height: parent.height
            anchors.right: parent.right

            contentItem: Rectangle {
                color: Style.primary
                radius: 15
            }
        }
    }
}
