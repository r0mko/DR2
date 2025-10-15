import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.3
import Dr.Cartograph 1.0
import Dr.MetaGraph 1.0
import Dr.UI 1.0
Item {
    id: root
    property alias view: _table
    property NodeChildrenModel model
    property alias selection: _table.selection
    property alias layers: _table.layers

    signal layersSelected(var selection)
    Action {
        id: a_addLayers
        text: "Добавить..."
        onTriggered: addLayers_loader.active = true
    }
    Action {
        id: a_removeCurrent
        enabled: _table.currentRow >= 0
        text: "Удалить"
        onTriggered: {

            root.model.remove(_table.currentRow)
            _table.currentRow = -1
        }
    }

    ColumnLayout {
        id: view
        anchors.fill: parent
        spacing: 1
        property var currentHandle: _table.currentRow < 0 ? null : model.getHandle(_table.currentRow)
        LayersTree {
            id: _table
            model: root.model
            Layout.fillWidth: true
            Layout.fillHeight: true
            onDoubleClicked: requestSettings(view.currentHandle, layers.objectAt(_table.currentRow))
        }

        RowLayout {
            spacing: 1
            Item { width: 1 }

            VectorIconButton {
                id: b_addLayers
                action: a_addLayers
                height: 20
                width: 20
//                flat: false
                tooltip: "Добавить слои"
                iconData: aux.icons.add.data
            }

            VectorIconButton {
                id: b_removeLayers
                iconVPadding: 2
                iconHPadding: 2
                action: a_removeCurrent
                height: 20
                width: 20
//                flat: false
                iconColor: "#000"
                tooltip: "Удалить"
                iconData: aux.icons.erase.data

            }

                        Item { Layout.fillWidth: true }
            VectorIconButton {
                id: b_moveup
                width: 20
                height: 20
//                flat: false
                iconVPadding: 2
                iconHPadding: 2
                tooltip: "Переместить выше"
                iconData: aux.icons.move_up.data
                onClicked: {
                    var rows = []
                    _table.selection.forEach(function(e) {
                        rows.push(e + 1)
                        console.log("deselect", e, "select", e+2)
                    })
                    _table.selection.clear()
                    view.currentHandle.moveDown()
                    rows.forEach(function(e) {
                        _table.selection.select(e)
                    })
                }
            }

            VectorIconButton {
                id: b_movedown
                width: 20
                height: 20
//                flat: false
                iconVPadding: 2
                iconHPadding: 2
                tooltip: "Переместить ниже"
                iconData: aux.icons.move_down.data
                onClicked: {
                    var rows = []
                    _table.selection.forEach(function(e) {
                        rows.push(e - 1)
                    })
                    _table.selection.clear()
                    view.currentHandle.moveUp()
                    rows.forEach(function(e) {
                        _table.selection.select(e)
                    })
                }
            }
            VectorIconButton {
                id: b_layerSettings
                anchors.verticalCenter: parent.verticalCenter
//                flat: false
                width: 20
                height: 20
                enabled: _table.currentRow >= 0
                iconVPadding: 2
                iconHPadding: 2
                iconData: aux.icons.gear6.data
                onClicked: requestSettings(view.currentHandle, layers.objectAt(_table.currentRow))
            }
            Item { width: 1 }
        }

    }


    Loader {
        id: addLayers_loader
        property NodeChildrenModel dsModel: model_ds
        active: false
        sourceComponent: AddLayersDialog {
            id: dlg_addLayers
        }
    }


}

