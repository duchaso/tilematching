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
    property int it2: -1
    property int countSwapAnimation: 0

    onCountSwapAnimationChanged: {
        if (countSwapAnimation == 0)
        {
            if (!boardModel.pop() && root.it != -1)
            {
                [gridView.itemAtIndex(root.it2).x, gridView.itemAtIndex(root.it).x] = [gridView.itemAtIndex(root.it).x, gridView.itemAtIndex(root.it2).x];
                [gridView.itemAtIndex(root.it2).y, gridView.itemAtIndex(root.it).y] = [gridView.itemAtIndex(root.it).y, gridView.itemAtIndex(root.it2).y];
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
//            property alias fromX: movingX.from
//            property alias fromY: movingY.from
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

//            NumberAnimation on x {
//                id: movingX

//                to: x
//                alwaysRunToEnd: true
//                running: false
//                duration: 1500
//                onFromChanged: {
//                    blinking.stop();
//                    movingX.start();
//                }
//                onStarted: {
//                    root.countSwapAnimation += 1;
//                }
//                onFinished: {
//                    root.countSwapAnimation -= 1;
//                }
//            }
//            NumberAnimation on y {
//                id: movingY

//                to: y
//                alwaysRunToEnd: true
//                running: false
//                duration: 1500
//                onFromChanged: {
//                    blinking.stop();
//                    movingY.start();
//                }
//                onStarted: {
//                    root.countSwapAnimation += 1;
//                }
//                onFinished: {
//                    root.countSwapAnimation -= 1;
//                }
//            }
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
//            Behavior on color {
//                ColorAnimation { duration: 1000 }
//            }

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
                            [parent.x, gridView.itemAtIndex(root.it).x] = [gridView.itemAtIndex(root.it).x, parent.x];
                            [parent.y, gridView.itemAtIndex(root.it).y] = [gridView.itemAtIndex(root.it).y, parent.y];
//                            gridView.itemAtIndex(root.it).fromX = gridView.itemAtIndex(root.it2).x;
//                            gridView.itemAtIndex(root.it).fromY = gridView.itemAtIndex(root.it2).y;
//                            gridView.itemAtIndex(root.it2).fromX = gridView.itemAtIndex(root.it).x;
//                            gridView.itemAtIndex(root.it2).fromY = gridView.itemAtIndex(root.it).y;
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

