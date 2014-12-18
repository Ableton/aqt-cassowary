// Copyright: 2014, Ableton AG, Berlin, all rights reserved

import QtQuick 2.3
import QtTest 1.0

import Ableton.Base 1.0
import Ableton.TestUtils 1.0
import Ableton.Cassowary 1.0

TestScene {
    id: scene

    TestCase {
        Solver { id: s1 }
        Variable {
            id: v1
        }
        Stay {
            id: st1; solver: s1
            target: v1; strength: Strength.Strong
        }
        Constraint {
            id: c1; solver: s1
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
