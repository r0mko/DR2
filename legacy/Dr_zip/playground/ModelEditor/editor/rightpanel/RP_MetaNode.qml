import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import Dr.UI 1.0
import "../treeview"

RP_Node {
    CaptionLabel {
        Layout.fillWidth: true
        text: 'MetaNode specific stuff'
        level: 3
    }
    ListView {
        Layout.fillWidth: true
        Layout.minimumHeight: 300
        model: objectData.handle.members
        delegate: Rectangle {
            height: 18
            width: 200
            RowLayout {
                id: _layout
                anchors.fill: parent
                spacing: 0
                NodeIcon {
                    id: icon
                    Layout.fillHeight: true
                    Layout.minimumWidth: height
                    iconColor: aux.delegates[modelData.typeName].data.iconColor
                    textColor: aux.delegates[modelData.typeName].data.iconTextColor
                    nodeType: modelData.typeName
                }

                Text {
                    Layout.fillWidth: true
                    text: modelData.nodeId
                }


                Item {
                    id: _padding
                    width: 4
                }
            }
        }
    }
//    Button {
//        text: 'test' + objectData.handle.members.length
//        onClicked: {
//            objectData.handle.addMember( objectData.handle )
//        }
//    }
}

