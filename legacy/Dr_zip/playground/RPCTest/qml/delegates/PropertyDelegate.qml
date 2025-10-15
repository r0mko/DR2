import QtQuick 2.3
import QtQuick.Layouts 1.1
import DataReactor.UI 1.0
import "../theme" 1.0
import "../widgets"

Rectangle {
    id: root
    property int defaultSize: 20
    height: defaultSize
    width: propList.width
    clip: true
    opacity: height / defaultSize
    color: propertyIndex >= scanner.propertyOffset ? "#334" : "transparent"
    property QtObject sourceObj

    //                            visible: showSuper1.checked ? true : propertyIndex >= scanner.propertyOffset
    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 4
        anchors.rightMargin: 4
        spacing: 1
        Text {
            font.weight: Font.Bold
            font.family: "Consolas"
            id: ptype
            color: Theme.colors.dataType
            text: typeName
        }
        Item {
            id: space
            width: 4
        }

        Text {
            id: pname
            font.family: "Consolas"
            color: Theme.colors.text
            text: name
        }


        Item {
            id: padding
            Layout.fillWidth: true
        }

        Symbol {
            id: isenum
            color: Theme.colors.text
            visible: isEnumType
            font: Symbol.FontAwesome
            symbol: FontAwesome.ListUl
            width: defaultSize
        }
        Symbol {
            //                    visible: MetaProperty.
            color: Theme.colors.stringType
            visible: hasNotifySignal
            font: Symbol.FontAwesome
            symbol: FontAwesome.Rss
            width: defaultSize
        }
        Symbol {
            visible: !isWritable
            color: "#ff7070"
            font: Symbol.FontAwesome
            symbol: FontAwesome.Lock
            width: defaultSize
        }

        Rectangle {
            Layout.fillHeight: true
            width: 60
            clip: true
            color: "#a026283d"
            Text {
                id: val
                anchors.fill: parent
                color: "white"
                font: Theme.defaultFont
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignRight
                text: value
            }
        }


    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons
        enabled: true
        onClicked: {
            val.text = value
//            console.log("type:", typeName, UI.getComponent("form", typeName).url)
        }
    }
}
