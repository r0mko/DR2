import QtQuick 2.3
import DataReactor.UI 1.0
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.2
UniDelegate {
    anchors.fill: parent
    Button{
        id: b
        height: parent.height
        enabled: isWritable
        clip: true
        smooth: true
        antialiasing: true
        text: modelData.value + '...'
        onClicked: fd.open()
//        style: ButtonStyle{
//            label: Text{
//                anchors.fill: parent
//                verticalAlignment: Text.AlignVCenter
//                renderType: Text.QtRendering
//                text: b.text
//            }
//        }
    }

    FontDialog{
        id: fd
        font: modelData.value
        modality: Qt.ApplicationModal
        onAccepted: modelData.value = fd.font
        onFontChanged: modelData.value = fd.font
    }

}
