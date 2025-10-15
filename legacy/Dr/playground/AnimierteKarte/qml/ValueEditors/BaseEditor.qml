import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Controls.Styles 1.4

RowLayout {
    spacing: 1
    property alias text: _text.text
    property alias displayText: _text.displayText
//    property alias placeholderText: _text.placeholderText
    property alias validator: _text.validator
//    property alias textFieldStyle: _text.style
    TextInput {
        id: _text
        Layout.leftMargin: 4
        Layout.rightMargin: 4
        Layout.fillHeight: true
        Layout.fillWidth: true
        Component.onCompleted: text = modelValue
        selectedTextColor: sysPalette.highlightedText
        selectionColor: sysPalette.highlight
        font.family: "Helvetica Neue"
//        font.weight: 40
//        verticalAlignment: Text.AlignBottom
        onAccepted: {
            valueUpdated(text)
            focus = false
        }
        onActiveFocusChanged: {
            if (activeFocus)
                selectAll()
        }
//        font.pixelSize: 12
        renderType: Text.NativeRendering
//        style: TextFieldStyle {
//            background: Rectangle {
//                implicitHeight: 20
//                height: 20
//    //            anchors.verticalCenter: control.verticalCenter
//                border.width: control.activeFocus ? 2 : 0
//                border.color: sysPalette.highlight
//                radius: 3
//            }

    //        font.pixelSize: 12

//        }

    }
    SystemPalette { id: sysPalette }
    function commitValue(value) {
        valueUpdated(value)
        focus = false
    }


}
