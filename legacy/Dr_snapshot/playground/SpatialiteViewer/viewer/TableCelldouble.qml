import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

TextField {
    Component.onCompleted: text = modelValue
    verticalAlignment: Text.AlignBottom

//    renderType: Text.NativeRendering
    onAccepted: {
        valueUpdated(text)
        focus = false
    }
    validator: DoubleValidator { id: validator }
    onActiveFocusChanged: {
        if (activeFocus)
            selectAll()
    }
    style: TextFieldStyle {
        background: Rectangle {
//            implicitHeight: 20
//            anchors.verticalCenter: control.verticalCenter
            border.width: control.activeFocus ? 1 : 0
            border.color: sysPalette.highlight
            radius: 3
        }
        renderType: Text.NativeRendering

//        font.pixelSize: 12

    }
    SystemPalette { id: sysPalette }
}

