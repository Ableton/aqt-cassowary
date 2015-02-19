import Aqt.Cassowary 1.0
import QtQuick 2.3

Solver {
    property alias vX: vX
    property alias vY: vY
    property alias x0: vX.initial
    property alias y0: vY.initial
    property alias r: r
    property var bounds: {}

    Variable {
        id: vX
        Stay {}
        Edit {
            when: mouse.drag.active
            suggested: dragger.x
        }
    }
    Variable {
        id: vY
        Stay {}
        Edit {
            when: mouse.drag.active
            suggested: dragger.y
        }
    }
    Constraint { expr: geq(vX, 0) }
    Constraint { expr: geq(vY, 0) }
    Constraint { expr: leq(vX, !bounds ? null : bounds.width - r.width) }
    Constraint { expr: leq(vY, !bounds ? null : bounds.height  - r.height) }

    Rectangle {
        id: r
        x: vX.value
        y: vY.value
        width: 20
        height: 20
        color: "black"
        radius: 4
        opacity: mouse.drag.active ? 1 : 0.5

        MouseArea {
            id: mouse
            anchors.fill: parent
            drag {
                threshold: 0
                smoothed: false
                target: dragger
            }
        }
    }

    Item {
        id: dragger
        Binding on x {
            when: !mouse.drag.active
            value: vX.value
        }
        Binding on y {
            when: !mouse.drag.active
            value: vY.value
        }
    }
}
