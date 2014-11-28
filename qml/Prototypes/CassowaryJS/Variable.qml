import QtQuick 2.3
import "cassowary-qml.js" as C
import "."

SolverItem {
    id: root

    property bool stay: false
    property real initial
    property real value: initial

    impl: new C.c.Variable({ value: initial })

    function edit(value, strength, weight) {
        // strength and weight are optional
        message("Editing:", impl, value, strength, weight)
        if (__solver) {
            __solver.commit()
            var s = __solverImpl
            var v = impl
            var e = new C.c.EditConstraint(v, strength, weight)
            s.addConstraint(e)
                .suggestValue(v, value)
                .resolve()
            s.removeConstraint(e)
        }
        else
            initial = value
    }

    function update() {
        if (impl)
            value = impl.value
    }

    Connections {
        target: __solver
        onUpdated: update()
    }

    on__SolverChanged: update()
}
