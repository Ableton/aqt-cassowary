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
        Constraint { id: c1; solver: s1 }

        function test_constraints_data() {
            return [
                { expr: c1.eq(v1, 42), value: 42 },
                { expr: c1.eq(v1, c1.plus(42, 2)), value: 44 },
                { expr: c1.eq(v1, c1.minus(42, 2)), value: 40 },
                { expr: c1.eq(v1, c1.divide(42, 2)), value: 21 },
                { expr: c1.eq(v1, c1.times(42, 2)), value: 84 },
                { expr: c1.eq(42, v1), value: 42 },
                { expr: c1.eq(42, c1.plus(v1, 21)), value: 21 },
                { expr: c1.eq(42, c1.minus(v1, 21)), value: 63 },
                { expr: c1.eq(42, c1.divide(v1, 2)), value: 84 },
                { expr: c1.eq(42, c1.times(v1, 2)), value: 21 }
            ]
        }

        function test_constraints(data) {
            c1.expr = data.expr
            compare(v1.value, data.value)
        }
    }
        }
    }
}
