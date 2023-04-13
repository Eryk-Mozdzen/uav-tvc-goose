import QtQuick 2.15
import QtQuick.Controls 2.15

import "qrc:/styles"

Rectangle {
    color: Style.background
    radius: 15

    Component {
        id: log

        Text {
            color: Style.loggerError
            text: ""
            wrapMode: Text.Wrap
            font.family: "Monospace"
            font.pixelSize: 14
        }
    }

    //property int messageCounter: 0

    function add(type, text) {
        let color = Style.text

        switch(type) {
            case "debug":   color = Style.loggerDebug;      break
            case "info":    color = Style.loggerInfo;       break
            case "warning": color = Style.loggerWarning;    break
            case "error":   color = Style.loggerError;      break
        }

        /*messageCounter++

        if(messageCounter>5) {
            loggerColumn.children.splice(0, 1);
            messageCounter--
        }*/

        log.createObject(loggerColumn, {color: color, text: type + "\t" + text})
    }

    // TEST
    Timer {
        interval: 1000
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

        Column {
            id: loggerColumn
            spacing: 2
            padding: 20
            anchors.fill: parent
        }
    }
}
