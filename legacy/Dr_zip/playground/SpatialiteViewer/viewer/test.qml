import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0

import Dr.Cartograph 1.0
import Dr.MetaGraph 1.0
import QtQml.Models 2.2
import Dr.Graphics 1.0
import Dr.UI 1.0
import Dr.Paint 1.0

AAWindow {
    id: appWin
    //    title: qsTr("%1 - GISView".arg(main_model.fileName))
    color: "#ddd"
    samples: 64
    height: 900
    width: 1600
    Component.onCompleted: {
        show()
        main_model.init()
    }

    PaintBoard {
        id: pbrd
//        layer.enabled: true
//        layer.format: ShaderEffectSource.RGBA
//        layer.samplerName: "pbrd"

        anchors.fill: parent
        RowLayout {
            anchors.fill: parent
            anchors.margins: 3
            Rectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true

                color: "#000"

                Rectangle {
                    color: "#080"
                    x: 50
                    y: 80
                    width: 100
                    height: 60
                }


                Rectangle {
                    color: "#06f"
                    x: 250
                    y: 180
                    width: 50
                    height: 160
                }


                Rectangle {
                    id: hover
                    width: 20
                    height: 20
                    radius: 8
                    color: "#f93"
                    x: pbrd.hoverPos.x
                    y: pbrd.hoverPos.y
                    transform: Translate {
                        x: -hover.width / 2
                        y: -hover.height / 2
                    }
                }

                Loader {
                    id: gview
                    property GeoView view: item
                    Layout.fillHeight: true
                    //            Layout.fillWidth: true
                    active: spview.linked
                    //            anchors.fill: parent
                    sourceComponent: GeoView {
                        id: g_view
                        parent: gview
                        anchors.fill: parent
                        targetSRID: spview.handle.data.targetSRID
                    }
                }
            }

            ScrollView {
                Layout.fillHeight: true
                Layout.minimumWidth: 300
                frameVisible: true
                //            highlightOnFocus: true
                ListView {
                    id: s_list
                    anchors.fill: parent
                    //                anchors.margins: 1
                    clip: true
                    model: styles_model
                    delegate: Rectangle {
                        color: "#ccc"
                        border.color: "#aaa"
                        border.width: 1
                        anchors.leftMargin: -3
                        anchors.rightMargin: -3
                        width: parent.width
                        height: layout.implicitHeight + 1
                        ColumnLayout {
                            id: layout
                            spacing: 1
                            y: 1
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.leftMargin: 1
                            anchors.rightMargin: 1
                            //                        Layout.fillWidth: true
                            //                        width: parent.width

                            CaptionLabel {
                                text: handle.type
                                Layout.fillWidth: true
                                level: 4
                            }

                            JSObjectView {
                                Layout.fillWidth: true
                                id: p_ed
                                property QtObject modelAgent: handle.agent
                                Repeater {
                                    id: rowmaker
                                    //                        height: count * 20
                                    width: parent.width
                                    model: gview.item.getStyleProperties(handle.type)
                                    delegate:  JSObjectViewRow {
                                        width: parent.width
                                        text: modelData.name
                                        valueDelegate: Loader {
                                            //                                    width: 20
                                            property string propertyName: modelData.name
                                            signal valueUpdated(var newValue)
                                            property var modelValue: p_ed.modelAgent.blob$metadata[propertyName]
                                            onValueUpdated: {
                                                var md = p_ed.modelAgent.blob$metadata
                                                md[propertyName] = newValue
                                                p_ed.modelAgent.blob$metadata = md
                                            }
                                            source: "ValueEditors/VE_%1.qml".arg(modelData.type)
                                        }
                                    }
                                }
                            }


                        }

                    }



                }
            }

        }

    }


    MenuBar {
        Menu {
            title: "Файл"
            MenuItem { action: a_openModel }
        }
        Menu {
            id: styles_menu
            title: "Стили"
            MenuItem { action: a_openStyleEditor }
        }
    }


    Action {
        id: a_openModel
        text: "Открыть..."
        shortcut: "Ctrl+O"
        onTriggered: {
            fileselector.open()
        }
    }

    Action {
        id: a_openStyleEditor
        text: "Редактор стилей"
        //        enabled: !style_editor_loader.active
        //        onTriggered: style_editor_loader.active = true
    }

    FileDialog {
        id: fileselector
        selectExisting: true
        //        modality: Qt.NonModal
        folder: shortcuts.desktop
        nameFilters: ["MetaGraph model files (*.mgf)"]
        onAccepted: {
            main_model.file = fileUrl
            //            console.log("loading model", fileUrl.toString())
            main_model.loadModel(fileUrl)
            //            console.log("model loaded")
        }
    }

    FileDialog {
        id: saveFileselector
        folder: shortcuts.desktop
        //        modality: Qt.NonModal
        selectExisting: false
        nameFilters: ["MetaGraph model files (*.mgf)"]
        onAccepted: {
            var st = fileUrl.toString()
            var p = st.toString().lastIndexOf(".mgf")
            if (p < st.length - 4)
                fileUrl = Qt.resolvedUrl(st + ".mgf")
            main_model.file = fileUrl
            main_model.exportModel(fileUrl, main_model.rootIndex)
        }
    }

    SystemPalette { id: palette }

    GraphModel {
        id: main_model
        property url file
        property string fileName: file.toString().length ? file.toString().match(/(\w+)\..+$/)[1] : "untitled"
        property bool changed
        onDataChanged: changed = true
        //        onModelRootChanged: console.log(modelRoot)
        DataNodeFactory { id: fDataNode }
        NodeFactory { id: fNode }
        MetaNodeFactory { id: fMetaNode }
        CollectionFactory { id: fCollection }
        TypeBuilder { id: builder }
        BlobFactory { id: fBlob }
        RootNodeFactory { }

        NodeChildrenModel {
            id: model_ds
            mrl: "spview/datasources"
        }

        NodeChildrenModel {
            id: model_layers
            mrl: "spview/layers"
        }

        ModelWatcher {
            id: spview
            mrl: "spview"
        }

        NodeChildrenModel {
            id: cache_model
            mrl: "spview/cache"
        }

        NodeChildrenModel {
            id: styles_model
            mrl: "spview/styles"

        }

        function init() {
            loadModel("viewer/spv_base.mgf")
            fNode.parentHandle = getHandle("/spview")
            fNode.create("datasources")
            fNode.create("layers")
            fNode.create("cache")
            fBlob.parentHandle = styles_model.rootHandle
            var t = 60
            while (t--) {
                if (Math.random() <= 0.5) {
                    fBlob.type = "CosmeticStroke"
                    var col = Qt.hsla(Math.random(), 0.5, 0.7, 0.5)
                    fBlob.metadata = { "color": col.toString(), "width": 1 }
                } else {
                    fBlob.type = "LineStroke"
                    col = Qt.hsla(Math.random(), 0.5, 0.7, 0.5)
                    fBlob.metadata = { "color": col.toString(), "width": Math.round(Math.random() * 10), "offset": 0 }
                }
                fBlob.create("%1_%2".arg(fBlob.type).arg(styles_model.count))
            }

            //            console.log("Resolved:", Graph.resolve("node://main_model/spview/datasources"))
        }
    }






    //    Loader {
    //        id: style_editor_loader
    //        active: false

    //        sourceComponent: StyleEditor {
    //            Component.onCompleted: show()
    //            stylesModel: styles_model
    //            geoView: gview.item
    //            onClosing: style_editor_loader.active = false

    //        }
    //    }

    Weaver {
        id: i_styles
        index: styles_model.rootIndex
        typeRole: "blob$type"
        valueRole: "blob$metadata"
        onObjectAdded: if (parent)
                           parent.insertChild(object, index)
        onObjectRemoved: {
            console.log("Removed style", object, "parent", parent, index)
        }
        onValueChanged: {
            var handle = main_model.getHandle(index)
            //            var l = i_layers.objectAt(handle.inNodes()[0].parent.rowIndex)
            //            l.update()
            //            console.log("value changed", handle, instance, l)
        }
    }


}
