import QtQuick 2.3
import Aqt.Cassowary 1.0
import "moreUtils.js" as MoreUtils

Item {
    id: root

    width: orientation == Qt.Horizontal ? vSize.value : root.otherSize
    height: orientation == Qt.Vertical ? vSize.value : root.otherSize
    x: orientation == Qt.Horizontal ? vPosition.value : 0
    y: orientation == Qt.Vertical ? vPosition.value : 0

    property int orientation
    property var wrapped
    property Solver solver
    property var prevHandle
    property var nextHandle

    property bool dragging
    property bool resizing
    property bool animate

    property real totalSize
    property real otherSize
    property real fixedSize
    property real handleSize

    readonly property alias vPosition: __vPosition
    readonly property alias vSize: __vSize
    readonly property alias eSize: __eSize
    readonly property alias cCollapse: __cCollapse
    readonly property bool fixed: cCollapse.when || cSnap.when

    readonly property real initialRatio: MoreUtils.optional(
        wrapped, 'initialRatio', .3)
    readonly property real minimumSize: MoreUtils.optional(
        wrapped, 'minimumSize', 40)
    readonly property real maximumSize: MoreUtils.optional(
        wrapped, 'maximumSize', Infinity)
    readonly property var snapTo: MoreUtils.optional(
        wrapped, 'snapTo')

    Solver {
        extend: root.solver
        Variable { id: __vPosition }
        Variable { id: __vSize }
        Edit {
            id: __eSize
            target: vSize
            when: false
        }
        Constraint { expr: geq(vSize, minimumSize) }
        Constraint { expr: leq(vSize, maximumSize) }
        Constraint {
            expr: eq(vPosition,
                   root.prevHandle
                   ? plus(root.prevHandle.vPosition, root.handleSize)
                   : 0)
        }
        Constraint {
            expr: eq(plus(vPosition, vSize),
                     root.nextHandle ? root.nextHandle.vPosition : vTotalSize)
        }
        Constraint {
            id: cSnap
            strength: Strength.Strong
            weight: 100
            when: !!snapTo && !root.dragging && vSize.value > 0
            onWhenChanged: update()
            Component.onCompleted: update()
            function update() {
                expr = eq(vSize, MoreUtils.closest(snapTo, vSize.value))
            }
        }
        Constraint {
            id: __cCollapse
            strength: Strength.Strong
            weight: 1000
            when: false
            expr: eq(vSize, minimumSize)
        }
        Constraint {
            when: root.resizing
            strength: Strength.Strong
            weight: 2
            onWhenChanged: update()
            Component.onCompleted: update()
            function update() {
                var ratio = root.totalSize == 0
                    ? root.initialRatio
                    : vSize.value / (root.totalSize - root.fixedSize)
                expr = eq(vSize, times(minus(
                    vTotalSize, root.fixedSize), ratio))
            }
        }
    }

    Item {
        id: wrapper
        anchors.fill: parent
        children: wrapped
        Binding {
            target: wrapped.anchors
            property: "fill"
            value: wrapper
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "#262626"
        opacity: cCollapse.when ? 0.5 : 0
        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InOutSine
                duration: 300
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "transparent"
        opacity: wrapped.focus ? 0.6 : 0
        border {
            width: 2
            color: "#fff"
        }
        Behavior on opacity {
            NumberAnimation {
                easing.type: Easing.InOutSine
                duration: 300
            }
        }
    }

    SplitterBehavior on x { enabled: root.animate }
    SplitterBehavior on y { enabled: root.animate }
    SplitterBehavior on width { enabled: root.animate }
    SplitterBehavior on height { enabled: root.animate }
}
