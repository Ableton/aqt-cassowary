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

import ".."

TestScene {
    Component {
        id: sceneComponent

        Item {
            property string tag: "root"
            objectName: "myName"

            Item {
                property string tag: "itemWithoutName"

                Item {
                    property string tag: "nestedItem"
                    objectName: "myName"

                    Item {
                        property string tag: "itemNestedInNestedItem"
                        objectName: "myName"
                    }

                    QtObject {
                        property string tag: "QtObject"
                        objectName: "myName"
                    }
                }

                Item {
                    property string tag: "nestedItemOnSameLevel"
                    objectName: "myName"
                }

                Item {
                    property string tag: "itemWithUniqueName"
                    objectName: "myUniqueName"
                }
            }
        }
    }

    TestCase {
        name: "compareRects"

        function test_compareRects_data() {
            return [
                { tag: "equal rects",
                  a: Qt.rect(1, 2, 3, 4),
                  b: Qt.rect(1, 2, 3, 4),
                  expectedResult: true, },
                { tag: "equal differing by x",
                  a: Qt.rect(1, 2, 3, 4),
                  b: Qt.rect(5, 2, 3, 4),
                  expectedResult: false, },
                { tag: "equal differing by y",
                  a: Qt.rect(1, 2, 3, 4),
                  b: Qt.rect(1, 5, 3, 4),
                  expectedResult: false, },
                { tag: "equal differing by width",
                  a: Qt.rect(1, 2, 3, 4),
                  b: Qt.rect(1, 2, 5, 4),
                  expectedResult: false, },
                { tag: "equal differing by height",
                  a: Qt.rect(1, 2, 3, 4),
                  b: Qt.rect(1, 2, 3, 5),
                  expectedResult: false, },
                { tag: "equal rectlike objects",
                  a: {x: 1, y: 2, width: 3, height: 4},
                  b: {x: 1, y: 2, width: 3, height: 4},
                  expectedResult: true, },
                { tag: "unequal rectlike objects",
                  a: {x: 1, y: 2, width: 3, height: 4},
                  b: {x: 1, y: 2, width: 3, height: 5},
                  expectedResult: false, },
                { tag: "incomplete rectlike objects",
                  a: {x: 1, y: 2, width: 3, height: 0},
                  b: {x: 1, y: 2, width: 3},
                  expectedResult: false, },
                { tag: "equal rect and rectlike object",
                  a: Qt.rect(1, 2, 3, 4),
                  b: {x: 1, y: 2, width: 3, height: 4},
                  expectedResult: true, },
                { tag: "unequal rect and rectlike object",
                  a: Qt.rect(1, 2, 3, 4),
                  b: {x: 1, y: 2, width: 3, height: 5},
                  expectedResult: false, },
            ];
        }

        function test_compareRects(data) {
            compare(TestUtils.compareRects(data.a, data.b), data.expectedResult);
        }
    }

    TestCase {
        name: "findChildren"

        function test_findChildren() {
            TestUtils.withComponent(sceneComponent, null, {}, function(testScene) {
                var result = TestUtils.findChildren(testScene, "myName");
                compare(result.map(function(object) { return object.tag; }),
                    ["nestedItem",
                     "nestedItemOnSameLevel",
                     "QtObject",
                     "itemNestedInNestedItem"]);
            });
        }
    }

    TestCase {
        name: "findUniqueChild"

        function test_returnsFoundChildIfUnique() {
            TestUtils.withComponent(sceneComponent, null, {}, function(testScene) {
                var result = TestUtils.findUniqueChild(testScene, "myUniqueName");

                compare(result.tag, "itemWithUniqueName");
            });
        }


        function test_throwsErrorWhenNoChildFound() {
            TestUtils.withComponent(sceneComponent, null, {}, function(testScene) {
                var result;

                try {
                    result = TestUtils.findUniqueChild(testScene, "anotherName");
                }
                catch (exception) {
                    // This is good.
                }
                finally {
                    compare(result, undefined);
                }
            });
        }

        function test_throwsIfMoreThanOneChildFound() {
            TestUtils.withComponent(sceneComponent, null, {}, function(testScene) {
                var exceptionThrown = null;
                try {
                    var result = TestUtils.findUniqueChild(testScene, "myName");
                }
                catch (exception) {
                    exceptionThrown = exception;
                }

                compare(exceptionThrown.message, "Found more than one result!");
            });
        }
    }

    TestCase {
        name: "with_component should create components and always destroy them"

        function checkFunction(fun) {
            var called = {},
                mockComponentClass,
                mockComponent,
                expectedCalls;

            mockComponent = {
                destroy: function() {
                    called.destroy = true;
                }
            };

            mockComponentClass = {
                createObject: function(parent, args) {
                    called.createObject = true;

                    compare("fake-parent", parent);
                    compare("fake-args", args);

                    return mockComponent;
                }
            };

            expectedCalls = ["test_fn"].concat(
                Object.keys(mockComponent).concat(
                    Object.keys(mockComponentClass)
                )
            ).sort();

            try {
                TestUtils.withComponent(
                    mockComponentClass, "fake-parent", "fake-args",
                    function (obj) {
                        called.test_fn = true;

                        compare(mockComponent, obj);
                        fun(obj);
                    });
            }
            finally {
                compare(
                    Object.keys(called).sort(),
                    expectedCalls
                );
            }
        }

        function test_niceFunction() {
            checkFunction(function() {});
        }

        function test_throwingFunction() {
            var sentinelError = new Error("this is an exception");
            try {
                checkFunction(
                    function() {
                        throw sentinelError;
                    }
                );
            } catch (e) {
                if (e !== sentinelError) {
                    throw e;
                }
            }
        }
    }

    TestCase {
        name: "parsingNotes"

        function test_parsingSomeCommonNotes_data() {
            return [
                { input: "C-2", result: 0 },
                { input: "C3", result: 60 },
                { input: 'A#4', result: 82 },
                { input: "G8", result: 127 },
            ];
        }

        function test_parsingSomeCommonNotes(data) {
            compare(TestUtils.letterKeyToMIDIKey(data.input), data.result);
        }
    }

    TestCase {
        name: "parsingTimeSpecs"

        function test_parsingSomeCommonTimes_data() {
            return [
                { input: "1.1.1", result: 0.0 },
                { input: "-1.1.1", result: -4.0 },
                { input: "-1.4.4", result: -1.0/4.0 },
                { input: "1.2.1", result: 1.0 },
                { input: "1.4.1", result: 3.0 },
                { input: "1.4.4", result: 4.0 - 1.0/4.0 },
                { input: "4.1.1", result: 12.0 },
            ];
        }

        function test_parsingSomeCommonTimes(data) {
            compare(TestUtils.dottedBarQuarterSixteenToTime(data.input), data.result);
        }
    }

    TestCase {
        name: "keyClicks"

        function createMockTestCase() {
            var __recordedKeyClicks = [];

            return {
                keyClick: function(key, modifiers, delay) {
                    __recordedKeyClicks.push([key, modifiers, delay]);
                },

                recordedKeyClicks: function() { return __recordedKeyClicks; }
            }
        }

        function test_keyClicks() {
            var mockTestCase = createMockTestCase();
            TestUtils.keyClicks(mockTestCase, "Live", Qt.ShiftModifier, 23);
            compare(mockTestCase.recordedKeyClicks(), [
                ["L", Qt.ShiftModifier, 23],
                ["i", Qt.ShiftModifier, 23],
                ["v", Qt.ShiftModifier, 23],
                ["e", Qt.ShiftModifier, 23],
            ]);
        }
    }

    TestCase {
        id: signalSpyTestCase
        name: "signalSpy"

        signal sentinel(string message);

        function test_thatItCanBeCreatedUnbound() {
            var spy = TestUtils.createSignalSpy();
            spy.target = signalSpyTestCase;
            spy.signalName = "sentinel";

            sentinel("hello");

            compare(spy.count, 1);
            compare(spy.signalArguments[0][0], "hello");
        }

        function test_thatItKnowsHowToConnectToTargets() {
            var spy = TestUtils.createSignalSpy(signalSpyTestCase, "sentinel");

            sentinel("hello");

            compare(spy.count, 1);
        }

        function test_thatItThrowsWhenConnectingToAnUnknownSignal() {
            try {
                var spy = TestUtils.createSignalSpy(signalSpyTestCase, "misspellt");
                fail();
            } catch (error) {
                compare(/SignalSpy/.test(error.message), true, "exception did not match: " + error);
            }
        }
    }
}
