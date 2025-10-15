import QtQuick 2.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1

BaseEditor {
    LayoutMirroring.enabled: true
    Rectangle {
        Layout.fillHeight: true
        Layout.rightMargin: 2
        Layout.leftMargin: 2
        width: 40
        color: modelValue
        radius: 3
        ColorDialog {
            id: colorpicker
            showAlphaChannel: true
            onVisibleChanged: color = modelValue
            onCurrentColorChanged:{
                if( visible ) {
                    text = currentColor.toString()
                    valueUpdated(text)
                }
            }
            onAccepted: {
                color.toString()
                valueUpdated(color.toString())
            }
        }
        MouseArea {
            anchors.fill: parent
            onClicked: colorpicker.open()
        }
    }
}

