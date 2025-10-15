import QtQuick 2.3
import DataReactor.UI 1.0
import QtQuick.Controls 1.2

UniDelegate {
    id: root
    value: cbox.checked
    anchors.fill: parent

//    Component.onCompleted: cbox.checked = value

    PropertyDelegateCommon{ // not writable integer
        visible: !isWritable
        color: Theme.palette.lightSeq[ 6 ]
    }
    CheckBox{
        id: cbox
        width: parent.width
        height: parent.height
        visible: isWritable
        enabled: isWritable
        clip: true
        smooth: true
        antialiasing: true
        checked: root.value
//        text: modelData.value
//        onTextChanged: modelData.value = text
//        onCheckedChanged: modelData.value = checked
    }
}
