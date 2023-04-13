pragma Singleton

import QtQuick 2.15

QtObject {
    readonly property color primary: "#00ADB5"
    readonly property color secondary: "#393E46"
    readonly property color background: "#222831"
    readonly property color text: "#EEEEEE"

    readonly property color loggerDebug: text
    readonly property color loggerInfo: primary
    readonly property color loggerWarning: "#EEEE00"
    readonly property color loggerError: "#EE0000"
}
