import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import board 1.0

Window {
    id: root

    width: 640
    height: 790
    visible: true
    title: qsTr("Hello World")

    property int it: -1
    property int it2: -1
    property int countSwapAnimation: 0
    property bool shifted: false

    onCountSwapAnimationChanged: {
        if (countSwapAnimation == 0)
        {
            if (!boardModel.pop() && root.it != -1)
            {
                [gridView.itemAtIndex(root.it2).x, gridView.itemAtIndex(root.it).x] = [gridView.itemAtIndex(root.it).x, gridView.itemAtIndex(root.it2).x];
                [gridView.itemAtIndex(root.it2).y, gridView.itemAtIndex(root.it).y] = [gridView.itemAtIndex(root.it).y, gridView.itemAtIndex(root.it2).y];
                boardModel.move(it, it2);
            } else {
                if (!shifted)
                {
                    boardModel.shift();
                    root.shifted = true;
                } else {
                    boardModel.fill();
                    root.shifted = false;
                }
            }

            root.it = -1;
        }
    }

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
            property alias blink: blinking.running
            property int newIndex: model.index
            property bool fallDown: model.active
            height: gridView.cellHeight
            width: gridView.cellWidth
            color: model.color

            Text {
                anchors.centerIn: parent
                text: model.index
            }

            Component.onCompleted: {
                ready = true;
            }

            onNewIndexChanged: {
                if (ready)
                {
                    x = gridView.itemAtIndex(newIndex).x;
                    y = gridView.itemAtIndex(newIndex).y
                }
            }

            onXChanged: {
                blinking.stop();
            }
            onYChanged: {
                blinking.stop();
            }

            onFallDownChanged: {
                if (ready && fallDown)
                {
                    fallingDown.start();
                }
            }

            NumberAnimation on y {
                id: fallingDown

                from: y - (height * (index % 6))
                to: y
                running: false
                duration: 1500
                alwaysRunToEnd: true
            }

            Behavior on x {
                NumberAnimation {
                    duration: 1500
                    onRunningChanged: {
                        if (running)
                        {
                            root.countSwapAnimation += 1;
                        } else {
                            root.countSwapAnimation -= 1;
                        }
                    }
                }
            }
            Behavior on y {
                NumberAnimation {
                    duration: 1500
                    onRunningChanged: {
                        if (running)
                        {
                            root.countSwapAnimation += 1;
                        } else {
                            root.countSwapAnimation -= 1;
                        }
                    }
                }
            }
            Behavior on color {
                ColorAnimation { duration: 1000 }
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
                        if (boardModel.move(root.it, index)) {
                            root.it2 = index;
                        } else {
                            gridView.itemAtIndex(root.it).blink = false;
                            root.it = index;
                            blinking.start();
                        }
                    }
                }
            }
        }
    }
}

