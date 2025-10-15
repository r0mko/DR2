import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

FormGenericDelegate {
    id: root
    property alias labelText: l.text
    property alias value: v.text
    property alias v: v
    property alias l: l
    Label {
        id: l
        Layout.minimumWidth: __labelWidth
        font.pixelSize: __fontSize
    }
    TextField {
        id: v
        Layout.minimumWidth: __widgetWidth
        Layout.fillWidth: true
        echoMode: TextInput.Password
    }
}
