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
