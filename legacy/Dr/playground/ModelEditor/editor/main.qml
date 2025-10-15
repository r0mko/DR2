import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import Dr.MetaGraph 1.0
import QtQuick.Dialogs 1.2
import Dr.UI 1.0
import "treeview"
import "rightpanel"
import Playground 1.0
import QtQml.Models 2.2

//import QtQml.StateMachine 1.0 as DSM
//import QtQml.Models 2.2
ApplicationWindow {
    id: mainWin
    title: qsTr("Metagraph Model Editor 1.2")
    width: 1300
    height: 800
    visible: true
    color: "#eee"
    Component.onCompleted: {
        dasModel.init()
    }

    menuBar: MenuBar {
        Menu {
            title: "File"
            MenuItem {
                text: "Initialize model"
                shortcut: "Ctrl+Shift+N"

                onTriggered: {
                    dasModel.fileName = ""
                    dasModel.init()
                }
            }

            MenuItem {
                text: "Open..."
                shortcut: "Ctrl+O"
                onTriggered: importDialog.run(openModel)
                function openModel(fileUrl) {
                    dasModel.fileName = fileUrl
                    dasModel.loadModel(fileUrl)
                }
            }

            MenuItem {
                text: "Import to Root..."
                shortcut: "Ctrl+Shift+A"
                onTriggered: {
                    tree_view.sel.clearCurrentIndex()
                    //                    tree_view.sel.clear()
                    tree_view.importAction.trigger()
                }
            }

            MenuItem {
                text: "Save %1".arg(dasModel.fileName)
                shortcut: "Ctrl+S"
                enabled: dasModel.fileName.toString().length > 0
                onTriggered: dasModel.exportModel(dasModel.fileName)
            }

            MenuItem {
                text: "Save As..."
                shortcut: "Ctrl+Shift+S"
                onTriggered: exportDialog.run(saveModel)
                function saveModel(fileUrl) {
                    dasModel.exportModel(fileUrl)
                    dasModel.fileName = fileUrl
                }
            }

            MenuItem {
                text: "Revert to saved"
                enabled: dasModel.fileName.toString().length > 0
                onTriggered: dasModel.loadModel(dasModel.fileName)
            }
        }

        Menu {
            title: "Tools"
            MenuItem {
                text: "Reload config"
                onTriggered: aux.init()
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 8

        ColumnLayout {
            Layout.fillHeight: true

            Rectangle {
                Layout.fillHeight: true
                Layout.minimumWidth: 280
                GraphTreeView {
                    id: tree_view
                    anchors.fill: parent
                    graphModel: dasModel
                    property var currentHandle: dasModel.getHandle(activeIndex)
                }
            }
        }


        Item {
            id: rightView
            Layout.fillHeight: true
            Layout.fillWidth: true

            GroupBox {
                anchors.fill: parent
                DefaultDelegateWrapper {
                    id: rightLoader
                    anchors.fill: parent
//                    onNodeModelDataChanged: console.log( "currentHandle", tree_view.currentHandle, tree_view.currentHandle.agent)
//                    nodeModelData: tree_view.currentHandle ? tree_view.currentHandle.agent : null
                    nodeModelData: tree_view.currentHandle ? dasModel.get(tree_view.currentHandle.modelIndex) : null
                    treeSelection: tree_view.sel

                }
            }
        }
    }


    FileDialog {
        id: importDialog
        //            id: fileselector
        modality: Qt.NonModal
        property var argument
        property var callback
        selectExisting: true
        nameFilters: ["MetaGraph model files (*.mgf)"]

        function runMultiple(callbackFn, arg) {
            selectMultiple = true
            callback = callbackFn
            argument = arg
            open()
        }
        function run(callbackFn, arg) {
            callback = callbackFn
            argument = arg
            open()
        }
        onAccepted: {
            if (selectMultiple) {
                callback(fileUrls, argument)
                selectMultiple = false
            } else {
                callback(fileUrl, argument)
            }
        }
    }
    FileDialog {
        id: exportDialog
        //            id: fileselector
        property var argument
        property var callback
        selectExisting: false
        nameFilters: ["MetaGraph model files (*.mgf)"]

        function run(callbackFn, arg) {
            callback = callbackFn
            argument = arg
            open()
        }
        onAccepted: { callback(fileUrl, argument) }
    }

    Dialog {
        id: dia_selectParent
        visible: false
        standardButtons: StandardButton.Cancel
        width: dia_selectParentContent.width + 24

        property var handle
        property var target
        property bool multiple
        property ItemSelectionModel selection: null

        function show(childHandle) {
            handle = childHandle
            selection = null
            multiple = false
            open()
        }

        function showMulti(selectionModel) {
            multiple = true
            selection = selectionModel
            open()
        }

        onAccepted: {
            if (selection) {
                dasModel.moveSelection(selection.selection, target.modelIndex)
            } else {
                handle.parent = target
            }

        }

        ColumnLayout {
            id: dia_selectParentContent
            width: 300
            height: 400
            GraphTreeView {
                anchors.fill: parent
                graphModel: dasModel
                selectionMode: SelectionMode.SingleSelection
                sel.onSelectionChanged:  {
                    var cHandle = graphModel.getHandle(sel.currentIndex)

                    dia_selectParent.target = null
                    dia_selectParent.standardButtons = StandardButton.Cancel

                    if( sel.selectedIndexes.length === 1 ) {

                        if (dia_selectParent.selection) {
                            if(dia_selectParent.selection.isSelected(sel.currentIndex)) {
                                sel.clear()
                                sel.clearCurrentIndex()
                                console.log("cannot move to own children")
                                return
                            }
                        } else {
                            if (cHandle === dia_selectParent.handle) {
                                sel.clear()
                                sel.clearCurrentIndex()
                                console.log("cannot move to own children")
                                return
                            }
                            if (cHandle.parent === dia_selectParent.handle) {
                                console.log("already this parent")
                                return
                            }
                        }
                    }

                    dia_selectParent.target = cHandle
                    dia_selectParent.standardButtons = StandardButton.Cancel | StandardButton.Ok
                }
            }
            Item { Layout.minimumHeight: 25 }
        }
    }


    GraphModel {
        id: dasModel
        property url fileName
        RootNodeFactory { }

        RootNodeFactory { id: f_RootNode }
        NodeFactory { id: f_Node }
        DataNodeFactory { id: f_DataNode }
        MetaNodeFactory { id: f_MetaNode }
        CollectionFactory { id: f_Collection }
        TypeBuilder { id: f_Type }
        BlobFactory { id: f_Blob }
        InstanceFactory { id: f_Instance }
        ElementFactory { id: f_Element }
        SqlBridgeNodeFactory { id: f_SqlBridgeNode }
        PointNodeFactory { id: f_PointNode }

        function init() {
            clear()
            //            var n1 = f_Node.create("node")
            //            var n2 = f_DataNode.create("data")
            //            var n4 = f_Type.create("MyClass")
            //            var n3 = f_Collection.create("collection")
            //            var n5 = f_Blob.create("blob")
            //            var n6 = f_SqlBridgeNode.create("sqlBridgeNode")
            ////            var n7 = f_Collection2.create("collection2")
            ////            var n6 = f_Instance.create("instance")

            ////f_Collection
            ////            n3.setType()
            ////            n3.setType(null)
            ////            n3.setType(n4)

            //            f_MetaNode.addMember(n1)
            //            f_MetaNode.addMember(n2)
            //            f_MetaNode.addMember(n3)
            //            f_MetaNode.addMember(n4)
            //            f_MetaNode.addMember(n5)
            ////            f_MetaNode.addMember(n6)
            //            f_MetaNode.create("metanode")
        }
    }

    EdgeModel {
        id: edgeModel
        model: dasModel
//        tag:
        all: true
    }
    Window {
        visible: true
        width: 300
        height: 400
        TableView {
            model: edgeModel
            anchors.fill: parent
            TableViewColumn {
                role: 'tag'
            }
            TableViewColumn {
                role: 'edgeData'
                delegate: Label {
                    text: JSON.stringify(styleData.value)
                }

            }
        }
    }

    SystemPalette { id: palette }

    Component {
        id: comp_multisel
        GroupBox {
            Label {
                text: "multiple selected"
            }
        }
    }

    AuxModel {
        id: aux
    }

}
