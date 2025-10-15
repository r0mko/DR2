import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import Dr.UI 1.0
//import Dr.MetaGraph 1.0

RowLayout {
    id: dataview
    Layout.fillWidth: true
    spacing: 1
    signal modified(var value)
    signal accepted(var value)
    signal rejected
    property alias editor: oeditor
    JSObjectEditor {
        id: oeditor
        Layout.minimumHeight: 200
        Layout.fillHeight: true
        Layout.fillWidth: true
//        asynchronous: true
        object: objectData.handle.data
        onModified: dataview.modified(currentValue)
    }
    ColumnLayout {
        spacing: 1
        Layout.minimumWidth: 25
        Layout.maximumWidth: 25
        Layout.fillHeight: true
        Layout.alignment: Qt.AlignTop
        PicButton {
            tooltip: "Apply changes"
            enabled: dataModified
            iconSource: "image://picture/aux/ui/icons/check?size=16x16&color=#333"
            onClicked: accepted(oeditor.getObject())
        }
        PicButton {
            tooltip: "Revert changes"
            enabled: dataModified
            iconSource: "image://picture/aux/ui/icons/undo?size=16x16&color=#333"
            onClicked: rejected()
        }
    }
}
