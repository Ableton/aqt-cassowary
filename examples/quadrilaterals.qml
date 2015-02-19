import QtQuick 2.3
import QtQuick.Controls 1.1
import Aqt.Cassowary 1.0
import "Quadrilaterals"

// Inspired by:
//    http://constraints.cs.washington.edu/quad-demo/index.html
//    http://www.badros.com/greg/cassowary/js/quaddemo.html
//
// This application demonstrates a theorem about quadrilaterals, which
// is often used as a demonstration of constraint-based graphics. The
// theorem states that if one takes an arbitrary quadrilateral,
// bisects each side, and draws lines between the midpoints, the
// result is a parallelogram. The hypotheses of this theorem are
// expressed using required constraints that the x and y value of each
// midpoint is the average of the x and y values of the respective
// endpoints. To add a bit of complication, there are also required
// inequality constraints to keep all the points inside the demo
// window. When you move over an endpoint or midpoint and hold the
// mouse button down, this in effect adds a constraint that the
// selected point follow the mouse. This mouse constraint is strong
// but not required, so that if you try to move outside the window the
// selected point will bump against the side and stop. Notice the
// effect when you move a midpoint near the edge of the
// window. Finally, there are weak stay constraints on the x and y
// parts of each endpoint so that the solver tries to leave the
// endpoints at their current positions if possible.

ApplicationWindow {
    id: root
    width: 800
    height: 600

    Rectangle {
        color: "white"
        anchors.fill: parent
    }

    Solver {
        Box { id: b0; x0: 10; y0: 10; bounds: root }
        Box { id: b1; x0: 10; y0: 200; bounds: root }
        Box { id: b2; x0: 200; y0: 200; bounds: root }
        Box { id: b3; x0: 200; y0: 10; bounds: root }
        Box { id: m0 }
        Box { id: m1 }
        Box { id: m2 }
        Box { id: m3 }

        // Fix midpoints

        Constraint { expr: eq(m0.vX, divide(plus(b0.vX, b1.vX), 2)) }
        Constraint { expr: eq(m0.vY, divide(plus(b0.vY, b1.vY), 2)) }

        Constraint { expr: eq(m1.vX, divide(plus(b1.vX, b2.vX), 2)) }
        Constraint { expr: eq(m1.vY, divide(plus(b1.vY, b2.vY), 2)) }

        Constraint { expr: eq(m2.vX, divide(plus(b2.vX, b3.vX), 2)) }
        Constraint { expr: eq(m2.vY, divide(plus(b2.vY, b3.vY), 2)) }

        Constraint { expr: eq(m3.vX, divide(plus(b3.vX, b0.vX), 2)) }
        Constraint { expr: eq(m3.vY, divide(plus(b3.vY, b0.vY), 2)) }

        // Fix quadrilateral

        Constraint { expr: leq(plus(b0.vX, 20), b2.vX) }
        Constraint { expr: leq(plus(b0.vX, 20), b3.vX) }

        Constraint { expr: leq(plus(b1.vX, 20), b2.vX) }
        Constraint { expr: leq(plus(b1.vX, 20), b3.vX) }

        Constraint { expr: leq(plus(b0.vY, 20), b1.vY) }
        Constraint { expr: leq(plus(b0.vY, 20), b2.vY) }

        Constraint { expr: leq(plus(b3.vY, 20), b1.vY) }
        Constraint { expr: leq(plus(b3.vY, 20), b2.vY) }
    }

    Line { p0: b0.r; p1: b1.r }
    Line { p0: b0.r; p1: b3.r }
    Line { p0: b2.r; p1: b1.r }
    Line { p0: b2.r; p1: b3.r }
    Line { p0: m0.r; p1: m1.r }
    Line { p0: m0.r; p1: m3.r }
    Line { p0: m2.r; p1: m1.r }
    Line { p0: m2.r; p1: m3.r }
}
