import QtQuick 2.3
import DataReactor.UI 1.0
import QtQuick.Controls 1.2
import QtQuick.Window 2.2
import QtQuick.Controls.Styles 1.2
UniDelegate {
//    anchors.fill: parent
    property Window win: null
    signal buttonClicked
    anchors.fill: parent
    Button{
        id: b
        height: parent.height
//        enabled: isWritable
        clip: true
        smooth: true
        antialiasing: true
        text: 'List ' + String( modelData.value.length ) + ' ...'
        onClicked: {
            win = propWinComp.createObject( mainWindow, { 'obj': modelData.value } )
            buttonClicked()
        }
//        Component.onCompleted: console.log(Object.keys(modelData.value))
//        style: ButtonStyle{
//            label: Text{
//                anchors.fill: parent
//                verticalAlignment: Text.AlignVCenter
//                renderType: Text.QtRendering
//                text: b.text
//            }
//        }
    }
    Component{
        id: propWinComp
        Window{
            property alias obj: cont.value
//            Component.onCompleted: show()
            title: String( modelData.value ) + ' properties'
            color: '#333'
            height: 800
            width: 600
            UniLoader{
                id: cont
                anchors.fill: parent
                target: listView
                Component.onCompleted: {
                    console.log( 'UNI ' + cont.value )
                }
            }
            Component.onCompleted: {
                show()
                console.log( 'WINDOW ' + obj )
            }
            onClosing: win.destroy()
        }
    }

}
