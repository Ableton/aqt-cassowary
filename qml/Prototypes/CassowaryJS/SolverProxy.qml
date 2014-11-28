import QtQuick 2.3

SolverItem {
    signal updated

    function defer(event) { __solver.defer(event) }
    function commit() { __solver.commit(); }

    Connections {
        target: __solver
        onUpdated: updated()
    }

    on__SolverChanged: updated()
}
