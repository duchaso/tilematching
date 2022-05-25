import QtQuick 2.15
import QtQuick.Window 2.15
import board 1.0

Window {
    id: root

    width: 640
    height: 640
    visible: true
    title: qsTr("Hello World")

    TableView {
        id: tableView

        property int spacing: 2

        anchors.fill: parent
        columnSpacing: spacing
        rowSpacing: spacing
        columnWidthProvider: function (column) { return root.width / boardModel.columnCount() - spacing }
        rowHeightProvider: function (row) { return root.height / boardModel.rowCount() - spacing }
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
                duration: 1000
            }
            NumberAnimation on y {
                id: movingY

                property int shift: height + tableView.spacing
                to: y
                alwaysRunToEnd: true
                running: false
                duration: 1000
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
}

