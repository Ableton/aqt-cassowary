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

    TestCase {
        Solver { id: s1 }
        Variable { id: v1 }
        Constraint { id: c1; extend: s1 }

        function test_constraints_data() {
            return [
                { expr: c1.eq(v1, 42), value: 42 },
                { expr: c1.eq(v1, c1.plus(42, 2)), value: 44 },
                { expr: c1.eq(v1, c1.plus(42, 2, 3)), value: 47 },
                { expr: c1.eq(v1, c1.plus(42, 2, 3, 4)), value: 51 },
                { expr: c1.eq(v1, c1.plus(42, 2, 3, 4, 5)), value: 56 },
                { expr: c1.eq(v1, c1.plus(42, 2, 3, 4, 5, 6)), value: 62 },
                { expr: c1.eq(v1, c1.minus(42, 2)), value: 40 },
                { expr: c1.eq(v1, c1.minus(42, 2, 3)), value: 37 },
                { expr: c1.eq(v1, c1.minus(42, 2, 3, 4)), value: 33 },
                { expr: c1.eq(v1, c1.minus(42, 2, 3, 4, 5)), value: 28 },
                { expr: c1.eq(v1, c1.minus(42, 2, 3, 4, 5, 6)), value: 22 },
                { expr: c1.eq(v1, c1.times(42, 2)), value: 84 },
                { expr: c1.eq(v1, c1.times(42, 2, 3)), value: 252 },
                { expr: c1.eq(v1, c1.times(42, 2, 3, 4)), value: 1008 },
                { expr: c1.eq(v1, c1.times(42, 2, 3, 4, 5)), value: 5040 },
                { expr: c1.eq(v1, c1.times(42, 2, 3, 4, 5, 6)), value: 30240 },
                { expr: c1.eq(v1, c1.divide(30240, 6)), value: 5040 },
                { expr: c1.eq(v1, c1.divide(30240, 6, 5)), value: 1008 },
                { expr: c1.eq(v1, c1.divide(30240, 6, 5, 4)), value: 252 },
                { expr: c1.eq(v1, c1.divide(30240, 6, 5, 4, 3)), value: 84 },
                { expr: c1.eq(v1, c1.divide(30240, 6, 5, 4, 3, 2)), value: 42 },
                { expr: c1.eq(42, v1), value: 42 },
                { expr: c1.eq(42, c1.plus(v1, 21)), value: 21 },
                { expr: c1.eq(42, c1.minus(v1, 21)), value: 63 },
                { expr: c1.eq(42, c1.divide(v1, 2)), value: 84 },
                { expr: c1.eq(42, c1.times(v1, 2)), value: 21 }
            ]
        }

        function test_constraints(data) {
            c1.expr = data.expr
            s1.commit()
            compare(v1.value, data.value)
        }
    }

    TestCase {
        Solver { id: s2 }
        Variable { id: v2 }
        Constraint { id: c2; extend: s2; expr: eq(v2, 42) }
        Constraint { id: c3; extend: s2; expr: eq(v2, 21);
                     strength: Strength.Medium; }

        function test_strengths() {
            c2.when = true
            c2.strength = Strength.Strong
            c2.commit()
            compare(v2.value, 42)
            c2.strength = Strength.Weak
            c2.commit()
            compare(v2.value, 21)
        }

        function test_disable() {
            c2.when = true
            c2.strength = Strength.Strong
            s2.commit()
            compare(v2.value, 42)
            c2.when = false
            s2.commit()
            compare(v2.value, 21)
            c2.when = true
            s2.commit()
            compare(v2.value, 42)
        }
    }

    TestCase {
        Constraint { id: c4 }
        function test_defaults() {
            compare(c4.strength, Strength.Required)
            compare(c4.weight, 1)
        }
    }

    TestCase {
        Solver {
            id: s21
            Variable { id: v4 }
            Constraint { expr: eq(v4, 42) }
        }

        function test_commitsAsSoonAsEventLoopIterates() {
            s21.commit()
            compare(v4.value, 42)
        }
    }

    TestCase {
        Solver {
            id: s22
            Variable { id: v5; initial: 30; Stay {} }
            Constraint { expr: geq(v5, 10) }
            Constraint { expr: leq(v5, 50) }
        }

        function test_usesInitialValue() {
            s21.commit()
            compare(v5.value, 30)
        }

        function test_changingInitialAfterInitializationHasNoEffect() {
            s21.commit()
            compare(v5.value, 30)
        }
    }

    TestCase {
        Solver {
            id: s4
            property bool toggle: true
            Variable { id: v7 }
            Constraint {
                when: s4.toggle
                expr: eq(v7, 21)
            }
            Constraint {
                when: !s4.toggle
                expr: eq(v7, 42)
            }
        }

        function test_mutuallyExclusiveConstraints() {
            s4.commit()
            compare(v7.value, 21)

            s4.toggle = !s4.toggle
            s4.commit()
            compare(v7.value, 42)

            s4.toggle = !s4.toggle
            s4.commit()
            compare(v7.value, 21)
        }
    }

    TestCase {
        Solver {
            id: s5
            Variable { id: v8 }
            Variable { id: v9 }
            Constraint {
                expr: eq(v8, 21)
            }
            Constraint {
                when: v8.value > 0
                expr: eq(v9, 42)
            }
        }

        function test_constraintDependsOnOtherConstraint() {
            s5.commit()
            compare(v8.value, 21)
            compare(v9.value, 42)
        }
    }

}
