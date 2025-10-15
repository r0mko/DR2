import QtQuick 2.0
import QtQuick.Controls 1.2

Item{
    id: root
    anchors.fill: parent
    Text{
        text: modelData.value
        visible: modelData.isWritable ? false : true
        color: styleData.textColor
        clip: true
        anchors.fill: parent
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        verticalAlignment: Text.AlignVCenter
        font.bold: true
        smooth: true
        antialiasing: true
    }
    TextField{
        anchors.fill: parent
        text: modelData.value
        onTextChanged: if( modelData.isWritable ) modelData.value = text
        visible: !modelData.isWritable ? false : true
        clip: true
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        verticalAlignment: Text.AlignVCenter
        smooth: true
        antialiasing: true
    }
}
