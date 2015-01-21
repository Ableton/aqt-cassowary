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

/**
 * @namespace Ableton.TestUtils
 * @version 1.0
 */

.pragma library

.import Ableton.Base 1.0 as Base

"use strict";

/**
 * @fn bool Ableton.TestUtils.comparePoints(Object a, Object b)
 * @param a an object with at least `x` and `y` members
 * @param b an object with at least `x` and `y` members
 *
 * @returns whether the two points are equal
 */
function comparePoints(a, b) {
    return a.x === b.x && a.y === b.y;
}

/**
 * @fn bool Ableton.TestUtils.compareRects(Object a, Object b)
 * @param a an object with at least `x`, `y`, `width` and `height` members
 * @param b an object with at least `x`, `y`, `width` and `height` members
 *
 * @returns whether the two rects are equal
 */
function compareRects(a, b) {
    return a.x === b.x && a.y === b.y && a.width === b.width && a.height === b.height;
}

/**
 * @fn bool Ableton.TestUtils.isPixelAligned(Number pos)
 *
 * @returns true if @p pos is an integer
 */
function isPixelAligned(pos) {
    return Math.floor(pos) === pos;
}

/**
 * @fn color Ableton.TestUtils.logicalPixelColor(Object image, Number x, Number y, Number retinaFactor)
 * @param image the image to retrieve the pixel
 * @param x the x pixel coordinate to grab
 * @param y the y pixel coordinate to grab
 * @param retinaFactor the factor used to correct coordinates.
 *
 * @throw Error if @p retinaFactor is not greater than 0
 *
 * This function will retrieve the color value of the given
 * pixel in this scene, correcting for retina displays if necessary.
 *
 * @see Ableton.Display.ScreenObserver.devicePixelRatio
 *
 * @returns the color of the requested pixel
 */
function logicalPixelColor(image, x, y, retinaFactor) {
    if (retinaFactor > 0) {
        return image.pixel(x * retinaFactor, y * retinaFactor);
    }
    Base.Errors.throwError("retinaFactor must be > 0");
}

/**
 * @fn Object Ableton.TestUtils.boundingRectInWindowCoords(QtQuick.Item item)
 *
 * @returns an object with `left`, `right`, `top`, `bottom`, and `center` members
 */
function boundingRectInWindowCoords(item) {
    var coords = item.parent.mapToItem(null, item.x, item.y, item.width, item.height);

    coords.left = coords.x;
    coords.right = coords.x + coords.width;
    coords.top = coords.y;
    coords.bottom = coords.y + coords.height;
    coords.center = { x: coords.x + 0.5 * coords.width,
                      y: coords.y + 0.5 * coords.height };

    return coords;
}

/**
 * @fn void Ableton.TestUtils.singleMouseDrag(QtTest.TestCase testcase, QtQuick.Item item, Number x, Number y, Number dx, Number dy, QtQuick.Controls.Button button, int modifiers, Number delay)
 *
 * A replacement for `QtTest.TestCase.mouseDrag()` that does
 * not handle the `dragThreshold`.  `mouseDrag()` sometimes does
 * additional drags for small displacements to get past the
 * `dragThreshold`.
 */
function singleMouseDrag(testcase, item, x, y, dx, dy, button, modifiers, delay) {
    testcase.mousePress(item, x, y, button, modifiers, delay);
    testcase.mouseMove(item, x + dx, y + dy, delay, button);
    testcase.mouseRelease(item, x + dx, y + dy, button, modifiers, delay);
}

/**
 * @fn void Ableton.TestUtils.keyClicks(QtTest.TestCase testcase, String keys, int modifiers, Number delay)
 *
 * Simulates clicking a sequence of `keys` on the currently focused
 * item. Optionally, a keyboard `modifier` can be specified as well as a
 * `delay`. If `delay` is larger than 0 the test will wait for
 * `delay` milliseconds before each key click.
 */

function keyClicks(testcase, keys, modifiers, delay) {
    keys.split("").forEach(function(key) {
        testcase.keyClick(key, modifiers, delay);
    });
}

/**
 * @fn Array Ableton.TestUtils.findChildren(QtObject qtObject, String objectName)
 *
 * @returns an array of QML objects with the given `objectName`
 * that are descendants of `qtObject`
 *
 * This function works differently than Qt's `QObject.findChildren()` since the
 * children of a `QObject` and the children of a `QQuickItem` are different
 * concepts.  Consequently, `TestUtils.findChildren()` can't be implemented in
 * terms of `QObject.findChildren()`.
 */
function findChildren(qtObject, objectName) {
    if (qtObject.data === undefined) {
        return [];
    }
    var descendants = Base.ObjectUtils.values(qtObject.data);
    var result = descendants.filter(function (child) {
        return child.objectName === objectName;
    });
    descendants.forEach(function (child) {
        result = result.concat(findChildren(child, objectName));
    });
    return result;
}

/**
 * @fn Array Ableton.TestUtils.findChildrenWithPatience(QtQuick.Item qmlItem, String objectName, QtTest.TestCase testCase, Number minimum)
 * @param testCase a `TestCase` to use as a timer
 * @param minimum minimum number of children to find
 *
 * Same as findChildren(), but tries several times, waiting in between
 *
 * @returns an array of QML Items with the given `objectName`
 * that are descendants of `qmlItem`
 * @throws Error if not at least `minimum` children are found
 */
function findChildrenWithPatience(qmlItem, objectName, testCase, minimum) {
    var result      = [],
        timeoutLeft = 2000,
        minimum     = (typeof minimum === "undefined" ? 2 : minimum);
    while (result.length < minimum && timeoutLeft > 0) {
        result = findChildren(qmlItem, objectName);
        if (result.length < minimum) {
            testCase.wait(100);
            timeoutLeft -= 100;
        }
    }
    if (result.length < minimum) {
        Base.Errors.throwError(
          "Not enough results found (" + result.length + " instead of " + minimum + ")");
    }
    return result;
}

/**
 * @fn Object Ableton.TestUtils.findUniqueChild(QtQuick.Item qmlItem, String objectName)
 *
 * @returns the descendant of `qmlItem` that uniquely has the given `objectName`
 * @throws Error if the item does not exist, or if more than one item with the given name exists
 */
function findUniqueChild(qmlItem, objectName) {
    return onlyIfExactlyOne(findChildren(qmlItem, objectName));
}

/**
 * @fn Object Ableton.TestUtils.findUniqueChildWithPatience(QtQuick.Item qmlItem, String objectName, QtTest.TestCase testCase)
 * @param testCase a `TestCase` to use as a timer
 *
 * Same as findUniqueChild(), but tries several times,
 * waiting in between (see findChildrenWithPatience())
 */
function findUniqueChildWithPatience(qmlItem, objectName, testCase) {
    return onlyIfExactlyOne(findChildrenWithPatience(qmlItem, objectName, testCase, 1));
}

/**
 * @fn Object Ableton.TestUtils.onlyIfExactlyOne(Array result)
 *
 * @returns the first and only item in the array
 * @throws Error if there is not exactly one item in the array
 */
function onlyIfExactlyOne(result) {
    if (result.length > 1) {
        Base.Errors.throwError("Found more than one result!");
    }
    if (result.length === 0) {
        Base.Errors.throwError("Found zero results!");
    }
    return result[0];
}

/**
 * @fn whateverProcReturns Ableton.TestUtils.withComponent(QtQml.Component componentClass, QtQuick.Item parent, Object args, Function proc)
 * Instantiates @p componentClass and calls @p proc with the instantiated object as the sole parameter.
 *
 * Ensures proper lifetime management, i.e. the instantiated object is destroyed when
 * @p proc returns. Use this rather than manually creating Qt components for testing.
 *
 * @par Example
 * @code{.js}
 *   withComponent(mockNoteEditor, testScene, {}, function (editor) {
 *       var testResult = testUsing(editor);
 *       compare(testResult, expected);
 *   });
 * @endcode
 */
function withComponent(componentClass, parent, args, proc) {
    var obj = componentClass.createObject(parent, args);
    if (obj == null) {
        Base.Errors.throwError('Could not create object of class ' + componentClass);
    }

    try {
        return proc(obj);
    }
    finally {
        obj.destroy();
    }
}


/**
 * @fn void withConnection(signal sig, Function cb, Function body)
 * @param sig A signal
 * @param cb A callback to be connected to the signal
 * @param body A function to be executed while the connection is set
 *
 * Wraps a function call to `body` in a scope
 * where `cb` is connected to the signal.  It makes sure the
 * connection is cleaned up on disconnection.
 *
 * @par Example
 * @code{.js}
 *   var wasEmitted = false;
 *   withConnection(
 *       root.someSignal,
 *       function (msg) {
 *           wasEmitted = true;
 *       },
 *       function () {
 *           root.someSignal();
 *           verify(wasEmitted);
 *       })
 * @endcode
 */
function withConnection(sig, cb, body) {
    sig.connect(cb);
    try {
        body();
    } finally {
        sig.disconnect(cb);
    }
}

/**
 * @fn bool Ableton.TestUtils.becomesTrue(Function predicate, TestCase testCase, Number timeout, Number waitLength)
 * @param predicate a function which should return true
 * @param testCase the test case on which to wait
 * @param timeout in ms (defaults to 500)
 * @param waitLength how long to wait (in ms) between attempts (defaults to 10)
 *
 * @return whether the given `predicate` becomes true within a maximum of `timeout` milliseconds
 */
function becomesTrue(predicate, testCase, timeout, waitLength) {
    timeout = timeout === undefined ? 500 : timeout;
    waitLength = waitLength === undefined ? 10 : waitLength
    for ( ; timeout > 0; timeout -= waitLength) {
        if (predicate() === true) {
            return true;
        }
        testCase.wait(waitLength);
    }
    return false;
}

/**
 * @fn SignalSpy Ableton.TestUtils.createSignalSpy(Object target, String signalName)
 * @param target (may be undefined) the object which will emit the signal to spy
 * @param signalName (must be defined if target is) the signal to spy
 *
 * Instantiates a QtTest.SignalSpy for testing signal emission.
 *
 * @throw Error if @p target and @p signalName are defined
 * and the signal could not be connected to.
 *
 * @par Example:
 * @code{.js}
 *   var spy = TestUtils.createSignalSpy(testedObject, "someSignal");
 *   testedObject.doSomethingThatEmitsSomeSignal();
 *   compare(spy.count, 1);
 * @endcode
 * @par or if you want to connect manually:
 * @code{.js}
 *   var spy = TestUtils.createSignalSpy();
 *   spy.target = testedObject;
 *   spy.signalName = "someSignal";
 *   verify(spy.valid);
 *
 *   testedObject.doSomethingThatEmitsSomeSignal();
 *   compare(spy.count, 1);
 * @endcode
 */
function createSignalSpy(target, signalName) {
    var signalSpyComponent = Qt.createComponent('SignalSpy.qml'),
        spy = signalSpyComponent.createObject();

    if (null === spy) {
        Base.Errors.throwError(
            "could not instantiate SignalSpy component for target: "
                + target + ", signal: " + signalName);
    }

    if (target) {
        spy.target = target;
        spy.signalName = signalName;

        if (!spy.valid) {
            Base.Errors.throwError(
                "could not connect SignalSpy to " + target + "." + signalName);
        }
    }

    return spy;
}

/**
 * @fn void Ableton.TestUtils.unstable(QtTest testCase, String testName, String addMessage)
 * Declare a test case as unstable and skip it when running tests.
 *
 * @param testCase a test case that is being declared unstable
 * @param testName the name of the test
 * @param addMessage an additional message to display regarding this test case
 *
 * @par Example
 * @code{.js}
 *   function test_myTimerStops() {
 *       TestUtils.unstable(root, "test_myTimerStops()", "Timeout seems to be unreliable?");
 *   }
 * @endcode
 */
function unstable(testCase, testName, addMessage) {
    var msg = addMessage === undefined ? "This test is declared to be unstable" : addMessage;
    testCase.skip("UNSTABLE:" + testCase.name + "::" + testName + ": " + msg);
}

/**
 * @fn Number Ableton.TestUtils.letterKeyToMIDIKey(String keyString)
 * @param keyString a string representing a note in common notation: C-4, D#3 etc..
 *
 * @returns the numerical MIDI key corresponding to this note
 */
function letterKeyToMIDIKey(keyString) {
    var re = /^([A-G]#?)(-?)([0-9])$/,
    match = re.exec(keyString);

    if (match === null) {
        Base.Errors.throwError("invalid key spec: " + keyString + ", must match: " + re.source);
    }

    var offsets = {
        "C": 0, "C#": 1,
        "D": 2, "D#": 3,
        "E": 4,
        "F": 5, "F#": 6,
        "G": 7, "G#": 8,
        "A": 9, "A#": 10,
        "B": 11,
    };

    var octave = 2 + (match[2] ? -1 : +1) * parseInt(match[3]),
    semitoneOffset = offsets[match[1]];

    return 12 * octave + semitoneOffset;
}

/**
 * @fn Number Ableton.TestUtils.dottedBarQuarterSixteenToTime(String dottedBarQuarterSixteen)
 * @param dottedBarQuarterSixteen spec for a time as <bar>.<quarter>.<sixteen>
 *
 * @returns the floating point time corresponding to the spec
 */
function dottedBarQuarterSixteenToTime(dottedBarQuarterSixteen) {
    var input = dottedBarQuarterSixteen,
        array = input.split('.'),
        validateQ = function (value, type) {
            if (value < 1 || value > 4) {
                Base.Errors.throwError("invalid " + type + ": " + quarter + " in " + input);
            }
            return value;
        },
        validateBar = function (value) {
            if (value === 0) {
                Base.Errors.throwError("invalid bar " + value + " in " + input);
            }

            return value;
        },
        bar = validateBar(parseInt(array[0]), 'bar'),
        quarter = validateQ(parseInt(array[1]), 'quarter'),
        sixteen = validateQ(parseInt(array[2]), 'sixteen');

    return 4.0 * (bar > 0 ? bar - 1 : bar) + 1.0 * (quarter - 1) + 1/4.0 * (sixteen - 1);
}
