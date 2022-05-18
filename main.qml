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
        anchors.fill: parent
        columnWidthProvider: function (column) { return root.width / boardModel.columnCount() }
        rowHeightProvider: function (row) { return root.height / boardModel.rowCount() }
        clip: true

        model: Board {
            id: boardModel
        }

        delegate: Rectangle {
            implicitWidth: 100
            implicitHeight: 100
            radius: 100
            color: colour

            ColorAnimation on color {
                id: blinking

                loops: Animation.Infinite
                running: false
                from: colour
                to: "white"
                duration: 1000
            }

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    blinking.start();
                    boardModel.moveTile(boardModel.index(row, column));
                }
            }
        }
    }
}
