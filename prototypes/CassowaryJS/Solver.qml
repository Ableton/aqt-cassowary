// Copyright: 2014, Ableton AG, Berlin, all rights reserved
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy,
// modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

import QtQuick 2.3
import "cassowary-qml.js" as C

Item {
    id: root

    property var __solverImpl
    property var __eventQueue
    property bool __committing: false

    property bool debug: false
    property bool trace: false
    property bool verbose: false
    property bool traceAdded: false
    property bool traceGC: false

    onDebugChanged: C.c.debug = debug
    onTraceChanged: C.c.trace = trace
    onVerboseChanged: C.c.verbose = verbose
    onTraceAddedChanged: C.c.debug = traceAdded
    onTraceGCChanged: C.c.GC = traceGC

    signal updated

    Timer {
        id: eventDispatcher
        repeat: false
        interval: 0
        onTriggered: commit()
    }

    Component.onCompleted: {
        debugChanged()
        traceChanged()
        verboseChanged()
        traceAddedChanged()
        traceGCChanged()
        initialized()
    }

    function initialized() {
        if (__solverImpl == null) {
            message("Creating solver")
            var solver = new C.c.SimplexSolver()
            solver.autoSolve = false
            solver.onsolved = function () {
                root.updated()
            }
            __solverImpl = solver
        }
    }

    function v(x) {
        return x && x.impl || x
    }

    function edit(pairs, strength, weight) {
        // strength and weight are optional
        message("Editing:", pairs, strength, weight)
        commit()
        var s = __solverImpl
        var constraints = pairs.map(function (p) {
            if (p) {
                var c = new C.c.EditConstraint(v(p[0]), strength, weight)
                s.addConstraint(c)
                return c
            }
        })
        pairs.forEach(function (p) {
            if (p) s.suggestValue(v(p[0]), p[1])
        })
        s.resolve()
        constraints.forEach(function (c) {
            if (c) s.removeConstraint(c)
        })
    }

    function message() {
        if (debug) {
            var arr = new Array("[Ableton.Cassowary]")
            arr.push.apply(arr, arguments)
            console.log.apply(console, arr)
        }
    }

    function defer(event) {
        if (!__eventQueue) {
            __eventQueue = []
            eventDispatcher.start()
        }
        __eventQueue.push(event)
    }

    function commit() {
        if (!__committing) {
            initialized()
            try {
                __committing = true
                var needsSolve
                while (__eventQueue) {
                    message("Processing events")
                    var currentQueue = __eventQueue
                    __eventQueue = null
                    currentQueue.forEach(function (event) {
                        event()
                        needsSolve = true
                    })
                }
                if (needsSolve)
                    __solverImpl.solve()
            } finally {
                __committing = false
            }
        }
    }

    function solve() {
        __solverImpl.solve()
    }
}
