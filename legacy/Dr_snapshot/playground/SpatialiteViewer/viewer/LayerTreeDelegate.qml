import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import Dr.MetaGraph 1.0
import Dr.UI 1.0
import Dr.Cartograph 1.0

Item {
    id: root
//    height: 30
    property GeoLayer layerItem
//    property var handle: styleData.value
    property var handle: styleData.value
    Rectangle {
        width: parent.width * (layerItem ? layerItem.progress : 1)
        color:  layerItem ? (layerItem.isValid ? "#ddd" : "#500") : "#888"
        height: parent.height
        visible: layerItem != null
    }

    Rectangle {
        anchors.fill: parent
        visible: styleData.selected
        color: "#60009dff"
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 3
        spacing: 1
        VectorIconButton {
            id: b_visible
            anchors.verticalCenter: parent.verticalCenter
            checkable: true
//            flat:false
            buttonColor: "#30000000"
            checked: !handle.data.visible
            iconVPadding: 2
            iconHPadding: 2
            width: 22
            height: 22
            iconData: layerItem.visible ? aux.icons.visible.data : aux.icons.hidden.data
            onClicked: layerItem.visible = !checked
            iconColor: checked ? "#80000000" :"#000"
//            DRPicture {
//                anchors.fill: parent
//                anchors.margins: 2
//                sourceData: aux.icons.deny.data
//                visible: !b_visible.checked
//                color: "#800000"
//            }
        }


        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            ColumnLayout {
                anchors.fill: parent
                spacing: 0
                Label {
                    text: handle.data.tableName
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideMiddle
                }
            }
        }


        Item {
            width: height
            anchors.top: parent.top
            anchors.bottom: parent.bottom
//            Rectangle {
//                anchors.fill: parent
//                anchors.margins: 2
//                color: "#555"
//                radius: 4
//            }
            GeometryIcon {
                geometryType: handle.data.geometryType
            }

        }



    }
}

