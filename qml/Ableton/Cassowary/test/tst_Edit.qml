// Copyright: 2014, Ableton AG, Berlin, all rights reserved

import QtQuick 2.3
import QtTest 1.0

import Ableton.Base 1.0
import Ableton.TestUtils 1.0
import Ableton.Cassowary 1.0

TestScene {
    id: scene

    TestCase {
        id: case1
        property var someVar

        Solver {
            Variable {
                id: v1
            }
            Edit {
                id: e1
                target: v1
                suggested: case1.someVar
            }
            Stay {
                target: v1
            }
        }

        function test_suggestingValues() {
            e1.suggest(42)
            compare(v1.value, 42)
            e1.suggest(21)
            compare(v1.value, 21)
        }

        function test_suggestingValuesThroughBinding() {
            case1.someVar = 42
            compare(v1.value, 42)
            case1.someVar = 21
            compare(v1.value, 21)
        }
    }

    TestCase {
        Edit { id: e2 }
        function test_defaults() {
            compare(e2.strength, Strength.Strong)
            compare(e2.weight, 1)
        }
    }
}
