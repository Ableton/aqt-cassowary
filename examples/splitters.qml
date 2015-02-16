import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import "Splitters"

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

            //orientation: Qt.Horizontal

            ExampleView {
                anchors.fill: parent
                color: "#E099CB"
                text: "Small View"
                focus: true

                property real initialRatio: 0.1
                property real minimumSize: 16
            }

            ExampleView {
                anchors.fill: parent
                color: "#E0AF46"
                text: "View 1"
                property real initialRatio: 0.1
            }

            ExampleView {
                anchors.fill: parent
                color: "#70E0D1"
                text: "View 2"
                property real initialRatio: 0.5
            }

            ExampleView {
                anchors.fill: parent
                color: "#E0AF46"
                text: "View 3"
                property real initialRatio: 0.5
            }

            ExampleView {
                anchors.fill: parent
                color: "#70E0D1"
                text: "View 4"
                property real initialRatio: 0.5
            }

            ExampleView {
                anchors.fill: parent
                color: "#A6DB3B"
                text: "Snapping View"


                property real initialRatio: 0.2
                property var snapTo: [ 42, 71, 251 ]
                property real minimumSize: 42
                property real maximumSize: 251

                Repeater {
                    model: parent.snapTo
                    Rectangle {
                        opacity: 0.2
                        color: "white"
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        height: modelData
                    }
                }
            }
        }
    }
}
