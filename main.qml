import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import board 1.0

Window {
    id: root

    width: 480
    height: 640
    visible: true
    title: qsTr("Hello World")

    property int it: -1
    property int it2: -1
    property int countSwapAnimation: 0
    property bool shifted: false
    property bool filled: false

    onCountSwapAnimationChanged: {
        if (countSwapAnimation == 0)
        {
            if (root.it != -1 && !boardModel.pop(root.it, root.it2))
            {
                boardModel.move(it, it2);
            } else {
                if (root.it != -1 && !shifted)
                {
                    root.filled = boardModel.shift();
                    root.shifted = true;
                } else {
                    boardModel.fill();
                    root.shifted = false;
                }
            }
            root.it = -1;
        }
    }

    onFilledChanged: {
        if (filled)
        {
            boardModel.fill();
            root.filled = false;
            root.shifted = false;
        }
    }

    GridView {
        id: gridView

        anchors.fill: parent
        cellHeight: root.height / 6 - (root.height - root.width) / 6
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
            radius: 100
            color: model.color

            Component.onCompleted: ready = true;
            onXChanged: blinking.stop();
            onYChanged: blinking.stop();

            onNewIndexChanged: {
                if (ready)
                {
                    x = gridView.itemAtIndex(newIndex).x;
                    y = gridView.itemAtIndex(newIndex).y
                }
            }

            onFallDownChanged: {
                if (ready && fallDown)
                {
                    fallingDown.start();
                }
            }

            NumberAnimation on y {
                id: fallingDown

                from: y - (gridView.cellHeight * (Math.floor(index / 6) + 1))
                to: y
                running: false
                duration: 2000
                alwaysRunToEnd: true
            }

            Behavior on x {
                NumberAnimation {
                    duration: 2000
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
                    duration: 2000
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
                            boardModel.steps += 1;
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

    RowLayout {
        height: root.height - root.width
        width: root.width
        x: 0
        y: root.height - height

        Text {
            id: scoreWidget

            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 30
            text: "Score: " + boardModel.score + "/" + boardModel.scoreToWin
        }
        Text {
            id: stepsWidget

            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 30
            text: "Steps: " + boardModel.steps + "/" + boardModel.stepsToLose
        }
    }

    MessageDialog {
        id: winDialog

        standardButtons: StandardButton.Ok | StandardButton.Reset

        onAccepted: Qt.quit();
        onReset: boardModel.restart();
    }
    Connections {
        target: boardModel
        function onFinished() {
            if (boardModel.isWon)
            {
                winDialog.title = "CONGRATS!";
                winDialog.text = "You won! Try again or quit";
            } else {
                winDialog.title = ":(";
                winDialog.text = "Try again or quit";
            }
            winDialog.open();
        }
    }

}

