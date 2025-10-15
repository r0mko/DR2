import QtQuick 2.3

Item {
    width: ListView.view.width

    height: 30
    Column {
        anchors.fill: parent
        anchors.margins: 3
        Text {
            color: "black"
            font.pointSize: 12
            text: "Node %1".arg(index)
        }
        Text {
            color: "black"
            font.pointSize: 8
            text: "x: %2, y: %3".arg(i.x).arg(i.y)
        }
    }

    focus: ListView.isCurrentItem
    onFocusChanged: console.log("idx: %1 focus: %2".arg(index).arg(focus))
    Keys.onDeletePressed: del()
    MouseArea {
        anchors.fill: parent
        onClicked: select()
    }

    function select() {
        ListView.view.currentIndex = index
    }

    function del() {
        ListView.view.model.remove(index)
    }
}
