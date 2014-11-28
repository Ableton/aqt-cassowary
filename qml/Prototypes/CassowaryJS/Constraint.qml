import QtQuick 2.3
import "cassowary-qml.js" as C
import "."

SolverItem {
    property bool when: true
    property var strength: Strength.required
    property var weight: 1
    property var expr

    impl: when && expr

    function v(x) {
        return x && x.impl || x
    }

    function plus() {
        return Array.prototype.reduce.call(
            arguments, guard2(C.c.plus));
    }

    function minus() {
        return Array.prototype.reduce.call(
            arguments, guard2(C.c.minus));
    }

    function times() {
        return Array.prototype.reduce.call(
            arguments, guard2(C.c.times));
    }

    function divide() {
        return Array.prototype.reduce.call(
            arguments, guard2(C.c.divide));
    }

    function guard2(reducer) {
        return function (s, i) {
            return v(s) == null || v(i) == null ? null : reducer(v(s), v(i))
        }
    }

    function eq(e1, e2) {
        if (v(e1) != null && v(e2) != null)
            return new C.c.Equation(v(e1), v(e2), strength, weight);
    }

    function leq(e1, e2) {
        if (v(e1) != null && v(e2) != null)
            return new C.c.Inequality(v(e1), C.c.LEQ, v(e2), strength, weight);
    }

    function geq(e1, e2) {
        if (v(e1) != null && v(e2) != null)
            return new C.c.Inequality(v(e1), C.c.GEQ, v(e2), strength, weight);
    }

    function add(solver, impl) {
        message("Adding constraint:", impl)
        solver.addConstraint(impl)
    }

    function remove(solver, impl) {
        message("Removing constraint:", impl)
        solver.removeConstraint(impl)
    }

    function update() {}

    Component.onCompleted: update()
    onWhenChanged: update()
}
