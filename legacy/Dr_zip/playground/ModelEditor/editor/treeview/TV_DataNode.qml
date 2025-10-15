import QtQuick 2.5
import QtQuick.Controls 1.4
TV_Node {
    actions: [
        Action {
            id: a_duplicateDataNode
            text: qsTr("Дублировать")
//            shortcut: 'Ctrl+D'
            onTriggered: {
//                var idx = tree_view.activeIndex
//                var handle = graphModel.getHandle(tree_view.activeIndex)
                f_DataNode.parent = styleData.value.parent
                f_DataNode.data = styleData.value.data
                f_DataNode.create( styleData.value.nodeId + '_copy' )
            }
        }
    ]
}

