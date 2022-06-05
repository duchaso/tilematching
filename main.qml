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

    TableView {
        id: tableView

        property int spacing: 2

        anchors.fill: parent
        columnSpacing: spacing
        rowSpacing: spacing
        columnWidthProvider: function (column) { return root.width / boardModel.columnCount() - spacing }
        rowHeightProvider: function (row) { return root.height / boardModel.rowCount() - spacing - 25}
        clip: true
        interactive: false

        model: Board {
            id: boardModel
        }

        delegate: Rectangle {
            implicitWidth: 100
            implicitHeight: 100
            radius: 100
            color: colour


            NumberAnimation on x {
                id: movingX

                property int shift: width + tableView.spacing
                to: x
                alwaysRunToEnd: true
                running: false
                duration: 1500
                onFinished: boardModel.update();
            }
            NumberAnimation on y {
                id: movingY

                property int shift: height + tableView.spacing
                to: y
                alwaysRunToEnd: true
                running: false
                duration: 1500
                onFinished: boardModel.update();
            }


            function move(inx, dir) {
                if (index === inx) {
                    switch (dir) {
                    case 0:
                        movingY.from = y + (-movingY.shift);
                        movingY.start();
                        break;
                    case 1:
                        movingX.from = x + movingX.shift;
                        movingX.start();
                        break;
                    case 2:
                        movingY.from = y + movingY.shift;
                        movingY.start();
                        break;
                    case 3:
                        movingX.from = x + (-movingX.shift);
                        movingX.start();
                        break;
                    }
                }
            }

            Connections {
                target: boardModel
                function onStartAnimation(inx, dir) {
                    move(inx, dir);
                }
            }

            ColorAnimation on color {
                id: blinking

                loops: Animation.Infinite
                running: active
                from: colour
                to: "pink"
                duration: 1500
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    boardModel.moveTile(boardModel.index(row, column));
                }
            }
        }
    }

    RowLayout {
        height: 150
        width: root.width
        x: 0
        y: root.height - height

        Text {
            id: scoreWidget

            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 50
            text: "Score: " + boardModel.score + "/" + boardModel.scoreToWin
        }
        Text {
            id: stepsWidget

            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 50
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

