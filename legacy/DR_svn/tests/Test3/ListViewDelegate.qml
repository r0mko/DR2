import QtQuick 2.3
import DataReactor.UI 1.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2

ListWidget{
    id: root
    model: parent.value
    anchors.fill: parent
    spacing: 1
    delegate: Rectangle{
        height: 20
        width: root.width
        color: '#444'
        PropertyDelegateObject {
            height: 20
            width: 200
            property variant modelData: { "value": root.model[ index ] }
//            onButtonClicked: listWindow.destroy()
        }
    }
    Component.onCompleted: console.log( 'LISTVIEW MODEL' + model )
}
