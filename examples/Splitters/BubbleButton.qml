import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1

Button {
    id: root

    layer.enabled: true

    property bool leftmost: false
    property bool rightmost: false

    property color background: "#333"
    property color foreground: "#aaa"
    property color backgroundH: "#666"
    property color foregroundH: "#eee"
    property color backgroundC: "#fff"
    property color foregroundC: "#333"

    property real radius: 10

    implicitHeight: 20
    implicitWidth: 46 + leftmost * radius + rightmost * radius

    style: ButtonStyle {
        label: Item {
            Text {
                text: control.text
                anchors.fill: parent
                anchors.margins: 5
                font.pixelSize: 12
                color: control.pressed ? root.foregroundC
                    : control.hovered ? root.foregroundH
                    : root.foreground
                horizontalAlignment: leftmost && !rightmost ? Text.AlignRight
                    : rightmost && !leftmost ? Text.AlignLeft
                    : Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
        background: Item {
            anchors.fill: parent
            Rectangle {
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                }
                width: 2 * root.radius
                radius: root.leftmost ? root.radius : 0
                color: control.pressed ? root.backgroundC
                    : control.hovered ? root.backgroundH
                    : root.background
            }
            Rectangle {
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    right: parent.right
                }
                width: 2 * root.radius
                radius: root.rightmost ? root.radius : 0
                color: control.pressed ? root.backgroundC
                    : control.hovered ? root.backgroundH
                    : root.background
            }
            Rectangle {
                anchors.centerIn: parent
                width: parent.width - 2 * root.radius
                height: parent.height
                color: control.pressed ? root.backgroundC
                    : control.hovered ? root.backgroundH
                    : root.background
            }
        }
    }
}
