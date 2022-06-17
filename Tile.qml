import QtQuick 2.0

Rectangle {
    id: root

    property bool ready: false
    property alias blink: blinking.running
    property int newIndex: -1
    property bool fallDown: false

    signal moveAnimationRunningChanged(bool running)
    signal fallAnimationRunningChanged(bool running)
    signal clicked()

    height: gridView.cellHeight
    width: gridView.cellWidth
    radius: 100
    color: "transparent"

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
        onStarted: fallAnimationRunningChanged(running)
        onStopped: fallAnimationRunningChanged(running)
    }

    Behavior on x {
        NumberAnimation {
            duration: 2000
            onRunningChanged: moveAnimationRunningChanged(running)
        }
    }
    Behavior on y {
        NumberAnimation {
            duration: 2000
            onRunningChanged: moveAnimationRunningChanged(running)
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
        onClicked: root.clicked()
    }

    Component.onCompleted: ready = true;
}
