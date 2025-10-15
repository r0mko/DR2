import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import Dr.Cartograph 1.0
import Dr.MetaGraph 1.0


ColumnLayout {
    id: root
    property alias model: _table.model
    property alias view: _table
    property QtObject currentNode
    spacing: 1
    TableView {
        id: _table
        Layout.fillHeight: true
        Layout.fillWidth: true
        onCurrentRowChanged: { root.currentNode = model.get(currentRow) }

        TableViewColumn {
            role: "handle"
            width: _table.width - 2
            delegate: Item {
                clip: true
                Component.onCompleted: console.log("DS row:", styleData.value)
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 4
                    anchors.rightMargin: 4
                    Item {
                        Layout.fillHeight: true
                        Layout.minimumWidth: parent.height
                        Rectangle {
                            anchors.centerIn: parent
                            color: styleData.value.data.isOpen ? "#00c000" : "#b00000"
                            width: 8
                            height: 8
                            radius: 2
                        }
                    }
                    Text {
                        renderType: Text.NativeRendering
                        font.pixelSize: 12
                        font.bold: true
                        text: styleData.value.nodeId
                        anchors.verticalCenter: parent.verticalCenter
                        color: styleData.textColor
                    }
                    Text {
                        renderType: Text.NativeRendering
                        font.pixelSize: 12
                        text: styleData.value.data.filename
                        elide: Text.ElideMiddle
                        anchors.verticalCenter: parent.verticalCenter
                        color: styleData.textColor
                    }
                    Item {
                        Layout.fillWidth: true
                    }
                }
            }
        }
    }

    RowLayout {
        Layout.fillWidth: true
        spacing: 1
        Item { width: 1 }
        VectorIconButton {
            id: b_add
//            text: "Добавить..."
            height: 20
            width: 20
            flat: false
            tooltip: "Добавить БД"
            iconData: aux.icons.add.data
            onClicked: dbPicker.open()
        }
    }

    FileDialog {
        id: dbPicker
        nameFilters: ["SQLite files (*.sqlite)"]
        selectExisting: true
        modality: Qt.NonModal
        folder: shortcuts.desktop
        onAccepted: {
            console.log(fileUrl, fileUrl.toString().match(/\/([^/]+\.\w+)/))
            fDataNode.data = { "url": fileUrl.toString(), "isOpen": false, "filename": fileUrl.toString().match(/\/([^/]+\.\w+)/)[1] }
            fDataNode.parentHandle = model.rootHandle
            var n = fDataNode.create("db%1".arg(model.count + 1))
        }
    }
}

