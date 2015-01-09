import QtQuick 2.3

Item {
    id: root

    property QtObject target
    property string property

    property bool active: true
    property real delay: 1000 / 10

    Timer {
        id: timer
        interval: root.delay
        onTriggered: {
            root.active = false
        }
    }

    property QtObject __lastTarget
    property string __lastProperty
    property var __callback

    function signalName(propertyName) {
        return "on" + propertyName[0].toUpperCase() + propertyName.slice(1) + "Changed";
    }

    function reconnect() {
        if (__lastTarget != target ||
            __lastProperty != property)
        {
            if (__lastTarget && __lastProperty) {
                __lastTarget[signalName(property)].disconnect(__callback)
                __lastTarget = __lastProperty = __callback = null
            }
            if (target && property) {
                __callback = function () {
                    root.active = true
                    timer.restart()
                }
                target[signalName(property)].connect(__callback)
                __lastTarget = target
                __lastProperty = property
                __callback()
            }
        }
    }

    onTargetChanged: reconnect()
    onPropertyChanged: reconnect()
    Component.onCompleted:  reconnect()
}
