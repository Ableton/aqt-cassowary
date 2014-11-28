import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

ApplicationWindow {
    id: window
    title: "Alternative splitter test"
    visible: true
    height: 600
    width: 800

    contentItem {
        minimumWidth: minimumHeight
        minimumHeight: splitView.minimumHeight * splitView.views.length
    }

    Rectangle {
        color: "#000"
        anchors.fill: parent

        MultiSplitView {
            id: splitView
            anchors.fill: parent
            orientation: Qt.Horizontal

            ExampleView {
                anchors.fill: parent
                color: "#E099CB"
                text: "Session"
                focus: true

                property real minimumSize: 16
            }

            ExampleView {
                anchors.fill: parent
                color: "#E0AF46"
                text: "Mixer"
            }

            ExampleView {
                anchors.fill: parent
                color: "#70E0D1"
                text: "Detail"
            }

            ExampleView {
                anchors.fill: parent
                color: "#E0AF46"
                text: "Mixer 2"
            }

            ExampleView {
                anchors.fill: parent
                color: "#70E0D1"
                text: "Detail 2"
            }

            ExampleView {
                anchors.fill: parent
                color: "#A6DB3B"
                text: "Chain"

                property var snapTo: [ 42, 71, 251 ]
                property real minimumSize: 42
                property real maximumSize: 251
            }
        }
    }
}
