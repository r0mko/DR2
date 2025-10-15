import QtQuick 2.3
import DataReactor.UI 1.0

UniDelegate {
    anchors.fill: parent
    property alias color: c.color
    property alias text: c.text
    Text{
        id: c
        height: parent.height
        clip: true
        smooth: true
        antialiasing: true
        font.family: main_font.regular.name
        verticalAlignment: Text.AlignVCenter
        text: String( modelData.value )
        color: '#fff'
    }
}
