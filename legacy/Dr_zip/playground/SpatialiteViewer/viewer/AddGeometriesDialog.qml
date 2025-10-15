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
    id: dia_addgeom
    title: "Добавить геометрию"
    modality: Qt.ApplicationModal
    color: "#ddd"
    width: 800
    height: 500
    
    ColumnLayout {
        anchors.fill: parent
        anchors.bottomMargin: 8
        anchors.topMargin: 8
        anchors.leftMargin: 8
        anchors.rightMargin: 8
        CaptionLabel {
            Layout.fillWidth: true
            text: "Классифицировать по столбцу"
            level: 3
        }
        RowLayout {
            Layout.fillWidth: true
            spacing: 1
            Label { text: "Столбец" }
            
            ComboBox {
                id: cmb_selectcolumn
                Layout.fillWidth: true
                textRole: "column_name"
                model: cols.reply
                onCurrentTextChanged: categories.clear()
            }
            VectorIconButton {
                width: 20
                height: 20
                flat: true
                iconColor: "#333"
                iconData: aux.icons.cross.data
                radius: 4
                onClicked: cmb_selectcolumn.currentIndex = -1
                //                                onClicked: main.addGeomsByClass(cmb_selectcolumn.currentText)
                //                                onClicked: main.addGeomsByClass(cmb_selectcolumn.currentText)
            }
        }
        
        RowLayout {
            Layout.fillWidth: true
            
            VectorIconButton {
                width: 32
                height: 32
                //                    square_rounded
                //                    iconData: aux.icons.ui40.data
                iconData: aux.icons.rect_dotted.data
                iconColor: "#555"
                tooltip: "Сгруппировать и добавить"
                //                    aspectRatioMode: Qt.IgnoreAspectRatio
                //                    flat: false
                //                    radius: 4
                iconVPadding: 1
                iconHPadding: 1
                DRPicture {
                    anchors.fill: parent
                    sourceData: aux.icons.layers_2.data
                    anchors.margins: 5
                    color: "#555"
                }
                onClicked: dia_addgeom.addGroupGeom()
                
            }
            VectorIconButton {
                id: b_add_separate
                
                width: 32
                height: 32
                iconVPadding: 1
                iconHPadding: 2
                //                    square_rounded
                iconData: aux.icons.layers_3.data
                tooltip: "Добавить по отдельности"
                iconColor: "#555"
                Rectangle {
                    anchors.fill: count
                    anchors.topMargin: -1
                    anchors.bottomMargin: -1
                    anchors.leftMargin: -3
                    anchors.rightMargin: -3
                    color: "#058"
                    border.color: "#eee"
                    border.width: 1
                    radius: 6
                }
                
                Text {
                    id: count
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.rightMargin: 4
                    anchors.bottomMargin: 2
                    verticalAlignment: Text.AlignBottom
                    horizontalAlignment: Text.AlignRight
                    color: "#fff"
                    font.pixelSize: 8
                    font.bold: true
                    property int catCount: cat_list.selection.count > 0 ? cat_list.selection.count : categories.count
                    text: catCount > 100 ? "100+" : catCount
                }
                onClicked: {
                    if (count.catCount > 100) {
                        count_warning.open()
                        return
                    }
                    dia_addgeom.addSeparateGeoms()
                }
                
                MessageDialog {
                    id: count_warning
                    icon: StandardIcon.Warning
                    text: "Внимание"
                    informativeText:  "Вы уверены, что хотите добавить %1 слоёв?".arg(count.catCount)
                    standardButtons: StandardButton.Ok | StandardButton.Cancel
                    onAccepted: dia_addgeom.addSeparateGeoms()
                }
                
                
            }
            
            VectorIconButton {
                width: 32
                height: 32
                iconVPadding: 5
                iconHPadding: 5
                enabled: cat_list.selection.count > 0
                iconData: aux.icons.rect_dashed.data
                //                    iconData: aux.icons.rect_dotted.data
                iconColor: "#555"
                tooltip: "Очистить выделение"
                onClicked: cat_list.selection.clear()
                //                    aspectRatioMode: Qt.IgnoreAspectRatio
                //                    flat: false
                //                    radius: 4
                //                    iconVPadding: 2
                //                    iconHPadding: 2
                DRPicture {
                    opacity: cat_list.selection.count > 0 ? 1.0 : 0.5
                    sourceData: aux.icons.deny.data
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.rightMargin: 0
                    anchors.bottomMargin: 0
                    width: 20
                    height: 20
                    color: "#855"
                }
            }

        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            TableView {
                id: cat_list
                Layout.fillWidth: true
                Layout.fillHeight: true
                selectionMode: SelectionMode.ExtendedSelection
                model: categories.reply
                onRowCountChanged: resizeColumnsToContents()

                TableViewColumn {
                    role: "cat"
                    title: "Столбец"
                    elideMode: Text.ElideMiddle
                }
                TableViewColumn {
                    role: "cnt"
                    title: "Количество"
                }
            }

            TableView {
                id: extra_attrs
                Layout.minimumWidth: 300
                Layout.fillHeight: true
                frameVisible: true
                selectionMode: SelectionMode.MultiSelection
                model: cols.reply
                alternatingRowColors: false
                onRowCountChanged: resizeColumnsToContents()
                function getColumns() {
                    var sc = []
                    selection.forEach(function(e) {
                        sc.push(cols.row(e).column_name)
                    })
                    console.log(sc.join(" "))
                    return sc.join(" ")
                }

                TableViewColumn {
                    role: "column_name"
                    title: "Столбец"
                    elideMode: Text.ElideMiddle
                }
                TableViewColumn {
                    title: "Тип"
                    role: "int_double_text_null"
                    elideMode: Text.ElideMiddle
                    delegate: Row {
                        id: row
                        anchors.fill: parent
                        property var datacount: styleData.value.split(":")
                        property var datatypes: styleData.role.split("_")
//                        onDatacountChanged: console.log(datacount)
                        onDatacountChanged: {
                            var t = 0
                            datacount.forEach(function(e) { t += Number(e) })
                            total = t
                            bars.model = row.datacount
                        }

                        property real total
                        Repeater {
                            id: bars
                            model: null
                            height: parent.height
                            delegate: Rectangle {
                                property var typecolor: {
                                    switch(row.datatypes[index]) {
                                    case 'int':
                                        return "#84b3cf"
                                    case 'double':
                                        return "#9184cf"
                                    case 'text':
                                        return "#e8c515"
                                    case 'null':
                                        return "#a8a8a8"
                                    }
                                    return "#fff"
                                }
                                color: styleData.selected ? "transparent" : typecolor
                                border.color: Qt.lighter(typecolor, 1.2)
                                border.width: styleData.selected ? 1 : 0
//                                radius: 0
                                height: 18
                                width: Number(modelData) / row.total * row.width
                                Label {
                                    color: styleData.textColor
                                    font.pixelSize: 12
                                    anchors.fill: parent
                                    verticalAlignment: Text.AlignVCenter
                                    horizontalAlignment: Text.AlignHCenter
                                    text: parent.height > 0 ? row.datatypes[index] : ""
                                    elide: Text.ElideMiddle
                                    renderType: "QtRendering"
                                }

                            }
                        }

//                        text: {
//                            var ret = []
//                            datacount.forEach(function(e,i) {
//                                if (e>0) {
//                                    ret.push(datatypes[i])
//                                }
//                            })
//                            return ret.join(" ")
//                        }

//                        color: styleData.textColor
                    }
                }
//                TableViewColumn {
//                    role: "cnt"
//                    title: "Количество"
//                }
            }

        }

        SpatialQuery {
            id: cols
            active: root.visible
            query: "SELECT column_name, integer_values || ':' || double_values || ':' || text_values || ':' || null_values AS int_double_text_null FROM vector_layers_field_infos WHERE table_name='%1' AND (integer_values > 0 OR double_values > 0 OR text_values > 0)".arg(main.geoLayer.table)
            database: main.db
        }
        
        SpatialQuery {
            id: categories
            property string column: cmb_selectcolumn.currentText
            active: root.visible
            //                            query: "SELECT DISTINCT(%1) AS cat FROM %2".arg(column).arg(main.geoLayer.table)
            query: "SELECT COUNT(%1) as cnt, %1 as cat FROM %2 GROUP BY %1 ORDER BY cnt DESC".arg(column).arg(main.geoLayer.table)
            database: main.db
        }
        //                        Item {  }
    }
    

    function addSeparateGeoms() {
        var indexList = []
        var attrs = extra_attrs.getColumns()
        if (cat_list.selection.count > 0)
            indexList = cat_list.selection
        else for (var i = 0; i < categories.count; ++i)
                indexList.push(i)
        indexList.forEach(function(e) {
            var clause = "%1=\"%2\"".arg(cmb_selectcolumn.currentText).arg(categories.row(e).cat)
            main.addGeometry(clause, categories.row(e).cnt, "%1:%2".arg(main.geoLayer.table).arg(categories.row(e).cat), attrs)
        })
    }
    
    function addGroupGeom() {
        var indexList = []
        if (cat_list.selection.count > 0)
            indexList = cat_list.selection
        else {
            main.addGeometry("", main.geoLayer.entityCount, main.geoLayer.table, extra_attrs.getColumns())
            return
        }
        
        var valList = []
        var cList = []
        var count = 0
        indexList.forEach(function(e) {
            valList.push("\"%1\"".arg(categories.row(e).cat))
            cList.push(categories.row(e).cat)
            count += categories.row(e).cnt
        })
        var id = "%1:%2".arg(main.geoLayer.table).arg(cList.join("|"))
        if (valList.length === 0) {
            main.addGeometry("", main.geoLayer.entityCount, main.geoLayer.table, extra_attrs.getColumns())
        } else {
            var clause = "%1 IN (%2)".arg(cmb_selectcolumn.currentText).arg(valList.join(", "))
            main.addGeometry(clause, count, id, extra_attrs.getColumns())
        }
    }
    
}
