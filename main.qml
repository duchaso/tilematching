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
        property int spacing: 2

        anchors.fill: parent
        columnSpacing: spacing
        rowSpacing: spacing
        columnWidthProvider: function (column) { return root.width / boardModel.columnCount() - spacing }
        rowHeightProvider: function (row) { return root.height / boardModel.rowCount() - spacing }
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
                running: active
                from: colour
                to: "white"
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
