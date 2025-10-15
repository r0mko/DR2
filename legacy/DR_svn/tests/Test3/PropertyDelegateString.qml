import QtQuick 2.3
import DataReactor.UI 1.0
import QtQuick.Controls 1.2

UniDelegate {
    anchors.fill: parent
    PropertyDelegateCommon{
        visible: !isWritable
        color: Theme.palette.lightSeq[ 3 ]
    }
    TextField{
        width: parent.width
        height: parent.height
        visible: isWritable
        enabled: isWritable
        clip: true
        smooth: true
        antialiasing: true
        font.family: main_font.regular.name
        text: modelData.value
        onTextChanged: modelData.value = text
    }
}
