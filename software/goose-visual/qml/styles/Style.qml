pragma Singleton

import QtQuick 2.15

QtObject {
    readonly property color primary: "#00ADB5"
    readonly property color secondary: "#393E46"
    readonly property color background: "#222831"
    readonly property color text: "#EEEEEE"

    readonly property color loggerDebug: "gray"
    readonly property color loggerInfo: "cyan"
    readonly property color loggerWarning: "yellow"
    readonly property color loggerError: "red"
}
