import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

FormGenericDelegate {
    id: root
    property alias labelText: l.text
    property alias value: v.checked
    property alias v: v
    property alias l: l
    Label {
        id: l
        Layout.minimumWidth: __labelWidth
        font.pixelSize: __fontSize
    }
    RadioButton {
        id: v
        text: 'Трамвай'
        Layout.minimumWidth: __widgetWidth * 0.5
        exclusiveGroup: x
        ExclusiveGroup{ id: x }
    }
    RadioButton {
        text: 'Троллейбус'
        Layout.minimumWidth: __widgetWidth * 0.5
        exclusiveGroup: x
        checked: !v.checked
    }
}
