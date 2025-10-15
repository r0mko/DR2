import QtQuick 2.5
import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import QtQml.Models 2.2
import Dr.MetaGraph 1.0
import Dr.UI 1.0
import "../treeview"


ColumnLayout {
    id: root
    anchors.fill: parent
    Layout.fillHeight: true

    property ItemSelectionModel sel: treeSelection
    property var idxList: sel.selectedIndexes
//    sel.onSelectedIndexesChanged:
    onIdxListChanged: {
        var ret = []
        sel.selectedIndexes.forEach(function(e) {
            ret.push(dasModel.getHandle(e))
//            ret.push(e)
        })
        list.handleModel = ret
    }

    Rectangle {
        height: 32
        clip: true
        Layout.fillWidth: true
        color: "#aacaec"
        RowLayout {
            anchors.fill: parent
            anchors.margins: 4
            spacing: 0
            NodeIcon {
                Layout.fillHeight: true
                Layout.minimumWidth: height
                nodeType: "?"
                iconColor: "#fff"
                textColor: "#888"
            }

            Label {
                font.pixelSize: 20
                anchors.verticalCenter: parent.verticalCenter
                font.family: "Helvetica Neue"
                font.weight: Font.Light
                text: "Multi selection: %1 nodes".arg(sel.selectedIndexes.length)
            }
            Item { Layout.fillWidth: true }
        }
    }
    GroupBox {
        Layout.fillHeight: true
        Layout.fillWidth: true

        TableView {
            anchors.fill: parent
            id: list
            property var handleModel: []

            model: handleModel

            itemDelegate: TreeNodeDelegate {
                handle: styleData.value
                nodeId: handle.nodeId
            }
            TableViewColumn {
                title: "Nodes"

            }
        }
    }
}
