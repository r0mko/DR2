import QtQuick 2.0
import "../GraphView" as GraphView
Item {
    id: nodesView
    property int checkedIndex: nodesList.currentIndex
    property int cursorShape: -1
    property bool lockNodes: false
    property int nodeSize: 1
    property int nodeRadius: 1
    anchors.fill: parent
    opacity: 0

    signal canceled(int index)
    signal clicked(int index, var mouse)
    signal doubleClicked(int index, var mouse)
    signal entered(int index)
    signal exited(int index)
    signal positionChanged(int index, var mouse)
    signal pressAndHold(int index, var mouse)
    signal pressed(int index, var mouse)
    signal released(int index, var mouse)
    signal wheel(int index, var wheel)

    Behavior on opacity {
        NumberAnimation { duration: 250 }
    }

    Repeater {
        visible: false
        anchors.fill: parent
        model: datamodel.nodes
        delegate: GraphView.NodeDelegate {
            size: nodesView.nodeSize
            radius: nodesView.nodeRadius
            onClicked: nodesView.clickNode(index)
            onEntered: nodesView.hoverNode(index)
            onPressed: nodesView.pressNode(index, mouse)
            onReleased: nodesView.releaseNode(index)
            onPositionChanged: nodesView.mouseMove(index, mouse)

            checked: nodesView.checkedIndex === index
            locked: nodesView.lockNodes
            cursorShape: nodesView.cursorShape
            Connections {

            }
        }
    }

    property var clickNode: function(index) {}
    property var hoverNode: function(index, hover) {}
    property var pressNode: function(index, mouse) {}
    property var releaseNode: function(index) {}
    property var mouseMove: function(index, mouse) {}
}
