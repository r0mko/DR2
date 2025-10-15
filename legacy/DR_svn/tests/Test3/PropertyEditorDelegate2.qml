import QtQuick 2.3
import DataReactor 1.0
import DataReactor.UI 1.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

UniDelegate {
    id: root
    anchors.fill: parent

    ColumnLayout{
        anchors.fill: parent
        spacing: 1
        SplitView {
            id: s
            Layout.fillWidth: true
            Layout.minimumHeight: 20
            Rectangle{
                id: propertyNameColumnWidth
                color: '#222'
                width: parent.width / 2
                Text{
                    anchors.fill: parent
                    anchors.leftMargin: 5
                    anchors.rightMargin: 5
                    verticalAlignment: Text.AlignVCenter
                    text: 'Property name'
                    color: '#ddd'
                }
            }
            Rectangle{
                color: '#222'
                Text{
                    anchors.fill: parent
                    anchors.leftMargin: 5
                    anchors.rightMargin: 5
                    verticalAlignment: Text.AlignVCenter
                    text: 'Property value'
                    color: '#ddd'
                }
            }
        }
        Rectangle{
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: '#222'
            ListWidget{
                id: listW
                anchors.fill: parent
                property alias cw: propertyNameColumnWidth.width
                model: mo.properties
//                spacing: 1

                MetaObject{
                    id: mo
                    object: root.parent.value
                }

                delegate: MetaPropertyDelegate {
        //            property alias modelValue: root.modelData.value
                }
            }
        }
    }
}
