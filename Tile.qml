import QtQuick 2.0

Rectangle {
    property bool ready: false
    property alias blink: blinking.running
    property int newIndex: model.index
    property bool fallDown: model.active
    height: gridView.cellHeight
    width: gridView.cellWidth
    radius: 100
    color: model.color

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
        onStarted: root.countFallingDownAnimation += 1;
        onStopped: root.countFallingDownAnimation -= 1;
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
            if (root.firstSelectedItem == -1)
            {
                root.firstSelectedItem = index;
                blinking.start();
            } else {
                if (boardModel.move(root.firstSelectedItem, index)) {
                    root.secondSelectedItem = index;
                } else {
                    gridView.itemAtIndex(root.firstSelectedItem).blink = false;
                    root.firstSelectedItem = index;
                    blinking.start();
                }
            }
        }
    }

    Component.onCompleted: ready = true;
}
