import QtQuick 2.0
import QtQuick.Controls 1.2
Item{
    id: root
    anchors.fill: parent
    ComboBox{
        clip: true
        anchors.fill: parent
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        visible: ( modelData.isEnumType && modelData.enumerator !== null ) ? true : false
        model: if( modelData.enumerator !== null ) return modelData.enumerator.keys
        currentIndex: if( modelData.enumerator !== null ) { for( var i = 0; i < modelData.enumerator.values.length; ++i ){ if( modelData.enumerator.values[ i ] === modelData.value ) return i; } }
        onCurrentIndexChanged: if( modelData.enumerator !== null ) modelData.value = modelData.enumerator.value( currentIndex )
        smooth: true
        antialiasing: true
    }
    Text{
        text: modelData.value
        visible: !modelData.isWritable && modelData.enumerator === null ? true : false
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
    SpinBox{
        anchors.fill: parent
        value: modelData.value
        maximumValue: 999999999
        minimumValue: -999999999
        onValueChanged: if( modelData.isWritable ) modelData.value = value
        visible: modelData.isWritable && modelData.enumerator === null ? true : false
        clip: true
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        smooth: true
        antialiasing: true
    }
}
