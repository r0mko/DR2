import QtQuick 2.5
import QtQuick.Layouts 1.2
import Dr.MetaGraph 1.0
import QtQuick.Dialogs 1.2
import Dr.UI 1.0
import QtQuick.Controls 1.4
import QtQml.Models 2.2

TreeView {
    id: tree_view
    property GraphModel graphModel
    model: graphModel
    //                    model: graphRoot
    property alias deleteCurrentNodeAction: a_deleteCurrentNode
    property alias importAction: a_importNode
    property alias exportAction: a_exportNode
    property alias contextMenuAction: a_contextMenu
    property alias sel: _sel
    property var activeIndex: _sel.isSelected(_sel.currentIndex) ? (_sel.selectedIndexes.length > 1 ? null : _sel.selectedIndexes[0]) : _sel.currentIndex

    selectionMode: SelectionMode.ExtendedSelection

    Keys.onUpPressed: _sel.decrementRow(event.modifiers)
    Keys.onDownPressed: _sel.incrementRow(event.modifiers)
    Keys.onRightPressed: if (!isExpanded(_sel.currentIndex)) expand(_sel.currentIndex)
    Keys.onLeftPressed: if (isExpanded(_sel.currentIndex)) collapse(_sel.currentIndex)

    selection: GraphTreeSelectionModel {
        id: _sel
        model: graphModel
    }
    rowDelegate: Item {
        id: rowitem
        height: 18

        BorderImage {
            anchors.fill: parent
            visible: styleData.selected || styleData.alternate
            source: "image://__tablerow/" + (styleData.alternate ? "alternate_" : "") + (styleData.selected ? "selected_" : "") + "active"
            border.left: 5
            border.right: 5
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.RightButton
            onClicked: tree_view.contextMenuAction.trigger(null)
        }
    }
    itemDelegate: TreeNodeDelegate {
        handle: dasModel.getHandle(styleData.index)
        nodeId: styleData.value.length ? styleData.value : graphModel.address(styleData.index)
    }


    TableViewColumn {
        title: "Узлы"
        role: "nodeId"
    }

    Action {
        id: a_deleteCurrentNode
        text: "Delete"
        shortcut: StandardKey.Delete
        //        enabled: _sel.hasSelection || _sel.cu
        onTriggered: {
            var l = _sel.selectedIndexes.length
            _sel.buildTitle()
            ask_delete.text = "Delete %1?".arg(_sel.selectionTitle)
            ask_delete.open()
        }
    }

    Action {
        id: a_importNode
        text: "Import..."
        enabled: activeIndex
        onTriggered: importDialog.runMultiple(importNodes, tree_view.sel.currentIndex)
        function importNodes(fileUrls, idx) {
            if (!tree_view.isExpanded(idx))
                tree_view.expand(idx)
//                        _sel.clear()
            var h
            fileUrls.forEach(function(fileUrl) { h = graphModel.importFile(fileUrl, idx) })
            if (h) { _sel.selectElement(h.modelIndex) }
        }
        function importNode(fileUrl, idx) {
            //            var idx =
            if (!tree_view.isExpanded(idx))
                tree_view.expand(idx)
            var h = graphModel.importFile(fileUrl, idx)
//            sel.clear()
            setCurrentIndex(h.modelIndex)
        }
    }

    Action {
        id: a_exportNode
        text: "Export..."
        enabled: activeIndex
        onTriggered: exportDialog.run(
                         function(fileUrl, idx) {
                             graphModel.exportModel(fileUrl, idx)
                         }, tree_view.activeIndex)
    }
    Action {
        id: a_renameNode
        text: "Rename"
        enabled: activeIndex
        shortcut: "F2"
        onTriggered: ctxmenu.currentItem.activateNodeIdEditor()
    }

    Action {
        id: a_contextMenu
        onTriggered: ctxmenu.open(source)
    }

    Menu {
        id: ctxmenu
        property list<Action> extraActions
        property var nodeActions: [tree_view.deleteCurrentNodeAction, a_exportNode, a_importNode, a_renameNode]
        property var emptyActions: [a_importNode]
        function open(item) {
            headItem.text = item ? _sel.selectionTitle : "root node"
            base_menu.model = item ? nodeActions : emptyActions
            specific_menu.model = item ? item.actions : null
            popup()
        }
        MenuItem {
            id: headItem
            enabled: false
        }

        MenuSeparator {}

        Instantiator {
            id: base_menu
            model: null
            MenuItem { action: modelData }
            onObjectAdded: ctxmenu.insertItem(index, object)
            onObjectRemoved: ctxmenu.removeItem(object)
        }

        Instantiator {
            id: specific_menu
            model: null
            //            model: ctxmenu.extraActions
            MenuItem { action: modelData }
            onObjectAdded: ctxmenu.insertItem(index, object)
            onObjectRemoved: ctxmenu.removeItem(object)
        }

//        Instantiator {
//            model: graphModel.nodeFactories
//            delegate: MenuItem {
//                text: "Add " + modelData.type
//                property NodeFactory factory: modelData.factory
//                enabled: true
//                onTriggered: {
//                    var idx = tree_view.activeIndex.row === -1 ? 0 : tree_view.activeIndex
//                    factory.parent = graphModel.getHandle(idx)
//                    var c = factory.create()
//                    if (!tree_view.isExpanded(idx))
//                        tree_view.expand(idx)
//                    tree_view.setCurrentIndex(c.modelIndex)
//                    sel.selectElement(c.modelIndex)
//                }
//            }
//            onObjectAdded: ctxmenu.insertItem(index, object)
//            onObjectRemoved: ctxmenu.removeItem(object)
//        }

        Menu {
            id: node_menu
            title: "Add new..."
            Instantiator {
                model: graphModel.nodeFactories
                delegate: MenuItem {
                    text: modelData.type
                    property NodeFactory factory: modelData.factory
                    enabled: true
                    onTriggered: {
                        var idx = tree_view.activeIndex.row === -1 ? 0 : tree_view.activeIndex
                        factory.parentHandle = graphModel.getHandle(idx)
                        var c = factory.create()
                        if (!tree_view.isExpanded(idx))
                            tree_view.expand(idx)
                    }
                }
                onObjectAdded: node_menu.insertItem(index, object)
                onObjectRemoved: node_menu.removeItem(object)
            }
        }
    }

    Timer {
        id: delay_move
        property var node
        property var parentNode
        property int targetRow

        repeat: false
        interval: 50
        running: false

//        onTriggered: if (node) node.parent = dragData.handle
        onTriggered: {
            if (!node) {
                console.log("moving selection", _sel.selectedIndexes.length, "into row" << targetRow << "of" << parentNode.mrl)
                var start = targetRow - 5
                var end = targetRow + 5
                if (start < 0)
                    start = 0
                if (end > parentNode.childCount)
                    end = parentNode.childCount
                console.log("Before move:", parentNode.modelIndex)
                graphModel.dumpChildren(parentNode.modelIndex, true, start, end)
                end += _sel.selectedIndexes.length
                graphModel.moveSelection(_sel.selection, parentNode.modelIndex, targetRow)
                console.log("After move:", parentNode.modelIndex)
                graphModel.dumpChildren(parentNode.modelIndex, true, start, end)
            } else {

                start = node.rowIndex - 2
                end = node.rowIndex + 2
                if (start < 0)
                    start = 0
                if (end > node.parent.childCount)
                    end = node.parent.childCount
                console.log("moving node", node, _sel.selectedIndexes[0])
                console.log("Before move")
                graphModel.dumpChildren(node.parent.modelIndex, true, start, end)
                var h = graphModel.moveNode(node.modelIndex, parentNode.modelIndex, targetRow)
                console.log("After move:", parentNode)
                graphModel.dumpChildren(parentNode.modelIndex, true)
                if (!h)
                    return
                setCurrentIndex(h.modelIndex)
//                console.log("returned node", h, h.rowIndex)
            }
            if (!tree_view.isExpanded(parentNode.modelIndex)) {
                console.log(parentNode, "is not expanded")
                tree_view.expand(parentNode.modelIndex)
            }
        }
    }


    MessageDialog {
        id: ask_delete
        title: "Delete"
        standardButtons: StandardButton.Yes | StandardButton.Cancel
        onYes: {
            if (_sel.isSelected(_sel.currentIndex))
                graphModel.removeNodes(_sel.selection)
            else
                graphModel.removeNode(_sel.currentIndex)
        }
    }

    function setCurrentIndex(index) {
        if (_sel.isSelected(index)) {
            _sel.setCurrentIndex(index, ItemSelectionModel.Current)
        } else {
            _sel.setCurrentIndex(index, ItemSelectionModel.Current)
        }
    }

    function clearSelection() {
        _sel.clear()
        _sel.clearCurrentIndex()
    }

    function addToSelection(index) {
        _sel.select(index, ItemSelectionModel.Select)
    }

    function changeParent(handle, newParent) {
        if(handle.parentIndex == newParent.modelIndex)
            return
        delay_move.node = handle
        delay_move.parentNode = newParent
        delay_move.targetRow = -1
        delay_move.start()
    }
    function moveNode(handle, targetNode, targetIndex) {
        delay_move.node = handle
        delay_move.parentNode = targetNode
        delay_move.targetRow = targetIndex
        delay_move.start()
    }

    function moveSelection(targetNode, targetIndex) {
        delay_move.node = null
        delay_move.parentNode = targetNode
        delay_move.targetRow = targetIndex
        delay_move.start()
    }

}
