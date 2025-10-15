import QtQuick 2.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1

RowLayout {
    Rectangle {
        id: rect
        anchors.fill: parent
        anchors.margins: 2
        color: modelValue
        //    border.width: 1
        //    border.color: "#000"
        radius: 3
        ColorDialog {
            id: colorpicker
            onVisibleChanged: color = modelValue
            onCurrentColorChanged: valueUpdated(currentColor.toString())
            onAccepted: valueUpdated(color.toString())
        }
        MouseArea {
            anchors.fill: parent
            onClicked: colorpicker.open()
        }
    }
}

