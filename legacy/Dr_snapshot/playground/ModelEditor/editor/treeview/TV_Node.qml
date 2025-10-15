import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import QtQml.Models 2.2
import Dr.MetaGraph 1.0
import Dr.UI 1.0

Item {
    id: root
    anchors.fill: parent
//    height: 18
    default property alias children: _layout.data
    property list<Action> actions
    Drag.dragType: Drag.Automatic
    Drag.mimeData: {
        "text/plain": styleData.value.nodeId,
        "application/octet-stream": styleData.value.nodeData
    }
    Drag.source: handleData
    Drag.active: mouseArea.drag.active


//    Drag.keys: [styleData.value.mrl, styleData.value.nodeId, "node"]
//    Drag.source: root
//    Drag.supportedActions: Qt.LinkAction

    Timer {
        id: delay_move
        property var node
        repeat: false
        interval: 200
        running: false
        onTriggered: if (node) node.parent = handleData.handle
    }

    Item {
        id: rowBounds
        anchors.fill: parent
        anchors.leftMargin: parent.parent ? -parent.parent.x : 0
        Rectangle {
            visible: styleData.value.modelIndex === tree_view.selection.currentIndex
            anchors.fill: parent
            border.width: 2
            border.color: "#3679d7"
            radius: 3
            color: "transparent"

        }
        DropArea {
            id: drop_node
            anchors.fill: parent
//            anchors.bottomMargin: 2
//            anchors.topMargin: 2
            onDropped: {
                console.log("dropped", drop.text, drop.formats)
                tree_view.sel.clearCurrentIndex()
//                tree_view.sel.clear()
//                delay_move.node = Drag.source.handle
//                delay_move.running = true
                var h = drop.source.source.handle
                console.log("source handle", h, "this handle", handleData.handle)
//                h.parent = handleData.handle
                delay_move.node = h
                delay_move.start()
                drop.accept()
            }
            Rectangle {
                anchors.fill: parent
                border.width: 2
                border.color: "#e2b100"
                radius: 3
                color: "transparent"
                visible: drop_node.containsDrag
            }
        }
        DropArea {
            id: drop_upper
            width: parent.width
            height: 6
            anchors.top: parent.top
            anchors.topMargin: -3
            onDropped: {

                var h = drop.source.source.handle
                console.log("dropped", drop.text, h, handleData.handle)
                if(h.parentIndex === handleData.handle.parentIndex) {
                    h.rowIndex = handleData.handle.rowIndex
                    drop.accept()
                    return
                }
                console.log("implement move between parent")
                drop.accepted = false

            }
            Rectangle {
                anchors.topMargin: 2
                anchors.bottomMargin: 2
                anchors.fill: parent
                color: "#e2b100"
                visible: drop_upper.containsDrag
            }
        }

//        DropArea {
//            id: drop_lower
//            width: parent.width
//            height: 6
//            anchors.bottom: parent.bottom
//            anchors.bottomMargin: -3
//            onDropped: {
//                drop.accept(Qt.LinkAction)
//                console.log("dropped", drop.getDataAsString("mrl"),  styleData.value.mrl, drop.getDataAsString("row"),  styleData.value.rowIndex)
//                var h = dasModel.getHandle(drop.getDataAsString("mrl"))
//                if(h.parent == handleData.handle.parent) {
//                    h.moveTo(handleData.handle.rowIndex)
//                }

//            }

//            Rectangle {
//                anchors.topMargin: 2
//                anchors.bottomMargin: 2
//                anchors.fill: parent
//                color: "#e2b100"
//                visible: drop_lower.containsDrag
//            }
//        }



    }

    RowLayout {
        id: _layout
        anchors.fill: parent
        spacing: 0
        NodeIcon {
            id: icon
            Layout.fillHeight: true
            Layout.minimumWidth: height
            iconColor: aux.delegates[nodeType].data.iconColor
            textColor: aux.delegates[nodeType].data.iconTextColor
            nodeType: handleData.typeName
        }

        EditableText {
            id: node_id
            Layout.fillWidth: true
            editable: styleData.selected
            delayEdit: true
            value: handleData.nodeId
            textColor: editable ? palette.light : palette.text
            editorComponent: TextFieldEditor {
                text: styleData.value.nodeId
                onValueChanged: styleData.value.nodeId = value
            }
        }
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
                tree_view.sel.selectByMouse(styleData.value.modelIndex, mouse, root)
                mouse.accepted = true
                return
            }
            if( mouse.button === Qt.LeftButton ) { // do right mouse stuff
                tree_view.setCurrentIndex(styleData.value.modelIndex)
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
                tree_view.sel.selectByMouse(styleData.value.modelIndex, mouse, root)
                mouse.accepted = true
                return
            }
            mouse.accepted = false
        }
        onDoubleClicked: {
            if (styleData.hasChildren)
                tree_view.isExpanded(styleData.value.modelIndex) ? tree_view.collapse(styleData.value.modelIndex) : tree_view.expand(styleData.value.modelIndex)
            else
                activateNodeIdEditor()
            mouse.accepted = false
        }
    }
    QtObject {
        id: handleData
        property var handle: styleData.value
        property string typeName
        property string nodeId
        property GraphTreeSelectionModel selection: tree_view.sel

        Binding on typeName {
            when: handleData.handle
            value: styleData.value.typeName
        }

        Binding on nodeId {
            when: handleData.handle
            value: styleData.value.nodeId
        }
    }
    function activateNodeIdEditor() {
        node_id.activateEditor()
    }

    /**/
}
