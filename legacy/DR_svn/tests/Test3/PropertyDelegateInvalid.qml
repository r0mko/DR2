import QtQuick 2.3
import DataReactor.UI 1.0

UniDelegate {
    anchors.fill: parent
    Text{
        height: parent.height
        clip: true
        smooth: true
        antialiasing: true
        font.family: main_font.regular.name
        verticalAlignment: Text.AlignVCenter
        text: '=> (' + modelData.typeName + ') \'' + String( modelData.value ) + '\''
        color: Theme.palette.lightSeq[ 0 ]
    }
}
