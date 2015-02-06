import QtQuick 2.3
import QtQuick.Controls 1.3
import Aqt.Cassowary 1.0

ApplicationWindow {
    id: root
    width: 800
    height: 600

    Rectangle {
        color: "#000"
        anchors.fill: parent
    }

    Solver {
        Variable { id: centerY }
        Variable { id: circleX; }
        Variable { id: circleY; }
        Variable { id: marginX; initial: 100 }

        Stay {
            target: marginX
            strength: Strength.Required
        }

        Edit {
            id: editCircleX
            target: circleX
            suggested: dragger.x + dragger.width/2
        }

        Edit {
            id: editCircleY
            target: circleY
            suggested: dragger.y + dragger.height/2
        }

        Constraint { expr: eq(times(2, centerY), root.height) }
        Constraint { expr: geq(circleY, centerY) }
        Constraint { expr: leq(circleY, root.height) }
        Constraint { expr: leq(circleX, minus(root.width, marginX)) }
        Constraint { expr: geq(circleX, marginX) }
    }

    Rectangle {
        color: "#fa6"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: centerY.value
    }

    Rectangle {
        color: "white"
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        y: centerY.value
        height: centerY.value
    }

    Rectangle {
        color: "#cde"
        y: centerY.value
        height: centerY.value
        x: marginX.value
        width: root.width - marginX.value * 2
    }


    Item {
        id: dragger
        width: circle.width
        height: circle.height
        Binding on x {
            when: !mouse.drag.active
            value: circle.x
        }
        Binding on y {
            when: !mouse.drag.active
            value: circle.y
        }

        MouseArea {
            id: mouse
            anchors.fill: parent
            drag {
                target: dragger
                axis: Drag.XAndYAxis
            }
        }
    }

    Rectangle {
        id: circle
        color: mouse.drag.active ? "#04f" : "#48e"
        width: 20
        height: 20
        radius: 20

        x: circleX.value - width/2
        y: circleY.value - height/2
    }
}
