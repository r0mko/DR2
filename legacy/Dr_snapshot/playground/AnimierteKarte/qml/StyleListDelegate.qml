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

Rectangle {
    id: row
    property var geomModelItem
    height: 40
    width: parent.width
    color: ListView.isCurrentItem ? "#8000aaff" : "white"
    
    RowLayout {
        anchors.fill: parent
        Item {
            Layout.fillHeight: true
            Layout.minimumWidth: height
//            StylePreviewItem {
//                anchors.fill: parent
//                style: i_styles.instance(handle.modelIndex)
//                shapeSides: 6
//            }
//            Rectangle {
//                anchors.fill: parent
//                anchors.margins: 4
////                border.color: "#888"
//                color: "#eee"
//                radius: 8
//            }
        }

        Label {
            text: nodeId
        }
        Item { Layout.fillWidth: true }

    }
    MouseArea {
        anchors.fill: parent
        onPressed: parent.ListView.view.currentIndex = index
    }
}
