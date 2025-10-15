import QtQuick 2.0

Rectangle {
    id: handle
    property bool checked: false
    property bool locked
    property int nodeIdx: index
    property int size
//    property alias dragging: mArea.dragActive
    property alias cursorShape: mArea.cursorShape
    property alias mouseArea: mArea

    signal canceled(int index)
    signal clicked(int index, variant mouse)
    signal doubleClicked(int index, var mouse)
    signal entered(int index)
    signal exited(int index)
    signal positionChanged(int index, var mouse)
    signal pressAndHold(int index, var mouse)
    signal pressed(int index, var mouse)
    signal released(int index, var mouse)
    signal wheel(int index, var wheel)

    Behavior on size { NumberAnimation { duration: 250; easing.type: Easing.OutCubic } }
    Behavior on radius { NumberAnimation { duration: 250 } }

    Component.onCompleted: console.log(parent, parent.parent, parent.parent.color)

    width: size
    height: size
    radius: 8
    color: "transparent"
    x: i.x
    y: i.y
    z: 10
    onXChanged: i.x = x
    onYChanged: i.y = y

    border.width: 1
    border.color: "#80FFFFFF"

    transform: Translate {
        x: -handle.width / 2
        y: -handle.height / 2
    }

    Rectangle {
        anchors.fill: parent
        anchors.margins: -3
        color: "transparent"
        border.width: 2
        border.color: "#3ab6ff"
        visible: handle.checked
        radius: 11
    }

    MouseArea {
        id: mArea
        acceptedButtons: Qt.AllButtons
        anchors.fill: parent
        hoverEnabled: true
        drag.target: locked ? null: handle
        drag.threshold: 0
        onCanceled: handle.canceled(index)
        onClicked: handle.clicked(index, mouse)
        onDoubleClicked: handle.doubleClicked(index, mouse)
        onEntered: handle.entered(index)
        onExited: handle.exited(index)
        onPositionChanged: handle.positionChanged(index, mouse)
        onPressAndHold: handle.pressAndHold(index, mouse)
        onPressed: handle.pressed(index, mouse)
        onReleased: handle.released(index, mouse)
        onWheel: handle.wheel(index, wheel)
    }

}
