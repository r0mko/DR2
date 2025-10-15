import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtQml.Models 2.2
import Dr.UI 1.0
import Dr.MetaGraph 1.0
import Dr.Graphics 1.0
import Dr.Cartograph 1.0

SidebarPanel {
    id: nets_panel
    title: "Участки сети"
    property NodeChildrenModel model

    property var currentNetHandle
    property var addToSelection
    property var deselect
    property var clearSelection
    property var setCurrent
    property var selection

    ColumnLayout {
        anchors.fill: parent
        spacing: 0


        RowLayout {
            id: toolbar
            spacing: 1
//            Item { width: 1 }

            property color activeColor: "#2000ddff"

            VectorIconButton {
                id: b_addsegment
                action: a_addSegment
                height: 24
                buttonColor: checked ? toolbar.activeColor : "transparent"
                width: 24
                tooltip: "Добавить сегмент"
                iconData: aux.icons.crayon8.data
            }

            VectorIconButton {
                id: b_mergeSecs
                action: a_mergeSections
                height: 24
                buttonColor: checked ? toolbar.activeColor : "transparent"
                width: 24
                tooltip: "Слить участки"
                iconData: aux.icons.graph_inverted.data
            }


            VectorIconButton {
                id: b_settings
                action: a_sectionSettings
                height: 24
                buttonColor: checked ? toolbar.activeColor : "transparent"
                width: 24
                tooltip: "Настройки участка"
                iconData: aux.icons.gear6.data

            }


            VectorIconButton {
                id: b_addConnection
                action: a_addConnection
                height: 24
                buttonColor: checked ? toolbar.activeColor : "transparent"
                width: 24
                tooltip: "Добавить ввод питания"
                iconData: aux.icons.plug24.data
            }

            VectorIconButton {
                id: b_segsettings
                action: a_segmentSettings
                height: 24
                buttonColor: checked ? toolbar.activeColor : "transparent"
                width: 24
                tooltip: "Список сегментов"
                iconData: aux.icons.menu.data
            }

            VectorIconButton {
                id: b_removeSections
                action: a_removeSelSections
                height: 24
                width: 24
                //                flat: false
                iconColor: "#000"
                tooltip: "Удалить"
//                enabled:
                iconData: aux.icons.erase.data
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
            id: sections_table
            model: nets_panel.model
            Layout.fillWidth: true
            Layout.fillHeight:  true
            onCurrentRowChanged: currentNetHandle = model.getHandle(currentRow)

            onClicked: {
                m_scene.clearSelection()
                i_sections.objectAt(row).selected = true
            }

            onDoubleClicked: a_sectionSettings.trigger()

            //            onSelectionChanged:

            Component.onCompleted: {
                nets_panel.addToSelection = selection.select
                nets_panel.deselect = selection.deselect
                nets_panel.clearSelection = selection.clear
                nets_panel.selection = selection
                nets_panel.setCurrent = function(index) { currentRow = index }
            }

            selectionMode: SelectionMode.ExtendedSelection

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

        RowLayout {
            id: toolbar2
            Layout.fillWidth: true
            spacing: 1

            VectorIconButton {
                id: b_addTramSection
                action: a_addTramSection
                height: 24
                width: 24
                buttonColor: checked ? toolbar.activeColor : "transparent"

                DRPicture {
                    id: badge_tram
                    aspectRatio: root.aspectRatioMode
                    sourceData: aux.icons.minus.data
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    width: 12
                    height: 12
                    color: "#ff4000"
                }
//                color: "#000000"

                //                flat: false
                tooltip: "Добавить участок трамвайной сети"
                iconData: aux.icons.add.data
            }
            VectorIconButton {
                id: b_addTrollSection
                action: a_addTrollSection

                buttonColor: checked ? toolbar.activeColor : "transparent"
                height: 24
                width: 24

                DRPicture {
                    id: badge_troll
                    aspectRatio: root.aspectRatioMode
                    sourceData: aux.icons.equal.data
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    width: 12
                    height: 12
                    color: "#0080ff"
                }


                //                flat: false
                tooltip: "Добавить участок троллейбусной сети"
                iconData: aux.icons.add.data
            }

        }

        MessageDialog {
            id: dia_deleteSections
            property var sections:[]
            title: "Удалить участки"
            icon: StandardIcon.Question
            //            informativeText:
            standardButtons: StandardButton.Yes | StandardButton.Cancel
            onYes:  {
                sections.forEach(function(handle) { handle.destroy() })
            }
        }

        Action {
            id: a_mergeSections
            enabled: sections_table.selection.count > 1
            onTriggered: {
                var handles = []
                sections_table.selection.forEach(function(row) { handles.push(model.getHandle(row)) } )
                mergeSections(handles)
            }

        }

        Action {
            id: a_removeSelSections
            enabled: sections_table.selection.count > 0
            onTriggered: {
                var selectedSections = []
                sections_table.selection.forEach(function(row) {
                    var h = model.getHandle(row)
                    selectedSections.push(h.nodeId)
                    dia_deleteSections.sections.push(h)
                })
                dia_deleteSections.text = "Вы действительно хотите удалить выбранные участки?"
                dia_deleteSections.informativeText = selectedSections.join("\n")
                dia_deleteSections.open()
            }
        }

        function mergeSections(handles) {
            var outBounds = []
            var inBounds = []
//            ha

        }
    }

    
}
