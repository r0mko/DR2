import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import Dr.MetaGraph 1.0
//import QtQuick.Extras 1.4
import Dr.UI 1.0
import "../treeview"
import QtQuick.Dialogs 1.2
RP_Node {
    id: root
    property var handle: objectData.handle
    //    states: [

    //    ]


    Item { Layout.minimumHeight: 5 }
    //Button {
    //    text: 'TEST'
    //    onClicked: objectData.handle.setType2()
    //}


    /**/
    CaptionLabel {
        Layout.fillWidth: true
        text: 'Collection specific stuff'
        level: 3
    }
    RowLayout {
        Layout.fillWidth: true
        Button {
            text: 'Указать тип'
            onClicked: setType.open()

            Dialog {
                id: setType
                visible: false
                standardButtons: StandardButton.Cancel
                width: createEdgeDialogContent.width + 24
                property var target: null
                //            property alias edgetag: edgetagtextfield.text
                onAccepted: {

//                    console.log( "pre f_Collection.createTypeConnection( root.handle, target )" )
//                    console.log( target )
//                    console.log( root.handle )
                    f_Collection.createTypeConnection( root.handle, target )// objectData.handle.setType( target )//dasModel.addEdge( objectData.handle, target, edgetag )
//                    console.log( "post f_Collection.createTypeConnection( root.handle, target )" )
                }

                ColumnLayout {
                    id: createEdgeDialogContent
                    width: 300
                    height: 400
                    Loader {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        active: setType.visible
                        sourceComponent: GraphTreeView {
                            id: selectionTreeView
                            graphModel: dasModel
                            selectionMode: SelectionMode.SingleSelection
                            onActiveIndexChanged: {
                                console.log( "active index changed", dasModel.getHandle(activeIndex) )
                                if( activeIndex !== null ) {
                                    setType.target = dasModel.getHandle(activeIndex)
                                    setType.standardButtons = StandardButton.Cancel | StandardButton.Ok
                                }else{
                                    setType.standardButtons = StandardButton.Cancel
                                }
                            }
//                            sel.onSelectionChanged: {
//                                console.log( "sel changed", sel.selectedIndexes.length )
//                                if( sel.selectedIndexes.length > 0  ) {
////                                if( sel.selectedRows().length > 0  ) {
//                                    setType.target = graphModel.getHandle( sel.selectedIndexes[0] )
//                                    setType.standardButtons = StandardButton.Cancel | StandardButton.Ok
//                                }else{
//                                    setType.standardButtons = StandardButton.Cancel
//                                }
//                            }
                        }
                    }
                    Item { Layout.minimumHeight: 25 }
                }
            }
        }

        Button {
            text: 'RemoveType'
            onClicked: f_Collection.removeTypeConnection( root.handle )
        }

        Button {
            enabled: objectData.collection$isStored
            text: 'Restore'
            onClicked: {
                root.handle.restore()
            }

        }
        Button {
            enabled: !objectData.collection$isStored
            text: 'Store'
            onClicked: root.handle.store()
        }
        Item{
            Layout.fillWidth: true
        }
    }

    JSObjectView {
//        visible: !objectData.collection$isStored
        Layout.fillWidth: true
        model: [
//            { "Название класса": root.handle["className"] },
//            { "metaTypeId": root.handle.metaTypeId }
            { "Название класса": objectData.collection$className },
            { "metaTypeId": objectData.collection$metaTypeId }
        ]
    }

    Item { Layout.minimumHeight: 5 }



    CaptionLabel {
        visible: !objectData.collection$isStored
        Layout.fillWidth: true
        text: 'Объекты коллекции'
        level: 3
    }
    Button {
        visible: !objectData.collection$isStored
        text: 'Добавить'
        onClicked: {
            root.handle.addElement()
        }
    }
/**/
    TableView {
        id: tablevieelements
        Layout.fillWidth: true
        Layout.minimumHeight: 300
        visible: !objectData.collection$isStored// && ( objectData.collection$elements === undefined )
//        model: objectData.collection$elements
        model: objectData.handle.elements


        TableViewColumn {
            title: 'Элемент'
            role: 'nodeId'
        }
        Instantiator {
            onObjectAdded: tablevieelements.addColumn(object)
            onObjectRemoved: tablevieelements.removeColumn(index)
            model: //objectData.collection$elements === undefined || objectData.collection$elements === null ? null : objectData.collection$roleModel()
//                   objectData.handle.roleModel
                    objectData.collection$roleModel
            TableViewColumn {
                title: modelData
                role: modelData
            }
        }
    }/**/
}

