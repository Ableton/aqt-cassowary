import QtQuick 2.3

FocusScope {
    id: root

    property alias color: theRectangle.color
    property alias text: theText.text

    Rectangle {
        id: theRectangle
        color: "#E099CB"
        anchors.fill: parent
    }

    Text {
        id: theText
        anchors.centerIn: parent
        text: "An example view"
        color: "#fff"
        opacity: 0.6
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            root.focus = true
        }
    }
}
