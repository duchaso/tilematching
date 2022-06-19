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

    property int countSwapAnimation: 0
    property int countFallingDownAnimation: 0
    property bool gameFinished: false

    onCountFallingDownAnimationChanged: {
        if (countFallingDownAnimation == 0)
        {
            if (gameFinished)
            {
                winDialog.open();
                root.gameFinished = false;
            } else {
                boardModel.fallFinished();
            }
        }
    }

    onCountSwapAnimationChanged: {
        if (countSwapAnimation == 0)
        {
            boardModel.swapFinished();
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
            blink: model.selected

            onMoveAnimationRunningChanged: root.countSwapAnimation += running ? 1 : -1
            onFallAnimationRunningChanged: {
                if (running) {
                    root.countFallingDownAnimation += 1;
                } else {
                    root.countFallingDownAnimation -= 1;
                    model.active = false;
                }
            }
            onClicked: boardModel.tileClicked(index);
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

