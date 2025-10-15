import QtQuick 2.5
import QtQuick.Controls 1.4

Item {

    property alias value: _cbx.checked
    signal valueUpdated(bool value)
    CheckBox {
        id: _cbx
        onClicked: valueUpdated(checked)
    }
}
