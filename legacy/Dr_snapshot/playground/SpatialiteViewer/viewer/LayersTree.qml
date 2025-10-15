import QtQuick 2.5
import QtQuick.Controls 1.4
import Dr.MetaGraph 1.0
import Dr.UI 1.0


TableView {
    id: tree_view
    __listView.verticalLayoutDirection: ListView.BottomToTop


    property Instantiator layers
    rowDelegate: Rectangle {
//        color: "#ddd"
        height: 25
    }

    TableViewColumn {
        title: "Слои"
        role: "handle"
        delegate: LayerTreeDelegate {
            layerItem: layers.objectAt(styleData.row)
        }
    }

}

