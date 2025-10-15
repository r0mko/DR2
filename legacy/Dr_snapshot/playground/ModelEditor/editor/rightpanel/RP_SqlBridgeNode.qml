import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import "../treeview"
RP_Node {

    CaptionLabel {
        Layout.fillWidth: true
        text: 'SqlBridgeNode specific stuff '// + objectData.typeName
        level: 3
    }


    Button {
        text: 'test'
        onClicked: {
            setDestination.open()
//            objectData.handle.test()
        }
        Dialog {
            id: setDestination
            visible: false
            standardButtons: StandardButton.Cancel
            width: createEdgeDialogContent.width + 24
            property var target: null
            //            property alias edgetag: edgetagtextfield.text
            onAccepted: {

                //                    console.log( "pre f_Collection.createTypeConnection( root.handle, target )" )
                //                    console.log( target )
                //                    console.log( root.handle )
                objectData.handle.test(target)
//                f_Collection.createTypeConnection( root.handle, target )// objectData.handle.setType( target )//dasModel.addEdge( objectData.handle, target, edgetag )
                //                    console.log( "post f_Collection.createTypeConnection( root.handle, target )" )
            }

            ColumnLayout {
                id: createEdgeDialogContent
                width: 300
                height: 400
                Loader {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    active: setDestination.visible
                    sourceComponent: GraphTreeView {
                        id: selectionTreeView
                        graphModel: dasModel
                        selectionMode: SelectionMode.SingleSelection
                        onActiveIndexChanged: {
                            console.log( "active index changed", dasModel.getHandle(activeIndex) )
                            if( activeIndex !== null ) {
                                setDestination.target = dasModel.getHandle(activeIndex)
                                setDestination.standardButtons = StandardButton.Cancel | StandardButton.Ok
                            }else{
                                setDestination.standardButtons = StandardButton.Cancel
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
}

