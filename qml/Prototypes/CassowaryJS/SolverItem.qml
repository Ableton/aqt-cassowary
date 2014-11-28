import QtQuick 2.3

Item {
    property Solver solver
    property var impl

    property var __solver: {
        if (solver) return solver
        if (parent && parent.__solverImpl) return parent
        return null
    }
    property var __solverImpl: __solver && __solver.__solverImpl
    property var __oldSolverImpl
    property var __currentImpl

    function add(solver, impl) {}
    function remove(solver, impl) {}

    function message() {
            __solver && __solver.message.apply(solver, arguments)
    }

    onImplChanged: {
        var curr = __currentImpl
        var next = impl
        var si = __solverImpl
        if (curr != next) {
            if (curr) {
                console.assert(si)
                __solver.defer(function () { remove(si, curr) })
                __currentImpl = null
            }
            if (__solverImpl && next) {
                __solver.defer(function () { add(si, next) })
                __currentImpl = next
            }
        }
    }

    on__SolverImplChanged: {
        var curr = __currentImpl
        var next = impl
        var oldSi = __oldSolverImpl
        var si = __solverImpl
        if (oldSi && curr) {
            __solver.defer(function () { remove(oldSi, curr) })
            __currentImpl = null
        }
        if (si && next) {
            __solver.defer(function () { add(si, next) })
            __currentImpl = next
        }
        __oldSolverImpl = si
    }

    Component.onCompleted: __solverImplChanged()
}
