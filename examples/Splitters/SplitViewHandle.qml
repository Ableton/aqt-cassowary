import QtQuick 2.3
import Aqt.Cassowary 1.0
import "moreUtils.js" as MoreUtils

Item {
    id: root

    property Solver solver
    property int idx
    property bool animate
    property color handleColor: "#333"
    property color handleColor2: "#333"

    readonly property bool dragging: mouse.drag.active
    readonly property alias vPosition: __vPosition
    readonly property alias ePosition: __ePosition

    signal fullPrev
    signal fullNext
    signal toggleCollapse

    Solver {
        extend: root.solver
        Variable { id: __vPosition }
        Stay {
            target: vPosition
            weight: 1 << (orientation == Qt.Horizontal ? 1 + views.length - idx : idx)
        }
        Edit {
            id: __ePosition
            target: vPosition
            strength: Strength.Medium
            enabled: anyDragging
            weight: mouse.drag.active ? 20 : 1
            suggested: vPosition.value
            Binding on suggested {
                when: mouse.drag.active
                value: orientation == Qt.Horizontal ? dragger.x : dragger.y
            }
        }
    }

    Item {
        id: dragger
        Binding on x {
            when: orientation == Qt.Horizontal && !mouse.drag.active
            value: vPosition.value
        }
        Binding on y {
            when: orientation == Qt.Vertical && !mouse.drag.active
            value: vPosition.value
        }
    }

    Rectangle {
        id: handle
        color: handleColor
        x: orientation == Qt.Horizontal ? vPosition.value : 0
        y: orientation == Qt.Vertical ? vPosition.value : 0
        width: root.width
        height: root.height

        SplitterBehavior on x { enabled: root.animate }
        SplitterBehavior on y { enabled: root.animate }
    }

    Rectangle {
        y: orientation == Qt.Vertical
            ? handle.y + handle.height/2 - height/2
            : MoreUtils.clamp(mouse.mouseY - height/2, 0, root.height - height)
        x: orientation == Qt.Horizontal
            ? handle.x + handle.width/2 - width/2
            : MoreUtils.clamp(mouse.mouseX - width/2, 0, root.width - width)
        height: orientation == Qt.Vertical ? 5 : 20
        width: orientation == Qt.Horizontal ? 5 : 20
        radius: 2
        color: handleColor2
        opacity: mouse.containsMouse || mouse.drag.active ? 1 : 0
        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InOutSine
                duration: 300
            }
        }
    }

    MouseArea {
        id: mouse
        hoverEnabled: true
        anchors.centerIn: handle
        width: orientation == Qt.Horizontal ? root.width + 12 : root.width
        height: orientation == Qt.Vertical ? root.height + 12 : root.height
        drag {
            target: dragger
            threshold: 1
            axis: orientation == Qt.Horizontal ? Drag.XAxis : Drag.YAxis
        }

        onDoubleClicked: root.toggleCollapse()

        SplitterButtons {
            y: orientation == Qt.Vertical ? -height + 6 : root.height - height
            x: orientation == Qt.Horizontal ? -width + 6 : root.width - width
            enabled: mouse.containsMouse && !mouse.drag.active
            onFull: root.fullPrev()
        }

        SplitterButtons {
            y: orientation == Qt.Vertical ? parent.height - 6 : root.height - height
            x: orientation == Qt.Horizontal ? parent.width - 6 : root.width - width
            enabled: mouse.containsMouse && !mouse.drag.active
            onFull: root.fullNext()
        }
    }
}
