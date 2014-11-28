import QtQuick 2.3
import "cassowary-qml.js" as C
import "."

SolverItem {
    property bool when: true
    property var target

    property var strength: Strength.weak
    property var weight: 1

    impl: when
        && target
        && target.impl
        && new C.c.StayConstraint(target.impl, strength, weight)

    function add(solver, impl) {
        message("Adding stay:", impl)
        solver.addConstraint(impl)
    }

    function remove(solver, impl) {
        message("Removing stay:", impl)
        solver.removeConstraint(impl)
    }
}
