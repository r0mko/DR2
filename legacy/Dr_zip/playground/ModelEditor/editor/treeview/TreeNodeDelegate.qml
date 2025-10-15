import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import QtQml.Models 2.2
import Dr.MetaGraph 1.0
import Dr.UI 1.0

Item {
    id: root
    //    anchors.fill: parent
    //    width: parent.width
    default property alias children: _layout.data
    property string type: handle.typeName
    property alias nodeId: node_id.text
    //    property QtObject agent: dasModel.get(styleData.index)
    property var handle
    property list<Action> actions

    Item {
        id: rowBounds
        anchors.fill: parent
        anchors.leftMargin: -root.x
        Rectangle {
            visible: handle.modelIndex === tree_view.selection.currentIndex
            anchors.fill: parent
            border.width: 2
            border.color: "#3679d7"
            radius: 3
            color: "transparent"

        }
        DropArea {
            id: drop_node
            anchors.fill: parent
            anchors.bottomMargin: 3
            anchors.topMargin: 3
            onDropped: {
                //                if(tree_view.sel.isSelected(drop.source.source.handle.modelIndex)) {
                if(tree_view.sel.selectedIndexes.length > 1) {
                    tree_view.moveSelection(handle, -1)
                    drop.accept(Qt.MoveAction)
                    return
                }
                var dropped = drop.source.source.handle
                if (dropped.modelIndex === dragData.handle.modelIndex) {
                    drop.accepted = false
                    return
                }
                //                console.log("dropped one node", drop.source.source.handle)
                tree_view.moveNode(dropped, handle, -1)
                drop.accept(Qt.MoveAction)

                //                drop.accepted = false

                //                console.log("dropped", drop.text, drop.formats, drop.x, drop.y)
                //                tree_view.sel.clear()
                //                delay_move.node = Drag.source.handle
                //                delay_move.running = true
                //                var h = drop.source.source.handle
                //                if (h.modelIndex === dragData.handle.modelIndex) {
                ////                    console.log("same shit")
                //                    drop.accepted = false
                //                    return
                //                }
                //                tree_view.sel.clearCurrentIndex()
                ////                console.log("source handle", h, "this handle", dragData.handle)
                ////                h.parent = dragData.handle
                //                delay_move.node = h
                //                delay_move.start()


            }
            Rectangle {
                anchors.fill: parent
                anchors.bottomMargin: -3
                anchors.topMargin: -3
                border.width: 3
                border.color: "#cc0634"
                radius: 8
                color: "transparent"
                visible: drop_node.containsDrag
            }
        }
        DropArea {
            id: drop_upper
            width: parent.width
            //            anchors.right: parent.right
            //            x: root.x
            height: 8
            anchors.top: parent.top
            anchors.topMargin: -4

            onDropped: {
                if (dragData.multiple) {
                    tree_view.moveSelection(handle.parent, handle.rowIndex)
                    drop.accept(Qt.MoveAction)
                } else  {
                    tree_view.moveNode(drop.source.source.handle, handle.parent, handle.rowIndex - 1)
                    drop.accept(Qt.MoveAction)
                }

            }

            Rectangle {
                anchors.topMargin: 3
                anchors.bottomMargin: 3
                anchors.fill: parent
                color: "#cc0634"
                visible: drop_upper.containsDrag
            }
        }
        DropArea {
            id: drop_lower
            width: parent.width
            //            anchors.right: parent.right
            //            x: root.x
            height: 8
            anchors.top: parent.top
            anchors.topMargin: -4

            onDropped: {
                if(tree_view.sel.isSelected(drop.source.source.handle)) {
                    tree_view.moveSelection(handle.parent, handle.rowIndex + 1)
                    //                    console.log("moving selection", drop.source.source.handle.mrl, "to parent", handle.parent.mrl, "at index", handle.rowIndex)
                    drop.accept(Qt.MoveAction)
                    return
                }
                //                console.log("dropped one node", drop.source.source.handle)
                tree_view.moveNode(drop.source.source.handle, handle.parent, handle.rowIndex)
                drop.accept(Qt.MoveAction)
            }

            Rectangle {
                anchors.topMargin: 3
                anchors.bottomMargin: 3
                anchors.fill: parent
                color: "#cc0634"
                visible: drop_lower.containsDrag
            }
        }
    }

    RowLayout {
        id: _layout
        anchors.fill: parent
        spacing: 0

        Drag.mimeData: { "text/plain": dragData.plaintext }
        Drag.dragType: Drag.Automatic

        Drag.proposedAction: Qt.CopyAction
        Drag.supportedActions: Qt.CopyAction | Qt.LinkAction | Qt.MoveAction
        Drag.source: dragData
        Drag.active: mouseArea.drag.active

        NodeIcon {
            id: icon
            Layout.fillHeight: true
            Layout.minimumWidth: height
            iconColor: nodeType ? aux.delegates[nodeType].data.iconColor : "#000"
            textColor: nodeType ? aux.delegates[nodeType].data.iconTextColor : "#333"
            nodeType: handle.typeName
        }

        Label {
            id: node_id
            color: styleData.selected ? palette.light : palette.text
        }

        //        EditableText {
        //            id: node_id
        //            Layout.fillWidth: true
        //            editable: styleData.selected
        //            delayEdit: true
        //            value: handle.nodeId
        //            textColor: editable ? palette.light : palette.text
        //            editorComponent: TextFieldEditor {
        //                text: styleData.value.nodeId
        //                onValueChanged: styleData.value.nodeId = value
        //            }
        //        }

        Item {
            Layout.fillWidth: true
        }
//        Label {
//            text: agent ? agent.address.join(".") : ""
//            font.bold: true
//            font.pixelSize: 10
//        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        //        acceptedButtons: Qt.RightButton
        acceptedButtons: Qt.AllButtons
        drag.target: _layout
        propagateComposedEvents: true
        onPressed: {
            if( mouse.button === Qt.RightButton ) { // do right mouse stuff
                tree_view.sel.selectByMouse(handle.modelIndex, mouse, root)
                mouse.accepted = true
                return
            }
            if( mouse.button === Qt.LeftButton ) { // do left mouse stuff
                //                tree_view.setCurrentIndex(styleData.value.modelIndex)
                var h2 = dasModel.getHandle(styleData.index)
//                console.log("press", handle.mrl, handle.address.join("."), h2.mrl, h2.address.join("."))
                //                tree_view.setCurrentIndex(styleData.index)
                tree_view.setCurrentIndex(handle.modelIndex)
                //                tree_view
                mouse.accepted = true
                return
            }

        }
        onClicked: {
            if (mouse.button == Qt.RightButton) {
                tree_view.contextMenuAction.trigger(root)
                mouse.accepted = true
                return
            }
            if (!styleData.selected && mouse.button === Qt.LeftButton) {
                var h2 = dasModel.getHandle(styleData.index)
//                console.log("clicked", handle.mrl, handle.address.join("."), h2.mrl, h2.address.join("."))
                tree_view.sel.selectByMouse(handle.modelIndex, mouse, root)
                //                tree_view.sel.selectByMouse(styleData.value.modelIndex, mouse, root)
                mouse.accepted = true
                return
            }
            mouse.accepted = false
        }
        onDoubleClicked: {
            if (styleData.hasChildren)
                tree_view.isExpanded(styleData.index) ? tree_view.collapse(styleData.index) : tree_view.expand(styleData.index)
            //            else
            //                activateNodeIdEditor()
            mouse.accepted = false
        }
    }
    QtObject {
        id: dragData
        property var handle: root.handle
        property bool multiple: tree_view.sel.selectedIndexes.length > 1
//        property bool multiple: tree_view.sel.isSelected(handle.modelIndex) && tree_view.sel.selectedIndexes.length > 1
        property string plaintext: (multiple)
                                  ? "%1 nodes".arg(tree_view.sel.selectedIndexes.length)
                                  : "%1".arg(nodeId)
    }
    function activateNodeIdEditor() {
        //        node_id.activateEditor()
    }

    /**/
}
