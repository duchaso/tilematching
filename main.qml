import QtQuick 2.15
import QtQuick.Window 2.15
import board 1.0

Window {
    id: root

    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    TableView {
        anchors.fill: parent
        columnSpacing: 1
        rowSpacing: 1
        clip: true

        model: Board {}

        delegate: Rectangle {
            implicitWidth: 100
            implicitHeight: 50

            Text {
                text: display
            }
        }
    }
}
