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

        function test_setValue() {
            compare(spy.count, 0)
            variable.value = 10
            compare(variable.value, 10)
            compare(spy.count, 1)
        }
    }
}
