import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Dr.MetaGraph 1.0
import QtQml.Models 2.2

ColumnLayout {
    id: root
    property alias treeSelection: loader.treeSelection
    property alias nodeModelData: loader.objectData
    property bool modified: false
    Loader {
        id: loader
        property QtObject objectData
        property bool multiSelection
        property ItemSelectionModel treeSelection


        multiSelection: treeSelection.selectedIndexes.length > 1

        active: objectData != null || multiSelection
        Layout.fillWidth: true
//        asynchronous: true
        source: multiSelection
                ? "RP_Multi.qml"
                : objectData === null
                  ? ""
                  : aux.delegates[ objectData.typeName ] === undefined
                    ? "RP_Node.qml"
                    : aux.delegates[ objectData.typeName ].data.rightPanel
    }

    Connections {
        target: loader.item
        onDataModifiedChanged: {
            root.modified = loader.item.dataModified
            console.log("unsaved changes", loader.item.dataModified)
        }
        ignoreUnknownSignals: true

    }

    Item { Layout.fillHeight: true }
}
