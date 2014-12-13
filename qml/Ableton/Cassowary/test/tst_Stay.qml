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
        Variable { id: v1 }
        Stay {
            id: st1; solver: s1;
            target: v1; strength: Strength.Strong
        }
        Constraint {
            id: c1; solver: s1;
            expr: eq(v1, 42); strength: Strength.Weak
        }

        function test_variableStay() {
            compare(v1.value, 42)
            c1.expr = c1.eq(v1, 21)
            compare(v1.value, 42)
        }
    }

    TestCase {
        Stay { id: s2 }
        function test_defaults() {
            compare(s2.strength, Strength.Weak)
            compare(s2.weight, 1)
        }
    }
}
