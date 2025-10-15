import QtQuick 2.0

Item {
    anchors.fill: parent
    anchors.topMargin: 2
    anchors.bottomMargin: 2
    Rectangle {
        anchors.right: parent.right
        height: parent.height
        width: parent.height
        color: "transparent"
        border.width: 2
        border.color: "black"
        radius: 5
        Rectangle {
            anchors.fill: parent
            anchors.margins: 4
            radius: 3
            color: value ? "black" : "transparent"

        }
    }
}
