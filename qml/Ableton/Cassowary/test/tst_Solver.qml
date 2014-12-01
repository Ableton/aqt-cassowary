// Copyright: 2014, Ableton AG, Berlin, all rights reserved

import QtQuick 2.3
import QtTest 1.0

import Ableton.Base 1.0
import Ableton.TestUtils 1.0
import Ableton.Cassowary 1.0

TestScene {
    id: scene

    Component {
        id: solver
        Solver {}
    }

    TestCase {
        function test_instantiation() {
            TestUtils.withComponent(solver, null, {}, function (solver) {
            })
        }
    }
}
