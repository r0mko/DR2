import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import Dr.UI 1.0
//import Dr.MetaGraph 1.0

RP_Node {
    id: root
    CaptionLabel {
        Layout.fillWidth: true
        text: "Data"
        level: 3
    }
    ObjectEditor {
        id: dataview
        property var handle: objectData.handle
        editor.object: handle.data
        onAccepted: {
            handle.data = editor.getObject()
            dataModified = false
        }
        onRejected: {
            editor.fillModel(handle.data)
            dataModified = false
        }
        onModified: dataModified = true
    }


//    RowLayout {
//        PicButton {
//            action: a_apply
//        }
//        Button {
//            text: 'Отменить'
//            onClicked: {
//                oeditor.item.object = Qt.binding( function(){ return objectData.handle.data } )
//                root.dataModified = false
//            }
//        }
//    }
}

