import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import Dr.UI 1.0

RP_Node {

    id: root

    CaptionLabel {
        Layout.fillWidth: true
        text: '<b>%1</b> metadata:'.arg(objectData.blob$type)
        level: 3
    }

    properties: [
        { "Размер": "%1 байт".arg(objectData.blob$size) },
        { "Тип BLOB": objectData.blob$type }
    ]


    ObjectEditor {
        property var handle: objectData.handle
        editor.object: handle.metadata
        onAccepted: {
            handle.metadata = editor.getObject()
            dataModified = false
        }
        onRejected: {
            editor.fillModel(handle.metadata)
            dataModified = false
        }
        onModified: dataModified = true
    }
    TextField {
//        Component.onCompleted: text = objectData.blob$type
        text: objectData.blob$type
        onAccepted: {
            objectData.blob$type = text
            focus = false
        }
    }
}

