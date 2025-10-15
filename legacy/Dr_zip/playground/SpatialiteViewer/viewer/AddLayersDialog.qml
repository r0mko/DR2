import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.3
import Dr.Cartograph 1.0
import Dr.MetaGraph 1.0
import Dr.UI 1.0

Window {
    id: dlg_addLayers
    modality: Qt.NonModal
    width: 250
    height: 500
    onClosing: addLayers_loader.active = false

    Component.onCompleted: {
        dscombo.model = addLayers_loader.dsModel
        show()
    }
    
    function addLayers(selection, datasource) {
        var layers = mainModel.getHandle("/spview/layers")
        selection.forEach(function(row) {
            fDataNode.parentHandle = layers
            var ld = fDataNode.beginCreate()
            var l = _list.model[row]
            ld.nodeId = l.tableName
            ld.data = {
                "tableName": l.tableName,
                "geometryColumn": l.geometryColumn,
                "srid": l.srid,
                "geometryType": l.geometryType,
                "geometryKind": l.geometryKind,
                "visible": true
            }
            mainModel.addEdge(ld, datasource, "db")
            fDataNode.endCreate()
//            fDataNode.parentHandle = ld
//            var gt = fDataNode.beginCreate()
//            gt.nodeId = l.tableName
//            gt.data = { where: '', count: 0 }
//            fDataNode.endCreate()
        })
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 1
        
        ComboBox {
            id: dscombo
            currentIndex: 0
            Layout.fillWidth: true
            textRole: "nodeId"
            onCurrentIndexChanged: {
                if (!_list)
                    return
                if (i_ds.count > currentIndex) _list.model = i_ds.objectAt(currentIndex).geometryLayers()
            }
            onModelChanged: if (i_ds.count > currentIndex) _list.model = i_ds.objectAt(currentIndex).geometryLayers()
        }
        
        TableView {
            id: _list
            selectionMode: SelectionMode.ExtendedSelection
            Layout.fillWidth: true
            Layout.fillHeight: true
            TableViewColumn {
                role: "tableName"
                title: "Слой"
                delegate: Label {
                    x: 4
                    text: styleData.value
                    color: styleData.textColor
                }
            }
            
            TableViewColumn {
                role: "geometryType"
                title: "Тип"
                width: 32
                resizable: false
                delegate: DRPicture {
                    sourceData: getIconSource(styleData.value)
                    color: styleData.textColor
                }
            }
        }
        
        RowLayout {
            Button {
                id: b_addall
                text: "Добавить все"
                enabled: _list.rowCount > 0
                onClicked: {
                    _list.selection.selectAll()
                    addLayers(_list.selection, model_ds.getHandle(dscombo.currentIndex))
                    dlg_addLayers.close()
                }
            }
            Button {
                id: b_add
                text: "Добавить"
                enabled:  _list.selection.count > 0
                onClicked: {
                    addLayers(_list.selection, model_ds.getHandle(dscombo.currentIndex))
                    //                            dlg_addLayers.close()
                }
                
            }
        }
        
    }
}
