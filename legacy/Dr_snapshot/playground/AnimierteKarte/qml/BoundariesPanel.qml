import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Window 2.2

import Dr.Graphics 1.0
import Dr.UI 1.0
import Dr.MetaGraph 1.0
import Dr.Cartograph 1.0

SidebarPanel {
    id: bounds_panel
    title: "Секционные изоляторы"
    property var currentBoundaryHandle
    property EdgeModel model
    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            id: toolbar
            spacing: 1
            //            Item { width: 1 }

            property color activeColor: "#2000ddff"


            VectorIconButton {
                id: b_move
                action: a_moveBoundary
                height: 24
                buttonColor: checked ? toolbar.activeColor : "transparent"
                width: 24
                iconVPadding: 1
                iconHPadding: 1
                iconData: aux.icons.horizontal_move.data
            }

            VectorIconButton {
                id: b_removeSections
                height: 24
                width: 24
                iconColor: "#000"
                tooltip: "Удалить"
                iconData: aux.icons.erase.data
            }

            VectorIconButton {
                id: b_split
                action: a_split
                height: 24
                buttonColor: checked ? toolbar.activeColor : "transparent"
                width: 24
                tooltip: "Вставить разъединитель"
                iconData: aux.icons.break.data
            }

            //            VectorIconButton {
            //                id: b_layerSettings
            //                anchors.verticalCenter: parent.verticalCenter
            //                //                flat: false
            //                width: 24
            //                height: 24
            //                iconVPadding: 2
            //                iconHPadding: 2
            //                iconData: aux.icons.gear6.data
            //                onClicked: requestSettings(view.currentHandle, layers.objectAt(_table.currentRow))
            //            }
            Item { width: 1 }
        }


        TableView {
            id: bounds_table
            model: bounds_panel.model
            Layout.fillWidth: true
            Layout.fillHeight:  true
            onCurrentRowChanged: currentBoundaryHandle = bounds_panel.model.getHandle(currentRow)
            onClicked: i_bounds.objectAt(row).selected = true
            selectionMode: SelectionMode.SingleSelection

            TableViewColumn {
                role: "nodeId"
            }
            //        TableViewColumn {
            //            role: "blob$metadata"
            //            delegate: Rectangle {
            //                anchors.fill: parent
            //                ColorDialog {
            //                    id: colorpicker
            //                    color: styleData.value.color
            //                    //                                onAccepted:
            //                }
            //                MouseArea {
            //                    anchors.fill: parent
            //                    onClicked: colorpicker.open()

            //                }
            //            }
            //        }
        }


    }
}
