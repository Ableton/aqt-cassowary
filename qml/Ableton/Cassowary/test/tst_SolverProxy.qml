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
            Variable { id: v1 }
            Constraint { expr: eq(v1, 42) }
        }

        function test_solverCanBeSpecifiedViaParent() {
            compare(v1.value, 42)
        }
    }

    TestCase {
        Solver {
            id: s1
        }

        Solver {
            extend: s1
            Variable { id: v2 }
            Constraint { expr: eq(v2, 42) }
        }

        function test_solverCanBeExtendedViaProxy() {
            s1.commit()
            compare(v2.value, 42)
        }
    }

    TestCase {
        Solver {
            id: s2
        }
        Solver {
            id: s2p
            extend: s2
            Stay {
                Variable {
                    id: v3
                    Constraint {
                        expr: eq(v3, 42)
                    }
                }
                target: v3
            }
        }

        function test_solverCanBeInheritedFromParentArbitrarily() {
            wait(0)
            compare(v2.value, 42)
        }
    }
}
