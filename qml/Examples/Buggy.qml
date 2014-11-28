import QtQuick 2.1
import QtQuick.Controls 1.0

import "../Prototypes/CassowaryJS/cassowary-qml.js" as C

ApplicationWindow {
    id: root
    width: 800
    height: 600

    Rectangle {
        color: "#cde"
        anchors.fill: parent
    }

    property var solver
    property var circleX
    property var circleY
    property var centerY

    Canvas {
        id: canvas
        anchors.fill: parent
        onPaint: {
            if (solver) {
                var context = canvas.getContext('2d')
                var radius = 10
                context.beginPath()
                context.arc(circleX.value, circleY.value, radius, 0, 2 * Math.PI, false)
                context.fillStyle = "rgba(64, 128, 200, 0.5)"
                context.fill()
                context.closePath()
            }
        }
    }

    property real ticks
    NumberAnimation on ticks {
        from: 0; to: 1
        loops: Animation.Infinite
        running: true
    }

    onTicksChanged: {
        solver
            .addEditVar(circleX, C.c.Strength.strong, 0)
            .addEditVar(circleY, C.c.Strength.strong, 0)
        solver.beginEdit()
            .suggestValue(circleX, Math.random() * canvas.width)
            .suggestValue(circleY, Math.random() * canvas.height)
            .resolve()
        solver.endEdit()
        canvas.requestPaint()
    }

    Component.onCompleted: {
        show()
        solver = new C.c.SimplexSolver()
        centerY = new C.c.Variable()
        circleX = new C.c.Variable()
        circleY = new C.c.Variable()
        solver
            .addConstraint(new C.c.Equation(
                C.c.times(2, centerY), canvas.height))
            .addConstraint(new C.c.Inequality(
                circleY, C.c.GEQ, centerY))
            .addConstraint(new C.c.Inequality(
                circleX, C.c.LEQ, canvas.width - 100))
            .addConstraint(new C.c.Inequality(
                circleX, C.c.GEQ, 100))
    }
}
