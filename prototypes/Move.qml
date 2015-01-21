import QtQuick 2.1
import QtQuick.Controls 1.0
import "../Prototypes/CassowaryJS"

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

        Stay { target: marginX; strength: Strength.strong }

        Edit { id: editCircleY; target: circleY }
        Edit { id: editCircleX; target: circleX }

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

    Rectangle {
        id: circle
        color: mouse.drag.active ? "#04f" : "#48e"
        width: 20
        height: 20
        radius: 20
        x: circleX.value - width/2
        y: circleY.value - height/2

        Item {
            id: dragger
            width: parent.width
            height: parent.height
            onXChanged: {
                if (mouse.drag.active)
                    editCircleX.suggest(circle.x + circle.width/2 + x)
            }
            onYChanged: {
                if (mouse.drag.active)
                    editCircleY.suggest(circle.y + circle.height/2 + y)
            }
        }

        MouseArea {
            id: mouse
            anchors.fill: parent
            drag {
                target: dragger
                axis: Drag.XAndYAxis
                threshold: 0
                onActiveChanged: {
                    if (!active) {
                        dragger.x = 0
                        dragger.y = 0
                    }
                }
            }
        }
    }

    Component.onCompleted: show()
}
