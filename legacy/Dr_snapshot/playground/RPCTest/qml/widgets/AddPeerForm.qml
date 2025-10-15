import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import Dr.QRPC 1.0

Item {
    width: 200
    anchors.fill: parent
    anchors.margins: 16
    signal finished

    Keys.onEscapePressed: {
        finished()
    }


    ColumnLayout {
        id: columnLayout1
        anchors.fill: parent
        FormLine {
            id: hostAddress
            text: ""
            label: "Host"
        }

        FormLine {
            id: port
            text: "42001"
            label: "Port"
        }

        CheckBox {
            id: checkBox1
            text: qsTr("active connection")
            checked: true
        }

        Button {
            id: button1
            text: qsTr("Add")
            anchors.left: parent.left
            anchors.right: parent.right
            onClicked: {
                var object = peerComp.createObject(null, {host: hostAddress.text, port: parseInt(port.text) })
                finished()
            }
        }

        Item {
            id: dummy
            Layout.fillHeight: true
        }
    }

    Component {
        id: peerComp
        RemotePeer { }
    }
}
