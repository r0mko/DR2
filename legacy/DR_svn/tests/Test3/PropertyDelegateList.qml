import QtQuick 2.3
import DataReactor.UI 1.0
import QtQuick.Controls 1.2
import QtQuick.Window 2.2
import QtQuick.Controls.Styles 1.2
UniDelegate {
    id: root
    //    anchors.fill: parent
    property alias uni: root.parent
    property Window win: null
    height: 20
    width: parent.width
    Component.onCompleted: {

    }

    Button{
        id: b
        anchors.fill: parent
        enabled: uni.listValue.length > 0
        clip: true
        smooth: true
        antialiasing: true
        text: "%1[%2]".arg(uni.listItemType).arg(uni.listValue.length)
        onClicked: {
            var pos = UI.mapToScreen(b, 0, b.height)
            win = propWinComp.createObject( mainWindow, { mod: uni.value, obj: uni.listValue, x: pos.x, y: pos.y } )
//            win = propWinComp.createObject( mainWindow, { obj: modelData.value, x: pos.x, y: pos.y } )
        }

    }
    Component{
        id: propWinComp
        Window{
            id: listWindow
            flags: Qt.ToolTip
            property alias obj: cont.model
            property alias mod: ud.value
            onModChanged: console.log(JSON.stringify(mod, undefined, 2))
            UniLoader{
                anchors.fill: parent
                id: ud
                target: listView
            }

            ListWidget {
                id: cont
                anchors.fill: parent
                delegate: Item {
                    width: parent.width
                    height: 20
                    PropertyDelegateObject {
                        property variant modelData: { "value": uni.listValue[index] }
                        onButtonClicked: listWindow.destroy()
                    }
                }
            }
            Component.onCompleted: show()
            height: cont.listItem.height > 800 ? 800 : cont.listItem.height > 0 ? cont.listItem.height : 200
            width: 200
            onFocusObjectChanged: console.log("focus object changed")
            onClosing: win.destroy()
        }
    }
}
