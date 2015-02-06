import QtQuick 2.3
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import QtQuick.Layouts 1.0
import Aqt.Cassowary 1.0

ApplicationWindow {
    id: root
    width: 800
    height: 600

    property int numSquares: 2
    property bool push: checkBox.checked

    Solver {
        id: solver
        debug: true
    }

    Rectangle {
        color: "#000"
        anchors.fill: parent
    }

    Rectangle {
        x: 10
        y: 10
        radius: 6
        color: "#ccc"
        width: childrenRect.width + 20
        height: childrenRect.height + 20
        ColumnLayout {
            x: 10
            y: 10
            RowLayout {
                Text {
                    text: "Number of squares:"
                }
                Button {
                    implicitWidth: 20
                    text: "-"
                    onClicked: root.numSquares = Math.max(1, root.numSquares - 1)
                }
                Text {
                    Layout.minimumWidth: 16
                    text: root.numSquares
                }
                Button {
                    implicitWidth: 20
                    text: "+"
                    onClicked: root.numSquares = root.numSquares + 1
                }
            }
            CheckBox {
                id: checkBox
                text: "Squares push eachother"
            }
        }
    }

    Repeater {
        id: outer
        model: root.numSquares
        property var items: getItems(outer)

        Item {
            id: wrapper

            property alias vX: vX
            property alias vY: vY
            property alias r: r

            Solver {
                extend: solver
                Variable {
                    id: vX
                    initial: Math.random() * root.width
                    Stay { strength: !eX.when ? Strength.Strong : Strength.Weak }
                    Edit {
                        id: eX
                        weight: 2
                        strength: Strength.Weak
                        when: !root.push || mouse.drag.active
                        Binding on suggested {
                            value: dragger.x
                            when: mouse.drag.active
                        }
                    }
                }
                Variable {
                    id: vY
                    initial: Math.random() * root.height
                    Stay { strength: !eY.when ? Strength.Strong : Strength.Weak }
                    Edit {
                        id: eY
                        weight: 2
                        strength: Strength.Weak
                        when: !root.push || mouse.drag.active
                        Binding on suggested {
                            value: dragger.y
                            when: mouse.drag.active
                        }
                    }
                }

                Constraint { expr: geq(vX, 0) }
                Constraint { expr: geq(vY, 0) }
                Constraint {
                    expr: leq(plus(vX, r.width), root.width)
                }
                Constraint {
                    expr: leq(plus(vY, r.height), root.height)
                }
            }


            Repeater {
                model: outer.items.filter(function (x) { return x !== wrapper })
                Solver {
                    extend: solver
                    Constraint {
                        id: c1
                        when: r.x < modelData.r.x && Math.abs(r.y - modelData.r.y) < r.height+2
                        expr: leq(plus(vX, r.width), modelData.vX)
                    }
                    Constraint {
                        when: r.y < modelData.r.y && Math.abs(r.x - modelData.r.x) < r.width+2
                        expr: leq(plus(vY, r.height), modelData.vY)
                    }
                }
            }

            Rectangle {
                id: r
                width: 40
                height: 40
                x: vX.value
                y: vY.value
                color: mouse.drag.active ? "#d64937" : "#3d3d3d"
                radius: 4

                MouseArea {
                    id: mouse
                    anchors.fill: parent
                    drag {
                        target: dragger
                    }
                }
            }

            Item {
                id: dragger
                Binding on x {
                    when: !mouse.drag.active
                    value: r.x
                }
                Binding on y {
                    when: !mouse.drag.active
                    value: r.y
                }
            }
        }
    }

    function trace(msg, x) {
        console.log(msg, x)
        return x
    }

    function getItems(items) {
        var res = []
        forEachItem(items, function (x) {
            res.push(x);
        })
        return res
    }

    function forEachItem(items, fn) {
        var count = items.count
        for (var i = 0; i < count; ++i) {
            fn(items.itemAt(i), i)
        }
    }
}
