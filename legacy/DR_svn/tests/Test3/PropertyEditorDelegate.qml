import QtQuick 2.3
import DataReactor.UI 1.0

UniDelegate {
    id: root
    anchors.fill: parent

    ListWidget{
        id: listW
        anchors.fill: parent
        model: mo.properties
        spacing: 1
        onWidthChanged: console.log("list", width, height)

        MetaObject{
            id: mo
            object: root.parent.value
        }

        delegate: MetaPropertyDelegate {
//            property alias modelValue: root.modelData.value
        }
    }
}
