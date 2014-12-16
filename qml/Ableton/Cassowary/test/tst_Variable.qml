// Copyright: 2014, Ableton AG, Berlin, all rights reserved

import QtQuick 2.3
import QtTest 1.0

import Ableton.Base 1.0
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
            constraint.commit()
            compare(variable2.value, 42)
            compare(spy2.count, 1)
        }
    }
}
