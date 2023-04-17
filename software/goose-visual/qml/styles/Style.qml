pragma Singleton

import QtQuick 2.15

QtObject {
    readonly property color primary: "#00ADB5"
    readonly property color secondary: "#393E46"
    readonly property color background: "#222831"
    readonly property color text: "#EEEEEE"

    readonly property color loggerDebug: "#AAAAAA"
    readonly property color loggerInfo: primary
    readonly property color loggerWarning: "#DDDD00"
    readonly property color loggerError: "#DD0000"

    readonly property color batteryFull: primary
    readonly property color batteryMedium: "#DDDD00"
    readonly property color batteryEmpty: "#DD0000"
}
