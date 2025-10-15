import QtQuick 2.3
import DataReactor.UI 1.0
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.2
UniDelegate {
    id: root
    value: cb.color
    anchors.fill: parent

    Row{
        height: parent.height
        Rectangle{
            id: cb
            height: parent.height
            width: parent.height// * 1.5
            radius: parent.height / 2
            border.width: 1
            border.color: '#000'
            color: root.value
            MouseArea{
                anchors.fill: parent
                onClicked: cd.open()
            }
        }
        TextField{
            height: parent.height
            text: cb.color
            enabled: isWritable
            clip: true
            smooth: true
            antialiasing: true
            onEditingFinished: {
                focus = false
                root.value = text
            }
            validator: RegExpValidator{
                regExp: /#([a-f0-9]{3}){1,2}/gi
            }
            style: TextFieldStyle{
                renderType: Text.QtRendering
            }
        }
    }
    ColorDialog{
        id: cd
        color: root.value
        onColorChanged: modelData.value = color
    }
}
