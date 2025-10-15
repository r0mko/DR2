import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4

RowLayout {
    property alias text: data.text
    clip: true
    Item{ width: root.margin }
    CheckBox {
        id: data
        Layout.fillWidth: true
        Layout.fillHeight: true
        enabled: false
//        text: String( styleData.value )
        checked: styleData.value
    }
    Item{ width: root.margin }
}
