import QtQuick 2.3
import QtQuick.Controls 1.2

TextField {
    id: field

    signal valueChanged(string value)
    signal tabPressed(bool forward)
    font.family: "Roboto"
    function setValue(value) {
        text = value
        valueChanged(value)
        if (activeFocus) {
            editingFinished()
        }
    }

    onAccepted: valueChanged(text)
    Keys.onEscapePressed: editingFinished()

    Keys.onTabPressed:  tabPressed(event.modifiers & Qt.ShiftModifier)
    onActiveFocusChanged: {
        if (!activeFocus)  {
            editingFinished()
        }
    }

    Component.onCompleted: {
        forceActiveFocus()
        selectAll()
    }

}
