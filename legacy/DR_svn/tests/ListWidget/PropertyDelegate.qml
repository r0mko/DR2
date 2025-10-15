import QtQuick 2.3
import QtQuick.Layouts 1.1
import DataReactor.UI 1.0

Rectangle {
    id: root
    property int defaultSize: 20
    height: defaultSize
    width: propList.width
    clip: true
    opacity: height / defaultSize
    color: inheritLevel > 0 ? "#334" :"transparent"
    property QtObject sourceObj

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 4
        anchors.rightMargin: 4
        spacing: 1
        Text {
            font.weight: Font.Bold
            font.family: "Consolas"
            id: ptype
            color: Theme.palette.lightSeq[6]
            text: typeName
        }
        Item {
            id: space
            width: 4
        }

        Text {
            id: pname
            font.family: "Consolas"
            color: Theme.palette.formColors.type.text
            text: name
        }


        Item {
            id: padding
            Layout.fillWidth: true
        }

        Symbol {
            id: isenum
            color: Theme.palette.pastelSeq[9]
            visible: isEnumType
            font: Symbol.FontAwesome
            symbol: FontAwesome.ListUl
            width: defaultSize
        }
        Symbol {
            //                    visible: MetaProperty.
            color: Theme.palette.pastelSeq[2]
            visible: hasNotifySignal
            font: Symbol.FontAwesome
            symbol: FontAwesome.Rss
            width: defaultSize
        }
        Symbol {
            visible: !isWritable
            color: Theme.palette.pastelSeq[0]
            font: Symbol.FontAwesome
            symbol: FontAwesome.Lock
            width: defaultSize
        }

        Item {
            Layout.fillHeight: true
            width: 60
            clip: true
            // Property value UniDelegate intended to be somewhere around here
            Rectangle {
                anchors.fill: parent
                color: Theme.palette.darkSeq[7]
                opacity: 0.2

            }
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
