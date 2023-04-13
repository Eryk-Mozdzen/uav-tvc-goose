import QtQuick 2.15
import QtQuick.Controls 2.15

import "qrc:/styles"

Rectangle {
    color: Style.background
    radius: 15

    Component {
        id: log

        Text {
            id: logText
            color: log.color
            text: ""//log.text
            wrapMode: Text.Wrap
            font.family: "Monospace"
            font.pixelSize: 14
        }
    }

    function add(type, text) {
        let color = Style.text

        switch(type) {
            case "debug":   color = Style.loggerDebug;      break
            case "info":    color = Style.loggerInfo;       break
            case "warning": color = Style.loggerWarning;    break
            case "error":   color = Style.loggerError;      break
        }

        if(loggerColumn.children.length>=50) {
            loggerColumn.children[0].destroy()
        }

        if(loggerColumn.children.length>0) {
            const lastLog = loggerColumn.children[loggerColumn.children.length-1]

            const curr = type.toUpperCase() + " " + text
            const last = lastLog.text.substring(0, lastLog.text.lastIndexOf(" "));
            const counter = lastLog.text.split(" ").pop();

            if(curr==last) {
                lastLog.text = curr + " " + (Number(counter) + 1)
                return
            }
        }

        log.createObject(loggerColumn, {
            color: color,
            text: type.toUpperCase() + " " + text + " 1"
        })
    }

    // TEST
    Timer {
        interval: 200
        running: true
        repeat: true
        onTriggered: {
            let types = ["debug", "info", "warning", "error"]
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
