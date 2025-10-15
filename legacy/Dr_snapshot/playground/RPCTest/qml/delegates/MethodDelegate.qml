import QtQuick 2.3
import QtQuick.Layouts 1.1
import DataReactor.UI 1.0
import "../theme" 1.0
import "../widgets"

Rectangle {
    id: met
    width: 4
    color: methodIndex >= scanner.methodOffset ? "#333" : "#444"
    height: 20
    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 4
        anchors.rightMargin: 4
        spacing: 1
        Item {
            width: parent.height
            height: parent.height
            //                            color: "#80000000"
            Symbol {
                anchors.fill: parent
                color: Theme.colors.text
                font: Symbol.FontAwesome
                symbol: {
                    switch(methodType) {
                    case MetaMethod.Signal:
                        color = "#a0deff"
                        return FontAwesome.Plug
                    case MetaMethod.Slot:
                        color = "#FFa080"
                        return FontAwesome.PlayCircleO
                    case MetaMethod.Method:
                        color = "#FFe080"
                        return FontAwesome.DotCircleO
                    default:
                        return FontAwesome.Openid
                    }
                }
            }
        }

        Text {
            font.weight: Font.Bold
            font.family: "Consolas"
            color: Theme.colors.dataType
            text: returnTypeName
        }
        Item {
            width: 4
        }

        Text {
            font.family: "Consolas"
            color: Theme.colors.text
            text: name
        }
        Symbol {
            color: "#80ffffff"
            font: Symbol.FontAwesome
            symbol: FontAwesome.InfoCircle
        }

        Item {
            Layout.fillWidth: true
        }


        Text {
            font.family: "Consolas"
            color: Theme.colors.text
            text: tag
        }

        Rectangle {
            visible: revision > 0
            width: 35
            color: "#80a080"
            height: 16
            radius: 6
            border.width: 1
            border.color: "black"
            Text {
                font.family: Theme.defaultFont.family
                font.pointSize: 10
//                font.weight: Font.Light
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: "black"
                text: "REV " + revision
            }
        }

        Symbol {
            color: Theme.colors.stringType
            font: Symbol.FontAwesome
            visible: access != MetaMethod.Public
            symbol: {
                switch(access) {
                case MetaMethod.Private:
                    return FontAwesome.Lock
                case MetaMethod.Protected:
                    return FontAwesome.Key
                default:
                    return FontAwesome.Gear
                }
            }
        }

    }
}
