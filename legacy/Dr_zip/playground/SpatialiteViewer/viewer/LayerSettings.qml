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
    id: root
    width: 800
    height: 800
    //    modality: Qt.ApplicationModal
    flags: Qt.Tool
    color: "#eee"

    function open(handle, geoLayer) {
        main.handle = handle
        main.geoLayer = geoLayer
        console.log("open settings for", handle, geoLayer, geoLayer.iGeomBuffer)
        //        loader.active = true
        title = "Параметры слоя %1".arg(geoLayer.table)
        show()
    }
    ColumnLayout {
        id: main
        anchors.fill: parent
        anchors.margins: 8
        spacing: 4
        property GeoLayer geoLayer: null
        property var handle
        property SpatiaLiteDB db: geoLayer.dataSource


        CaptionLabel {
            Layout.fillWidth: true
            text: "Свойства"
            level: 3
        }

        JSObjectView {
            Layout.fillWidth: true
            model: main.handle ?
                       [
                           { "Таблица": main.handle.data.tableName },
                           { "SRID": main.handle.data.srid },
                           { "Число строк": main.geoLayer.entityCount },
                           { "Геометрия": main.geoLayer.geometryBuffers.length}
                       ] : []
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 1
            //                layoutDirection: Qt.RightToLeft
            PicToolButton {
                iconVPadding: 1
                iconHPadding: 1
                tooltip: "Добавить геометрию"
                iconName: "add"
                onClicked: dia_addgeom.show()
            }
            PicToolButton {
                iconVPadding: 1
                iconHPadding: 1
                enabled: table_geoms.selection.count > 0
                tooltip: "Удалить"
                iconName: "waste"
                onClicked: {
                    var idxList = []
                    table_geoms.selection.forEach(function(row) {
                        console.log("remove", row)
                        idxList.push(row)
                        //                            buffers_model.remove(row)
                    })
                    buffers_model.remove(idxList)
                    table_geoms.selection.clear()
                }
            }

            PicToolButton {
                tooltip: "Стиль"
                iconVPadding: 2
                iconHPadding: 2
                iconName: "paintbrush3"
                enabled: table_geoms.selection.count > 0
                onClicked: {
                    var objects = []
                    table_geoms.selection.forEach(function(e){
                        var gb = main.geoLayer.iGeomBuffer.objectAt(e)
                        console.log("element", e, gb)
                        objects.push({ instance: gb, index: e })

                    })
                    geom_style_dialog.run(objects)
                }
            }
            PicToolButton {
                tooltip: "Создать граф"
                property GeomBuffer gb: main.geoLayer.iGeomBuffer.objectAt(table_geoms.currentRow)
                iconVPadding: 2
                iconHPadding: 2
                iconName: "graph1"
                enabled: table_geoms.selection.count === 1
                onClicked: {
//                    var gb = main.g   eoLayer.iGeomBuffer.objectAt(table_geoms.currentRow)
                    fNode.parentHandle = main_model.getHandle("/spview")
                    var h = fNode.create("road_graph")
                    var tr = gb.combinedTransform()
                    fPoint.parentHandle = h
                    for (var i = 0; i < gb.queryData.count; ++i) {
                        var r = gb.queryData.row(i)
//                        console.log("Row", i, JSON.stringify(r))
                        fPoint.addPolygon(r.polygon, r, tr)
                    }
                }
            }
            PicToolButton {
                tooltip: "Создать граф2"
                property GeomBuffer gb: main.geoLayer.iGeomBuffer.objectAt(table_geoms.currentRow)
                iconVPadding: 2
                iconHPadding: 2
                iconName: "graph3"
                enabled: table_geoms.selection.count === 1
                onClicked: {
//                    var gb = main.geoLayer.iGeomBuffer.objectAt(table_geoms.currentRow)
                    fNode.parentHandle = main_model.getHandle("/spview")
                    var h = fNode.create("road_graph")
                    var tr = gb.combinedTransform(true)
                    fPoint.parentHandle = h
                    var pols = gb.buildGraph()
                    for (var i = 0; i < pols.length; ++i) {
//                        var r =
                        //                        console.log("Row", i, JSON.stringify(r))
                        fPoint.addPolygon(pols[i], {}, tr)
                    }
                }
            }
            PicToolButton {
                tooltip: "Cache"
                iconVPadding: 2
                iconHPadding: 2
                checkable: true
//                checked: cache.length > 0
                width: 20
                height: 20
                iconData: checked ? aux.icons.waste.data : aux.icons.binary_1.data
                //            flat: false
                onClicked: {
                    if (checked) {
                        for (var i = 0; i < buffers_model.count; ++i) {
                            var g = main.geoLayer.iGeomBuffer.objectAt(i)
                            var handle = buffers_model.getHandle(i)
                            main.saveCache(g, handle)
                        }
                    } else {
                        for (var i = 0; i < buffers_model.count; ++i) {
                            handle = buffers_model.getHandle(i)
                            handle.outNodes("cache").forEach(function(n) { n.destroy(); })
                        }
                    }
                }
            }
        }

        ColumnLayout {
            id: cont_geoms
            Layout.minimumWidth: 300
            Layout.fillHeight: true
            property int entityCount: 0
            spacing: 1

            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true
                RowLayout {
                    anchors.fill: parent
                    TableView {
                        id: table_geoms
                        anchors.fill: parent
                        frameVisible: true
                        selectionMode: SelectionMode.ExtendedSelection
                        model: buffers_model
                        rowDelegate: Rectangle {
                            height: 25
                            color: styleData.selected ? "#8000aaff" : "transparent"
                        }

                        TableViewColumn {
                            title: "Геометрия"
                            role: "handle"
                            delegate: GeomBufferDelegate {
                                handle: styleData.value
                                buffer: buf_watcher.object
                                InstanceWatcher {
                                    id: buf_watcher
                                    instantiator: main.geoLayer.iGeomBuffer
                                    index: styleData.row
                                }
                            }
                        }
                    }
                }
            }
        }


        function addGeometry(clause, count, nodeId, attrs) {
            fDataNode.parentHandle = main.handle
            var gt = fDataNode.beginCreate()
            gt.nodeId = nodeId
            gt.data = { where: clause, count: count, attrlist: attrs ? attrs : "" }
            fDataNode.endCreate()
        }

        function addGeomsByClass(column) {
            var q = "SELECT COUNT(%1) as entityCount, %1 AS class FROM %2 GROUP BY class ORDER BY entityCount DESC".arg(column).arg(main.handle.data.tableName)
            console.log(q, column, main.geoLayer.table)
            var geoms = db.query(q)
            fDataNode.parentHandle = main.handle
            geoms.forEach(function(row) {
                var gt = fDataNode.beginCreate()
                gt.nodeId = row.class
                gt.data = { where: '%1="%2"'.arg(column).arg(row.class), count: row.entityCount }
                if (styles_model.hasChild(row.class)) {
                    var sh = styles_model.getHandle(row.class)
                    console.log("adding link to style", row.class, gt, sh)
                    main_model.addEdge(gt, sh, "style")
                }
                fDataNode.endCreate()
            })
        }

        function saveCache(buffer, handle) {
            var c = handle.outNodes("cache")
            if (!c.length) {
                var ch = main_model.getHandle("/spview/cache")
                fBlob.parentHandle = ch
                console.log("saving cache to", fBlob.parentHandle)
                fBlob.metadata = { table: buffer.layer.table, where: handle.data.where, srid: buffer.layer.srid }
                fBlob.data = buffer.getCache()
                var cnode = fBlob.create("cache_%1".arg(buffer.layer.table))
                main_model.addEdge(handle, cnode, "cache")
            }
        }


        RowLayout {
            Layout.fillWidth: true
            //                layoutDirection: Qt.RightToLeft
            Item {
                Layout.fillWidth: true
            }

            Button {
                text: "Закрыть"
                onClicked: root.close()
            }
        }
    }

    Window {
        id: geom_style_dialog
        //        standardButtons: StandardButton.Ok | StandardButton.Cancel
        modality: Qt.ApplicationModal
        color: "#eee"
        flags: Qt.Tool
        property var geoms: []
        height: 320
        width: 200
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 8
            CaptionLabel {
                Layout.fillWidth: true
                level: 4
                text: "Стиль для %1 геометрий".arg(geom_style_dialog.geoms.length)
            }

            ComboBox {
                Layout.fillWidth: true
                model: styles_model
                textRole: "nodeId"
            }
            ScrollView {
                Layout.fillHeight: true
                Layout.fillWidth: true
                ListView {
                    id: styles_view
                    anchors.fill: parent
                    model: styles_model
                    delegate: StyleListDelegate {
                        geomModelItem: handle.inNodes()[0]
                    }
                }
            }
            RowLayout {
                PicToolButton {
                    width: 40
                    iconName: "check"
                    enabled: styles_view.currentIndex >= 0
                    onClicked: {
                        var sh = styles_model.getHandle(styles_view.currentIndex)
                        geom_style_dialog.geoms.forEach(function(e) {
                            var handle = buffers_model.getHandle(e.index)
                            var stlink = handle.outNodes("style")
                            if (stlink.length > 0) stlink.forEach(function(e) {
                                e.destroy()
                            })
                            main_model.addEdge(handle, sh, "style")
                            console.log("added link to style", sh)
                            e.instance.applyStyle()
                        })
                        geom_style_dialog.close()
                    }
                }
                PicToolButton {
                    width: 40
                    iconName: "cross"
                    onClicked: geom_style_dialog.close()
                }
            }
        }
        function run(geomList) {
            geoms = geomList
            showNormal()
        }
    }


    AddGeometriesDialog {
        id: dia_addgeom
    }

    NodeChildrenModel {
        id: buffers_model
        rootIndex: main.handle.modelIndex
    }


    //    }

}

