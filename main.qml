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
    title: qsTr("Match3")

    property int firstSelectedItem: -1
    property int secondSelectedItem: -1
    property int countSwapAnimation: 0
    property int countFallingDownAnimation: 0
    property bool gameFinished: false
    property bool shifted: false
    property bool filled: false

    onCountFallingDownAnimationChanged: {
        if (countFallingDownAnimation == 0)
        {
            if (gameFinished)
            {
                winDialog.open();
                root.gameFinished = false;
            }
        }
    }

    onCountSwapAnimationChanged: {
        if (countSwapAnimation == 0)
        {
            if (root.firstSelectedItem != -1 && !boardModel.pop(root.firstSelectedItem, root.secondSelectedItem))
            {
                boardModel.move(firstSelectedItem, secondSelectedItem);
            } else {
                if (root.firstSelectedItem != -1 && !shifted)
                {
                    root.filled = boardModel.shift();
                    root.shifted = true;
                } else {
                    boardModel.fill();
                    root.shifted = false;
                }
            }
            root.firstSelectedItem = -1;
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
        delegate: Tile {
            newIndex: model.index
            fallDown: model.active
            color: model.color

            onMoveAnimationRunningChanged: root.countSwapAnimation += running ? 1 : -1
            onFallAnimationRunningChanged: root.countFallingDownAnimation += running ? 1 : -1
            onClicked: {
                if (root.firstSelectedItem == -1)
                {
                    root.firstSelectedItem = index;
                    blink = true;
                } else {
                    if (boardModel.move(root.firstSelectedItem, index)) {
                        root.secondSelectedItem = index;
                    } else {
                        gridView.itemAtIndex(root.firstSelectedItem).blink = false;
                        root.firstSelectedItem = index;
                        blink = true;
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
        onReset: {
            boardModel.restart();
            root.firstSelectedItem = -1;
            root.secondSelectedItem = -1;
            root.shifted = false;
            root.filled = false;
        }
    }
    Connections {
        target: boardModel

        function onFinished(isWon) {
            root.gameFinished = true;
            if (isWon)
            {
                winDialog.title = "CONGRATS!";
                winDialog.text = "You won! Try again or quit";
            } else {
                winDialog.title = ":(";
                winDialog.text = "Try again or quit";
            }
        }
    }

}

