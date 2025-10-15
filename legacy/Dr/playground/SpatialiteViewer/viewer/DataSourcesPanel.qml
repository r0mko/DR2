import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import Dr.Cartograph 1.0
import Dr.MetaGraph 1.0
import QtQml.Models 2.2
import Dr.Graphics 1.0
import Dr.UI 1.0

SidebarPanel {
    id: panel_ds
    property NodeChildrenModel datasourcesModel
    property Instantiator datasources

    DataSourcesView {
        id: view_ds
        property SpatiaLiteDB currentDB: datasources.objectAt(view.currentRow)
        
        opacity: model_ds.linked ? 1.0 : 0.5
        Layout.fillWidth: true
        //                        Layout.fillHeight: true
        model: panel_ds.datasourcesModel
//        onCurrentDBChanged: {
//            console.log(currentDB.extent(3752))
//            var ls = currentDB.geometryLayers()
//        }
    }
    
}
