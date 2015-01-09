import QtQuick 2.3
import Ableton.Cassowary 1.0
import "moreUtils.js" as MoreUtils

Item {
    id: root

    property list<Item> views
    default property alias __views: root.views

    property int orientation: Qt.Vertical

    property int modifiers: Qt.NoModifier // Qt.ControlModifier | Qt.AltModifier

    property real handleSize:  1
    property color handleColor: "#333"
    property color handleColor2: "#333"

    property int __focusedView
    property var __wrappers: MoreUtils.getItems(wrappers)
    property var __handles: MoreUtils.getItems(handles)

    readonly property alias solver: __solver

    readonly property real fixedSize: !__wrappers || !__handles ? 0
        : handleSize * __handles.length + __wrappers.reduce(
            function (s, w) {
                return w.fixed
                    ? s + (orientation == Qt.Horizontal ? w.width : w.height)
                    : s
            }, 0)

    property alias resizing: sizeMovement.active

    readonly property bool dragging: __handles.some(function (h) {
        return h.dragging
    })
    readonly property alias anyDragging: root.dragging
    readonly property bool animate: !dragging && !resizing


    Solver {
        id: __solver
        debug: true

        Variable { id: vTotalSize }
        Edit {
            target: vTotalSize
            strength: Strength.Required
            suggested: orientation == Qt.Horizontal ? root.width : root.height
        }
    }

    Repeater {
        id: wrappers
        model: root.views

        delegate: SplitViewWrapper {
            orientation: root.orientation
            wrapped: modelData
            solver: root.solver
            prevHandle: __handles[index - 1]
            nextHandle: __handles[index]

            dragging: root.dragging
            resizing: root.resizing
            animate: root.animate

            totalSize: orientation == Qt.Horizontal ? root.width : root.height
            otherSize: orientation == Qt.Vertical ? root.width : root.height
            fixedSize: root.fixedSize
            handleSize: root.handleSize

            Connections {
                target: modelData
                onFocusChanged: if (modelData.focus) __focusedView = index
            }
        }
    }

    Repeater {
        id: handles
        model: root.views.length - 1

        delegate: SplitViewHandle {
            width: orientation == Qt.Horizontal ? root.handleSize : root.width
            height: orientation == Qt.Vertical ? root.handleSize : root.height

            solver: root.solver
            idx: index

            animate: root.animate
            handleColor: root.handleColor
            handleColor2: root.handleColor2

            readonly property var prevWrapper: __wrappers[index]
            readonly property var nextWrapper: __wrappers[index + 1]

            onDraggingChanged: {
                root.reconcile()
            }
            onToggleCollapse: root.toggleCollapse(
                orientation == Qt.Horizontal ? index : index + 1)
            onFullPrev: root.full(index)
            onFullNext: root.full(index + 1)
        }
    }

    PropertyMovement {
        id: sizeMovement
        target: root
        property: orientation == Qt.Horizontal ? "width" : "height"
        onActiveChanged: reconcile()
    }

    function reconcile() {
        solver.commit()
        solver.defer(function () {
            __wrappers.forEach(function (w) {
                var collapsed = MoreUtils.almost(w.vSize.value, w.minimumSize)
                w.eSize.when = false
                w.cCollapse.when = false
                if (!collapsed)
                    w.eSize.suggested = w.vSize.value
            })
            solver.resolve()
            __handles.forEach(function (h) {
                h.ePosition.suggested = h.vPosition.value
            })
        })
    }

    function toggleCollapse(index) {
        var w = __wrappers[index]
        if (w) {
            if (MoreUtils.almost(w.vSize.value, w.minimumSize))
                uncollapse(index)
            else
                collapse(index)
        }
    }

    function collapse(index) {
        var w = __wrappers[index]
        if (w && !MoreUtils.almost(w.vSize.value, w.minimumSize)) {
            w.eSize.when = false
            w.cCollapse.when = true
            reconcile()
        }
    }

    function uncollapse(index) {
        var w = __wrappers[index]
        if (w && MoreUtils.almost(w.vSize.value, w.minimumSize)) {
            w.eSize.when = true
            reconcile()
        }
    }

    function getFull() {
        var idx
        return __wrappers.length - 1 == __wrappers.reduce(function (s, w, i) {
            if (!w.cCollapse.when) idx = i
            return s + w.cCollapse.when
        }, 0) && idx
    }

    function full(index) {
        var lastFull = getFull()
        if (lastFull === index) {
            __wrappers.forEach(function (w) {
                w.eSize.when = true
                w.cCollapse.when = false
            })
            reconcile()
        } else {
            __wrappers.forEach(function (w, i) {
                w.cCollapse.when = i !== index
            })
        }
    }

    function focusNext() {
        views[
            (__focusedView + 1) % views.length
        ].focus = true
    }

    function focusPrev() {
        views[
            (__focusedView - 1) < 0 ? views.length - 1 : __focusedView - 1
        ].focus = true
    }

    function reveal(index) {

    }

    Keys.onPressed: {
        if ((event.modifiers & root.modifiers) == modifiers) {
            switch (event.key) {
            case Qt.Key_Return:
                full(__focusedView)
                break
            case Qt.Key_Down:
                focusNext()
                break
            case Qt.Key_Up:
                focusPrev()
                break
            default:
                if (event.key >= Qt.Key_1 && event.key <= Qt.Key_9) {
                    reveal(event.key - Qt.Key_1)
                }
                break
            }
        }
    }
}
