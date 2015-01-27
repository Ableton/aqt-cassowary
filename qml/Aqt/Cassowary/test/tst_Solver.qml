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
import QtTest 1.0

import Ableton.TestUtils 1.0
import Aqt.Cassowary 1.0

TestScene {
    id: scene

    Component {
        id: solver
        Solver {}
    }

    TestCase {
        when: windowShown

        function test_instantiation() {
            TestUtils.withComponent(solver, null, {}, function (solver) {
            })
        }

        function test_canDeferJavaScriptCall() {
            TestUtils.withComponent(solver, null, {}, function (solver) {
                var count = 0
                solver.defer(function () {
                    ++count
                })
                compare(count, 0)
                solver.commit()
                compare(count, 1)
            })
        }

        function test_commitsAfterRendering() {
            var solver2 = solver.createObject(scene, {});
            var count = 0
            solver2.defer(function () {
                ++count
            })
            compare(count, 0)
            waitForRendering(solver2)
            compare(count, 1)
            // @todo this causes the thingy to crash.  It is unclear
            // to me whether it is our fault or a Qt bug.
            //   solver2.destroy()
        }

        function test_recursiveCommitProtection() {
            TestUtils.withComponent(solver, null, {}, function (solver) {
                var count1 = 0
                var count2 = 0
                solver.defer(function () {
                    solver.defer(function () {
                        compare(count1, 1)
                        ++count2
                    })
                    solver.commit()
                    ++count1
                })
                solver.commit()
                compare(count1, 1)
                compare(count2, 1)
            })
        }
    }
}
