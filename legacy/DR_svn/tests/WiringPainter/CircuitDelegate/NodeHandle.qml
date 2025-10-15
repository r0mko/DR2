import QtQuick 2.3
import QtQuick.Controls 1.2
import '../'
Rectangle {
    id: handle
    width: 16
    height: 16
    radius: 8
    color: mArea.containsMouse ? "#20ffffff" : "transparent"
    border.color: "#ffffff"
    border.width: 1

    property int index
    property Node node: Node {
        x: handle.x
        y: handle.y
    }
    onNodeChanged: { x = node.x; y = node.y }

    property bool checked: false
    property ExclusiveGroup exclusiveGroup: null
    property alias mouseArea: mArea

    focus: checked

    Keys.onDeletePressed:  {
        console.log("Destroy", handle, node)
        destroy()
    }
//    Keys.onDeletePressed: drawboard.deleteNode(index)

    transform: Translate {
        x: -handle.width / 2
        y: -handle.height / 2
    }

    onXChanged: drawboard.requestPaint()
    onYChanged: drawboard.requestPaint()

    onExclusiveGroupChanged: {
        if (exclusiveGroup)
            exclusiveGroup.bindCheckable(handle)
    }

    Rectangle {
        id: selectFrame
        anchors.fill: parent
        anchors.margins: -3
        color: "transparent"
        border.width: 2
        border.color: "#3a96ff"
        radius: 19
        visible: handle.checked
    }

    Text {
        width: parent.width
        anchors.bottomMargin: 2
        anchors.bottom: handle.top
        visible: mArea.containsMouse || handle.checked
        text: "N%1".arg(handle.index)
        font.bold: true
        styleColor: "#000000"
        style: Text.Outline
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignBottom
        color: "#ffcc00"
        font.pointSize: 10
    }

    MouseArea {
        id: mArea
        cursorShape: drawboard.nodesMovable ? (drag.active ? Qt.SizeAllCursor : Qt.PointingHandCursor) : Qt.PointingHandCursor
        anchors.fill: parent
        hoverEnabled: true
        drag.target: drawboard.nodesMovable ? handle : null
        drag.threshold: 0
        onClicked: drawboard.clickHandle(handle.index)
        onPressed: drawboard.pressHandle(handle.index)
        onReleased: drawboard.releaseHandle(handle.index)
//        drag.onActiveChanged: {
//            if (!drag.active) {
//                drawboard.nodes_var[handle.index].x = handle.x
//                drawboard.nodes_var[handle.index].y = handle.y
//                settings.nodes = drawboard.nodes_var
//            }
//        }
    }
}
