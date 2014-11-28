import QtQuick 2.3
import "cassowary-qml.js" as C
import "."

SolverItem {
    property bool when: true
    property var target

    property var strength: Strength.strong
    property var weight: 0

    property var suggested

    impl: when
        && target
        && target.impl
        && new C.c.EditConstraint(target.impl, strength, weight)

    onSuggestedChanged: suggest(suggested)

    property bool __canSuggest: false

    function add(solver, impl) {
        message("Adding edit:", impl)
        solver.addConstraint(impl)
        __canSuggest = true
        if (suggested != null)
            suggest(suggested)
    }

    function remove(solver, impl) {
        message("Removing edit:", impl)
        __canSuggest = false
        solver.removeConstraint(impl)
    }

    function suggest(value) {
        if (__canSuggest) {
            __solver.commit()
            var s = __solverImpl
            message("Suggest", target.impl, value)
            s.suggestValue(target.impl, value)
            s.resolve()
        }
    }
}
