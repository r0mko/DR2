import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import Dr.QRPC 1.0
import DataReactor.Log 1.0
import QtQuick.Window 2.0

Window {
    id: win
    property QtObject logger
    color: "#eee"
    width: form.width
    height: form.height
    Column {
        id: form
        width: 400
        FormLine {
            id: type
            label: "Type:"
            text: "2"
            width: parent.width
        }
        FormLine {
            id: node
            label: "Node name:"
            text: RPC.engine.nodeName
            width: parent.width
        }

        FormLine {
            id: objname
            label: "Object name:"
            text: RPC.engine.objectName
            width: parent.width
        }
        FormLine {
            id: message
            label: "Message text:"
            width: parent.width
        }
        Button {
            id: btn
            text: "Submit"
            width: parent.width
            onClicked: {
                logger.log(new Date, node.text, objname.text, message.text, type.text, {})
                win.close()
            }
        }
    }
}
