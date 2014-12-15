// Copyright: 2014, Ableton AG, Berlin, all rights reserved

import QtQuick 2.3
import QtTest 1.0

import Ableton.Base 1.0
import Ableton.TestUtils 1.0
import Ableton.Cassowary 1.0

TestScene {
    id: scene

    TestCase {
        Solver {
            Variable {
                id: v1
            }
            Edit {
                id: e1
                target: v1
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
    }

    TestCase {
        Edit { id: e2 }
        function test_defaults() {
            compare(e2.strength, Strength.Strong)
            compare(e2.weight, 1)
        }
    }
}
