import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.0
import Dr.UI 1.0
import "typeeditor"

RP_Node {
    properties: [
        { "Размер": "%1 байт".arg(objectData.type$rawDataSize) },
    ]
    CaptionLabel {
        Layout.fillWidth: true
        text: 'Редактор типа'// + objectData.handle.isRegistered
        level: 3
    }
    TypeEditor {
        id: editor
        Layout.fillWidth: true
        Layout.minimumHeight: 400
        builder: f_Type
    }

}

