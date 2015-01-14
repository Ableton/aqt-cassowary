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
import QtTest 1.0

import Ableton.TestUtils 1.0
import Ableton.Cassowary 1.0

TestScene {
    id: scene

    TestCase {
        Variable {
            id: variable
        }

        SignalSpy {
            id: spy
            target: variable
            signalName: "valueChanged"
        }

        function test_setInitial() {
            compare(spy.count, 0)
            variable.initial = 10
            compare(variable.value, 10)
            compare(variable.initial, 10)
            compare(spy.count, 1)
        }
    }

    TestCase {
        Solver {
            id: s1
            Variable {
                id: variable2
            }
            Constraint {
                id: constraint
            }
        }

        SignalSpy {
            id: spy2
            target: variable2
            signalName: "valueChanged"
        }

        function test_notifiesOnResolvedChange() {
            compare(spy2.count, 0)
            constraint.expr = constraint.eq(variable2, 42)
            s1.commit()
            compare(variable2.value, 42)
            compare(spy2.count, 1)
        }
    }
}
