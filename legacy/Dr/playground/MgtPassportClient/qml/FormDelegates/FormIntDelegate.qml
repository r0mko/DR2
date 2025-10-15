import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

FormGenericDelegate {
    id: root
    property alias labelText: l.text
    property alias value: v.value
    property alias v: v
    property alias l: l
    Label {
        id: l
        Layout.minimumWidth: __labelWidth
        font.pixelSize: __fontSize
    }
    SpinBox {
        id: v
        Layout.minimumWidth: __widgetWidth
        Layout.fillWidth: true
        minimumValue: -999999999999999
        maximumValue: 999999999999999
    }
}
