import QtQuick 2.4
import Dr.MetaGraph 1.0
import Dr.UI 1.0

Item {
    property alias iconColor: icon.color
    property alias textColor: label.color
    property string nodeType

//    DRPicture {
////        id: icon
//        sourceData: aux.icons.hexagon.data
////        sourceData: aux.icons.hexagon_outline.data
//        anchors.fill: parent
//        anchors.margins: 0
//    }

    Image {
        id: icon
        property string color: "#000"
        sourceSize: Qt.size( parent.width, parent.height )
        source: "image://picture/aux/ui/icons/hexagon?color=%1".arg(color)
    }

//    DRPicture {
//        sourceData: aux.icons.hexagon_outline.data
//        anchors.fill: parent
//        anchors.margins: 1
//        color: aux.delegates[parent.nodeType].data.iconTextColor
//    }
//    DRPicture {
//        sourceData: aux.icons.hexagon_outline.data
//        anchors.fill: parent
//        anchors.margins: 1
//        color: aux.delegates[nodeType].data.iconColor
//    }

    Text {
        id: label
        anchors.centerIn: parent
//        verticalAlignment: Text.AlignVCenter
//        horizontalAlignment: Text.AlignHCenter
        text: nodeType ? nodeType[0] : ""
        minimumPointSize: 3

        font.pixelSize: parent.height / 1.8
//        font.bold: true
        font.weight: Font.Light
        font.family: "Helvetica Neue"
        renderType: Text.QtRendering
    }
}
