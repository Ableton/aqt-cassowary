import QtQuick 2.3
import QtQuick.Layouts 1.1

Item {
    id: root

    signal full
    signal above
    signal below

    property bool enabled: false
    property real spacing: 10

    implicitWidth: content.width + spacing * 2
    implicitHeight: content.height + spacing * 2

    MouseArea {
        x: root.spacing
        y: root.spacing
        implicitWidth: content.width + 1
        implicitHeight: content.height + 1
        hoverEnabled: true

        RowLayout {
            opacity: root.enabled ? 1 : 0
            visible: opacity > 0

            Behavior on opacity {
                NumberAnimation {
                    easing.type: Easing.InOutSine
                    duration: 500
                }
            }

            id: content
            spacing: 4

            BubbleButton {
                text: "Full"
                onClicked: full()
                leftmost: true
                rightmost: true
            }
        }
    }
}
