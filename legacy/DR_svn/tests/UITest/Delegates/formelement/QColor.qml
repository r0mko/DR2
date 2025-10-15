import QtQuick 2.0

Item {
    anchors.fill: parent
    Rectangle {
        anchors.right: parent.right
        height: parent.height
        width: parent.height
        color: value
        border.width: 2
        border.color: "black"
        radius: 5
    }
}
