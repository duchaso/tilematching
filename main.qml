import QtQuick 2.15
import QtQuick.Window 2.15
import board 1.0

Window {
    id: root

    width: 640
    height: 640
    visible: true
    title: qsTr("Hello World")

    property int it: -1

    GridView {
        id: gridView

        anchors.fill: parent
        cellHeight: root.height / 6
        cellWidth: root.width / 6

        model: Board {
            id: boardModel
        }

        delegate: Rectangle {
            property bool ready: false
//            property alias blink: blinking.running
            height: gridView.cellHeight
            width: gridView.cellWidth
            color: model.color

            Text {
                anchors.centerIn: parent
                text: index
            }

            Component.onCompleted: {
                ready = true;
            }

            onXChanged: {
                blinking.stop();
            }
            onYChanged: {
                blinking.stop();
            }

            Behavior on x {
                NumberAnimation { duration: 1000 }
            }
            Behavior on y {
                NumberAnimation { duration: 1000 }
            }

            ColorAnimation on color {
                id: blinking

                running: false
                from: "white"
                to: color
                duration: 2000
                alwaysRunToEnd: true
                loops: Animation.Infinite
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (root.it == -1)
                    {
                        root.it = index;
                        blinking.start();
                    } else {
                        if (boardModel.move(it, index)) {
                            [parent.x, gridView.itemAtIndex(root.it).x] = [gridView.itemAtIndex(root.it).x, parent.x];
                            [parent.y, gridView.itemAtIndex(root.it).y] = [gridView.itemAtIndex(root.it).y, parent.y];
                        }
                        root.it = -1;
                    }
                }
            }
        }
    }
}


