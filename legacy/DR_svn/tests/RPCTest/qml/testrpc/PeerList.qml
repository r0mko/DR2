import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import "../widgets"
Rectangle {
    id: root
    //    clip: true
    property alias model: list.model
    property alias listItemDelegate: list.delegate
    property Component addDialogComponent
    //    implicitHeight: childrenRect.height
    border.color: "#aaa"
    border.width: 1
    color: "transparent"
    ColumnLayout {
        id: columnLayout1

        anchors.fill: parent
        spacing: -1
        Rectangle {
            id: view
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.top: parent.top
            Layout.fillHeight: true
            clip: true
            border {
                width: 1
                color: "#aaa"
            }
            ListView {
                anchors.fill: view
                id: list
            }
        }
        ThinToolbar {
            id: toolbar
            anchors.right: parent.right
            anchors.left: parent.left
            height: 22
            ThinToolButton {
                id: addbutton
                width: 24
                font.family: "Entypo"
                font.pixelSize: 30
                text: "➕"
                onClicked: {
                    var window = windowComp.createObject(root)
                    var item = addDialogComponent.createObject(window)
                    window.show()
                    item.finished.connect(function() { window.close(); })
                }
            }
        }
    }

    Component {
        id: windowComp
        Window {
            modality: Qt.WindowModal


            width: 240
            color: "#eee"


        }
    }
}
