import QtQuick 2.5
import QtQuick.Layouts 1.2

RowLayout {
    property alias text: data.text
    clip: true
    Item{ width: root.margin }
    Text {
        id: data
        Layout.fillWidth: true
        Layout.fillHeight: true
        elide: styleData.elideMode
        horizontalAlignment: styleData.textAlignment
        verticalAlignment: Text.AlignVCenter
        text: String( styleData.value )
        renderType: Text.NativeRendering
        color: styleData.selected ? '#fff' : '#000'
        font.pixelSize: root.fontSize
        font.family: root.fontFamily
    }
    Item{ width: root.margin }
}
