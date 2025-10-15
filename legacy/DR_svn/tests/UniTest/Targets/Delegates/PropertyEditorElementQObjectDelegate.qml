import QtQuick 2.3
import QtQuick.Controls 1.2
import DataReactor.UI 1.0

Button{
    id: root
    clip: true
    smooth: true
    antialiasing: true
    property var object
    anchors.fill: parent
    anchors.leftMargin: 5
    anchors.rightMargin: 5
    text: object + ' ...'
    onClicked: {
        wC.createObject()
        console.log( modelData.value )
    }
    Component{
        id: wC
        MyWindow{
            width: 400
            height: 800
            flags: Qt.Tool
            UniDelegate {
                target: propertyEditor
                value: root.object
                anchors.fill: parent
            }
            Component.onCompleted: show()
        }
    }
}
