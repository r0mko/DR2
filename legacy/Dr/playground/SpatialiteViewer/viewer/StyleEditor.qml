import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import Dr.Cartograph 1.0
import Dr.MetaGraph 1.0
import QtQml.Models 2.2
import Dr.Graphics 1.0
import Dr.UI 1.0

Window {
    id: rootWindow
    width: 700
    height: 500
    color: "#eee"
    property NodeChildrenModel stylesModel
    property GeoView geoView
    //    property var geomModelItem: styleList.currentItem.geomModelItem
    //    onGeomModelItemChanged: if (geomModelItem)
    //            layerId = geomModelItem.parent.rowIndex
    //        else
    //            geomId = geomModelItem.rowIndex


    property int layerId
    property int geomId
    //    onGeomModelItemChanged: console.log(geomModelItem.rowIndex, geomModelItem.parent, geomModelItem.parent.rowIndex)
    RowLayout {
        anchors.fill: parent
        anchors.margins: 5
        ColumnLayout {
            ScrollView {
                frameVisible: true
                Layout.fillHeight: true
                Layout.minimumWidth: 300
                ListView {
                    id: styleList
                    anchors.fill: parent
                    model: stylesModel
                    //                    onCurrentIndexChanged: rightPanel.agent = stylesModel.get(currentIndex)
                    delegate: StyleListDelegate {
                        width: parent.width
                        geomModelItem: handle.type == 'Style' ? handle.inNodes()[0] : handle.parent.inNodes()[0]
                    }

                }
            }
            RowLayout {

                TextField {
                    id: tf_styleName
                    onAccepted: {
                        focus = false
                    }

                }


                PicToolButton {
                    //                    text: "Add"
                    iconName: "add"
                    enabled: tf_styleName.length != 0 || !styles_model.hasChild(tf_styleName)
                    onClicked: {
                        fBlob.parentHandle = styles_model.rootHandle
                        fBlob.type = "Style"
                        fBlob.metadata = { opacity: 1.0 }
                        fBlob.create(tf_styleName.text)
                    }
                }

                PicToolButton {
                    iconName: "cross"
                    enabled: styleList.currentIndex >= 0
                    onClicked: {
                        styles_model.getHandle(styleList.currentIndex).destroy()
                        styleList.currentIndex = -1
                    }
                }

            }

        }

        ColumnLayout {
            id: rightPanel
            anchors.top: parent.top
            Layout.fillHeight: true
            Layout.fillWidth: true
            spacing: 4
            NodeChildrenModel {
                id: style_subList
                rootIndex: stylesModel.getHandle(styleList.currentIndex).modelIndex
                //                onRootIndexChanged: console.log('onrootIndexChanged', main_model.getHandle(rootIndex), rootIndex, count )
                //                model: main_model
                //                                onSourceModelChanged: console.log("source model", sourceModel, rootIndex, count)
            }

            //            property int currentIndex:
            property QtObject agent: stylesModel.get(styleList.currentIndex)
            onAgentChanged: {
                rowmaker.model = null
                rowmaker.model = geoView.getStyleProperties(agent.blob$type)
                chrepeater.model = null
                chrepeater.model = style_subList
                //                console.log( '!!!!!!!!!!!!!!',agent, JSON.stringify(agent.blob$metadata), agent.blob$type, style_subList.count )
            }
            //            property var handle


            ColumnLayout {
                anchors.left: parent.left
                anchors.right: parent.right
                spacing: 1
                CaptionLabel {
                    Layout.fillWidth: true
                    level: 3
                    text: rightPanel.agent.blob$type
                }


                /**/
                JSObjectView {
                    id: p_ed
                    Layout.fillWidth: true
                    property QtObject modelAgent: rightPanel.agent
                    Repeater {
                        id: rowmaker
                        width: parent.width
                        model: geoView.getStyleProperties(rightPanel.agent.blob$type)
                        //                        Component.onCompleted: console.log(JSON.stringify(model))
                        JSObjectViewRow {
                            implicitHeight: 20
                            text: modelData.name
                            valueDelegate: Loader {
                                width: 20
                                height: 20
                                property string propertyName: modelData.name
                                //                                onPropertyNameChanged: console.log( 'propertyName', propertyName )
                                signal valueUpdated(var newValue)
                                //                                property var modelValue: p_ed.modelAgent.blob$metadata[propertyName]
                                property var modelValue: rightPanel.agent.blob$metadata.opacity
                                onValueUpdated: {
                                    var md = p_ed.modelAgent.blob$metadata
                                    md[propertyName] = newValue
                                    p_ed.modelAgent.blob$metadata = md
                                }
                                //                                source: "TableCell%1.qml".arg(modelData.type)
                                source: "ValueEditors/VE_slider.qml"
                                //                                onSourceChanged: console.log( 'sourceChanged:', source )
                            }
                        }
                        onItemAdded: {
                            //                            console.log("added line", rowmaker.height, count)
                            //                            height = count * 20
                            //                            s_list.forceLayout()
                        }
                        onItemRemoved: {
                            //                            height = count * 20
                            //                            s_list.forceLayout()
                        }
                    }

                }

                Repeater {
                    id: chrepeater
                    model: style_subList
                    //                    onModelChanged: console.log( 'style_subList', style_subList.count)
                    Layout.fillWidth: true

                    ColumnLayout {
                        id: childs
                        spacing: 1
                        anchors.left: parent.left
                        anchors.right: parent.right
                        property QtObject ag: style_subList.get(index)
                        onAgChanged: {
                            reprows.model = null
                            reprows.model = geoView.getStyleProperties(childs.ag.blob$type)
                        }
                        Item {
                            Layout.minimumHeight: 4
                        }

                        CaptionLabel {
                            id: cl
                            Layout.fillWidth: true
                            level: 3
                            text: childs.ag.blob$type
                            PicToolButton {
                                //                    text: "Add"
                                anchors.right: parent.right

                                iconName: "cross"
                                visible: cmb_styleType.currentIndex >= 0
                                onClicked: {
                                    var p = childs.ag.handle
                                    var styleInst = i_styles.instance(stylesModel.getHandle(styleList.currentIndex).childNodes[index].modelIndex)
                                    console.log(styleInst)
                                    styleInst.deleted = true
//                                    style_subList.remove(index)
                                    for( var j = 0; j < p.inNodes().length; ++j ) {
                                        var h = p.inNodes()[ j ]
                                        var ridx1 = h.rowIndex
                                        var ridx2 = h.parent.rowIndex
                                        console.log( 'HANDLE', h, ridx1, ridx2 )
                                        i_layers.objectAt(ridx2).update()//iGeomBuffer.objectAt(ridx1).style = p
                                    }
//                                    stylesModel.getHandle(styleList.currentIndex).childNodes[index].destroy()
//                                    fBlob.parentHandle = stylesModel.getHandle(styleList.currentIndex).childNodes[index]

//                                    fBlob.type = cmb_styleType.currentText
//                                    //                                var props = geoView.getStyleProperties(cmb_styleType.currentText)
//                                    //                                var md = {}
//                                    fBlob.metadata = {}
//                                    fBlob.create(cmb_styleType.currentText)
                                    //                                props.forEach(function(e) { md[e.name] = )

                                    //                                fBlob.metadata = {}

                                }
                            }

                        }
                        JSObjectView {
                            //                                text: 'teet %1'.arg( childs.ag.blob$type ).arg( childs.ag.blob$type )
                            Layout.fillWidth: true
                            property QtObject modelAgent: childs.ag
                            Repeater {
                                id: reprows
                                width: parent.width
                                JSObjectViewRow {
                                    width: parent.width

                                    text: modelData.name
                                    valueDelegate: Loader {
                                        width: 20
                                        height: 20
                                        property string propertyName: modelData.name

                                        //                                        onPropertyNameChanged: console.log( 'propertyName', propertyName )
                                        signal valueUpdated(var newValue)
                                        //                                property var modelValue: p_ed.modelAgent.blob$metadata[propertyName]
                                        property var modelValue: childs.ag.blob$metadata[propertyName]
                                        onValueUpdated: {
                                            var md = childs.ag.blob$metadata
                                            md[propertyName] = newValue
                                            childs.ag.blob$metadata = md
                                            console.log( 'AGENT PARENT', childs.ag.handle.parent.inNodes(), childs.ag.handle.parent.inNodes().length  )
                                            for( var j = 0; j < childs.ag.handle.parent.inNodes().length; ++j ) {
                                                var h = childs.ag.handle.parent.inNodes()[ j ]
                                                var ridx1 = h.rowIndex
                                                var ridx2 = h.parent.rowIndex
                                                console.log( 'HANDLE', h, ridx1, ridx2 )
                                                i_layers.objectAt(ridx2).update()//iGeomBuffer.objectAt(ridx1).style = childs.ag.handle.parent
                                            }

                                        }
                                        //                                source: "TableCell%1.qml".arg(modelData.type)
                                        source: "ValueEditors/VE_%1.qml".arg(modelData.type)
                                        //                                        source: "TableCell%1.qml".arg(modelData.type)
                                        //                                        onSourceChanged: console.log( 'sourceChanged:', source )
                                    }
                                }
                            }
                        }
                    }
                }



                /**/

            }
            RowLayout {
                ComboBox {
                    id: cmb_styleType
                    model: geoView.availableStyles
                }
                PicToolButton {
                    //                    text: "Add"
                    iconName: "add"
                    visible: cmb_styleType.currentIndex >= 0
                    onClicked: {
                        fBlob.parentHandle = stylesModel.getHandle(styleList.currentIndex)

                        fBlob.type = cmb_styleType.currentText
                        //                                var props = geoView.getStyleProperties(cmb_styleType.currentText)
                        //                                var md = {}
                        fBlob.metadata = {}
                        fBlob.create(cmb_styleType.currentText)
                        //                                props.forEach(function(e) { md[e.name] = )

                        //                                fBlob.metadata = {}

                    }
                }

            }
//            RowLayout {

////                PicTool
//                Button {
//                    text: "Add stroke"
////                    iconName: "add"
//                    visible: styleList.currentIndex !== -1
//                    onClicked: {
//                        fBlob.parentHandle = stylesModel.getHandle(styleList.currentIndex)
//                        fBlob.type = "CosmeticStroke"
//                        fBlob.metadata = {
//                            width: 1,
//                            color: '#888'
//                        }
//                        fBlob.create(fBlob.type)
//                    }
//                }
////                PicTool
//                Button {
//                    text: "Add fill"
////                    iconName: "add"
//                    visible: styleList.currentIndex !== -1
//                    onClicked: {
//                        fBlob.parentHandle = stylesModel.getHandle(styleList.currentIndex)
//                        fBlob.type = "SolidColorFill"
//                        fBlob.metadata = {
//                            color: Qt.hsla(Math.random(), 0.5, 0.7, 0.5).toString()
//                        }
//                        fBlob.create(fBlob.type)
//                    }
//                }
//            }

            /**
            CaptionLabel {
                Layout.fillWidth: true
                level: 3
                text: rightPanel.agent.blob$type
            }
            RowLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                ColumnLayout{
                    ScrollView {
                        Layout.fillHeight: true
                        Layout.minimumWidth: 240
                        ListView {
                            id: s_list
                            anchors.fill: parent
                            model: style_subList
                            delegate: JSObjectView {
                                id: p_ed
                                width: parent.width
//                                height: 200
                                height: __repeater.count * 20
                                Component.onCompleted: {
                                    console.log("d completed", height)
                                }

                                //                                    height: 100
                                property QtObject modelAgent: handle.agent
                                Repeater {
                                    id: rowmaker
                                    height: count * 20
                                    width: parent.width
                                    model: geoView.getStyleProperties(handle.type)
                                    Component.onCompleted: console.log(JSON.stringify(model))
                                    JSObjectViewRow {
                                        implicitHeight: 20
                                        text: modelData.name
                                        valueDelegate: Loader {
                                            width: 20
                                            height: 20
                                            property string propertyName: modelData.name
                                            signal valueUpdated(var newValue)
                                            property var modelValue: p_ed.modelAgent.blob$metadata[propertyName]
                                            onValueUpdated: {
                                                var md = p_ed.modelAgent.blob$metadata
                                                md[propertyName] = newValue
                                                p_ed.modelAgent.blob$metadata = md
                                            }
                                            source: "TableCell%1.qml".arg(modelData.type)
                                        }
                                    }
                                    onItemAdded: {
                                        console.log("added line", rowmaker.height, count)
                                        rowmaker.height = count * 20
                                        s_list.forceLayout()
                                    }
                                    onItemRemoved: s_list.forceLayout()
                                }

                            }
                        }

                    }

                    //                    TableView {
                    //                        id: styleTreeView
                    //                        Layout.fillHeight: true
                    //                        Layout.minimumWidth: 240
                    //                        model: styleTree
                    //                        TableViewColumn {
                    //                            role:"nodeId"
                    //                        }
                    //                        TableViewColumn {
                    //                            role:"typeName"
                    //                        }
                    //                    }
                    RowLayout {
                        ComboBox {
                            id: cmb_styleType
                            model: geoView.availableStyles
                        }
                        PicToolButton {
                            //                    text: "Add"
                            iconName: "add"
                            enabled: cmb_styleType.currentIndex >= 0
                            onClicked: {
                                fBlob.parentHandle = styleList.rootHandle

                                fBlob.type = cmb_styleType.currentText
                                //                                var props = geoView.getStyleProperties(cmb_styleType.currentText)
                                //                                var md = {}
                                fBlob.metadata = {}
                                fBlob.create("%1_%2".arg(cmb_styleType.currentText).arg(styleList.count))
                                //                                props.forEach(function(e) { md[e.name] = )

                                //                                fBlob.metadata = {}

                            }
                        }

                    }

                }
                JSObjectView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    //                    model: { "Название": "" }
                    Repeater {
                        width: parent.width
                        model: geoView.getStyleProperties(rightPanel.agent.blob$type)
                        JSObjectViewRow {
                            text: modelData.name
                            valueDelegate: Loader {
                                width: 20
                                height: 20
                                property string propertyName: modelData.name
                                signal valueUpdated(var newValue)
                                property var modelValue: rightPanel.agent.blob$metadata[propertyName]
                                onValueUpdated: {
                                    var md = rightPanel.agent.blob$metadata
                                    md[propertyName] = newValue
                                    rightPanel.agent.blob$metadata = md
                                }
                                source: "TableCell%1.qml".arg(modelData.type)
                            }
                        }
                    }
                }

                //                TableView {
                //                    id: props_table
                //                    model: geoView.getStyleProperties(rightPanel.agent.blob$type)
                //                    //                Component.onCompleted: console.log(rightPanel.styleElement.handle.metadata.type, geoView.getStyleProperties(rightPanel.styleElement.handle.metadata.type))
                //                    TableViewColumn {
                //                        title: "Параметр"
                //                        role: "name"
                //                    }

                //                    TableViewColumn {
                //                        title: "Значение"
                //                        delegate: Loader {
                //                            //                        height: 24
                //                            //                        rightPanel.agent
                //                            property string propertyName: styleData.value.name
                //                            signal valueUpdated(var newValue)
                //                            property var modelValue: rightPanel.agent.blob$metadata[propertyName]
                //                            //                        onModelValueChanged: console.log("modelvalue", JSON.stringify(modelValue))
                //                            onValueUpdated: {
                //                                //                            console.log("value", modelValue, JSON.stringify(agent.blob$metadata))
                //                                var md = rightPanel.agent.blob$metadata
                //                                md[propertyName] = newValue
                //                                rightPanel.agent.blob$metadata = md
                //                                //                            console.log("updated value", JSON.stringify(rightPanel.agent.blob$metadata))
                //                                //                            i_layers.objectAt(layerId).iGeomBuffer.objectAt(geomId).updateStyle(0, agent.blob$metadata)
                //                            }

                //                            source: "TableCell%1.qml".arg(styleData.value.type)
                //                        }
                //                    }


                //                }


            }
            /**/
        }
    }
}

