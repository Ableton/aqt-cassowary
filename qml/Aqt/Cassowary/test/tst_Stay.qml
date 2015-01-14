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
import Aqt.Cassowary 1.0

TestScene {
    id: scene

    TestCase {
        Solver { id: s1 }
        Variable {
            id: v1
        }
        Stay {
            id: st1; extend: s1
            target: v1; strength: Strength.Strong
        }
        Constraint {
            id: c1; extend: s1
            expr: eq(v1, 42); strength: Strength.Weak
        }

        function test_variableStay() {
            c1.commit()
            compare(v1.value, 0)
            c1.expr = c1.eq(v1, 21)
            c1.commit()
            compare(v1.value, 0)
        }
    }

    TestCase {
        Stay { id: s2 }
        function test_defaults() {
            compare(s2.strength, Strength.Weak)
            compare(s2.weight, 1)
        }
    }

    TestCase {
        Solver {
            Variable {
                id: v2
                initial: 42
                Stay {}
            }
            Constraint { expr: leq(v2, 100) }
        }

        function test_canFindTargetFromParent() {
            v2.commit()
            compare(v2.value, 42)
        }
    }
}
