import QtQuick 2.4
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4

RowLayout {
    id: root

    property alias text: key_lbl.text
    property alias repeater: conn.target
    property int keyLabelWidth
    property int labelWidth: keyLabelWidth
    property int rowHeight: 20
    default property Component valueDelegate
    property alias valueItem: valueLoader.item
    width: parent ? parent.width : 0
    spacing: 1
    implicitHeight: rowHeight


    Connections {
        id: conn
        target: null
        onWChanged: root.labelWidth = target.w
    }

    Rectangle {
        id: key_cont
        implicitHeight: rowHeight
        implicitWidth: labelWidth
        color: "#666"
        //        color: "#bebebe" // :)
        Label {
            id: key_lbl
            color: "#ddd"
            anchors.left: parent.left
            anchors.leftMargin: 5
            anchors.verticalCenter: parent.verticalCenter
            font.family: "Roboto"
            onPaintedWidthChanged: {
                root.keyLabelWidth = paintedWidth + anchors.leftMargin * 2
                if (repeater)
                    repeater.fixColumnWidth(root)
            }
        }
    }

    Rectangle {
        clip: true
        Layout.fillWidth: true
        Layout.fillHeight: true
//        implicitHeight: rowHeight
        Loader {
            id: valueLoader
            anchors {
                left: parent.left
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            sourceComponent: valueDelegate
        }
    }
    Component.onCompleted: {
//        console.log("row completed", width, implicitWidth, height, implicitHeight, parent)
        if (parent.hasOwnProperty("__repeater")) {
            conn.target = parent.__repeater
            labelWidth = parent.__repeater.w
            parent.__repeater.fixColumnWidth(root)
        }

    }
}
