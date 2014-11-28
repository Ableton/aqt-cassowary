pragma Singleton

import QtQuick 2.3
import "cassowary-qml.js" as C

QtObject {
    readonly property var weak: C.c.Strength.weak
    readonly property var medium: C.c.Strength.medium
    readonly property var strong: C.c.Strength.strong
    readonly property var required: C.c.Strength.required
}
