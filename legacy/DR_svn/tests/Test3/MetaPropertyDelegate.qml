import QtQuick 2.3
import DataReactor.UI 1.0
import QtQuick.Layouts 1.1

Rectangle{
    property real defaultHeight: 20
    property real collumnLabelWidth: listW.cw

    height: defaultHeight
//    height: childrenRect.height
    width: parent.width
    antialiasing: true
    clip: true
    opacity: height / defaultHeight
    color: ma.containsMouse ? '#30ccccff' : inheritLevel > 0 ? '#20ffffff' : '#10ffffff'

    MouseArea{
        id: ma
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        hoverEnabled: true
    }

    Item{
        id: lc
        width: collumnLabelWidth
        height: parent.height
        clip: true
        Item{
            anchors.fill: parent
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            clip: true
            RowLayout{
                height: parent.height
                clip: true
                Text {
                    text: typeName
                    clip: true
                    font.weight: Font.Bold
                    font.family: main_font.bold.name
                    color: {
                        if( isEnumType ) return Theme.palette.lightSeq[ 7 ]
                        switch( typeName ){
                        case 'QString': return Theme.palette.lightSeq[ 3 ]
                        case 'QUrl': return Theme.palette.lightSeq[ 3 ]
                        case 'bool': return Theme.palette.lightSeq[ 6 ]
                        case 'int': return Theme.palette.lightSeq[ 7 ]
                        case 'double': return Theme.palette.lightSeq[ 8 ]
                        default: return '#fff'
                        }
                    }
                }
                Text {
                    text: name
                    clip: true
                    font.family: main_font.regular.name
                    color: Theme.palette.formColors.type.text
                }
                Symbol {
                    visible: isEnumType
                    color: Theme.palette.pastelSeq[7]
                    font: Symbol.FontAwesome
                    symbol: FontAwesome.ListOl
                }
                Symbol {
                    visible: ( typeName.indexOf( 'QQmlListProperty' ) > -1 ) || ( typeName.indexOf( 'QVariantList' ) > -1 )
                    color: Theme.palette.pastelSeq[7]
                    font: Symbol.FontAwesome
                    symbol: FontAwesome.List
                }
            }
        }
    }
    Item{
        id: vc
        x: collumnLabelWidth
        width: parent.width - collumnLabelWidth
        height: parent.height

        Item {
            anchors.fill: parent
            anchors.leftMargin: 5
            anchors.rightMargin: 5

            UniLoader {
                id: loader1
                antialiasing: true
                anchors.fill: parent
                target: propertyEditorElement
//                value: modelData.value
                valueSource: "modelData.value"
                variant: modelData.inheritClassName + "." + modelData.name
//                onValueChanged: modelData.value = loader1.value
            }
        }
    }

}
