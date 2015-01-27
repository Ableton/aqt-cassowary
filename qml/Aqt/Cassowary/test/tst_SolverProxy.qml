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
        Solver {
            id: s0
            Variable { id: v1 }
            Constraint { expr: eq(v1, 42) }
        }

        function test_solverCanBeSpecifiedViaParent() {
            s0.commit()
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
            s2.commit()
            compare(v3.value, 42)
        }
    }
}
