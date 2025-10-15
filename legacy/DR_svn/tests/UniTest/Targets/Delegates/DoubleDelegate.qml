import QtQuick 2.0
import QtQuick.Controls 1.2
Item {
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
    SpinBox {
        stepSize: 0.01
        decimals: 2
        anchors.fill: parent
        value: modelData.value
        maximumValue: 999999999
        minimumValue: -999999999
        onValueChanged: if( modelData.isWritable ) modelData.value = value
        visible: !modelData.isWritable ? false : true
        clip: true
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        smooth: true
        antialiasing: true
    }
}
