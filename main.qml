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
        }
    }
}
