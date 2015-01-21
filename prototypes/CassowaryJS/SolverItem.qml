// Copyright (c) 2014, 2015 Ableton AG, Berlin
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

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
