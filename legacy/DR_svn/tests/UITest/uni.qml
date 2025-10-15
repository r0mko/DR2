import QtQuick 2.3
import QtQuick.Window 2.2
import DataReactor.UI 1.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2


Window {
    id: mainWindow
    width: 800
    height: 800
    title: 'Window title'

    readonly property bool test: true
    property bool test2: true
    property var w: mainWindow
    Component.onCompleted: {
        show()
        console.log(visibility)
    }

    TableView{
        style: TableViewStyle{
            textColor: '#ccc'
        }

        alternatingRowColors: true
        anchors.fill: parent
        model: mo.properties
        rowDelegate: Rectangle{
            height: 28
            color: styleData.selected ? '#558' : ( styleData.alternate ? ( ( styleData.row & 1 == 0 ) ? '#333' : '#383838' ) : '#333' )

        }
        itemDelegate: Text{
            clip: true
            anchors.fill: parent
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            verticalAlignment: Text.AlignVCenter
            color: styleData.textColor
            text: styleData.value
            font.bold: true
        }

        TableViewColumn {
            id: label
            role: 'name'
            title: 'Property'
            width: 140
            delegate: Text{
                clip: true
                anchors.fill: parent
                anchors.leftMargin: 5
                anchors.rightMargin: 5
                verticalAlignment: Text.AlignVCenter
                color: switch( modelData.typeName ){
                       case 'int': return '#79f'
                       case 'double': return '#88f'
                       case 'QString': return '#6f5'
                       case 'bool': return '#fc5'
                       case 'QColor': return '#c22'
                       default: return styleData.textColor
                       }
                text: styleData.value
                font.bold: true
            }

//            color: styleData.textColor
        }
        TableViewColumn {
            id: typename
            role: 'value'
            title: 'Type'
            width: 140
            delegate: UniLoader {
                    target: propertyEditorElement
                    value: modelData.value
                    anchors.fill: parent
                    anchors.margins: 3
                }
        }
        TableViewColumn {
            role: 'typeName'
            title: 'Type'
            width: 320
            delegate: Text{
                text: modelData.typeName
                color: '#ffe'
            }
        }
    }
    MetaObject{
        id: mo
        object: mainWindow
    }



    Target{
        id: propertyEditorElement
        targetName: 'propertyEditorElement'
        Delegate {
            typeName: 'int'
            component: intComp

        }
        Delegate {
            component: stringComp
            typeName: 'QString'
        }
        Delegate {
            component: boolComp
            typeName: 'bool'
        }
//        Delegate{
//            component: Text{ text: 'vis1 int ' + value }
//            typeName: 'Visibility'
//        }
        Delegate {
            component: Text{ clip: true
                anchors.fill: parent
                anchors.leftMargin: 5
                anchors.rightMargin: 5
                verticalAlignment: Text.AlignVCenter
                color: styleData.textColor
                text: 'QQuickWindow* ' + modelData.value
            }
            typeName: 'QQuickWindow*'
        }
        Delegate {
            component: doubleComp
            typeName: 'double'
        }
        Delegate{
            component: Text { clip: true
                anchors.fill: parent
                anchors.leftMargin: 5
                anchors.rightMargin: 5
                verticalAlignment: Text.AlignVCenter
                text: 'QQuickItem ' + modelData.value }
            typeName: 'QQuickItem*'
        }
        Delegate{
            component: Rectangle{ anchors.fill: parent; color: modelData.value; border.width: 1; border.color: '#000' }
            typeName: 'QColor'
        }
        Delegate{
            component: Text{ clip: true
                anchors.fill: parent
                anchors.leftMargin: 5
                anchors.rightMargin: 5
                verticalAlignment: Text.AlignVCenter
                text: '<Object>' }
            typeName: 'QQmlListProperty<QObject>'
        }
        Delegate{
            component: Text{ clip: true
                anchors.fill: parent
                anchors.leftMargin: 5
                anchors.rightMargin: 5
                verticalAlignment: Text.AlignVCenter
                text: 'QObject ' + value }
            typeName: 'QObject*'
        }

    }

    Component{
        id: boolComp
        Item{
            id: root
            anchors.fill: parent
            Text{
                text: modelData.object[ modelData.name ]
                visible: modelData.isWritable ? false : true
                color: styleData.textColor
                clip: true
                anchors.fill: parent
                anchors.leftMargin: 5
                anchors.rightMargin: 5
                verticalAlignment: Text.AlignVCenter
                font.bold: true
            }
            CheckBox{
                checked: modelData.object[ modelData.name ]
                visible: !modelData.isWritable ? false : true
                onCheckedChanged: if( modelData.isWritable ) modelData.object[ modelData.name ] = checked
                clip: true
                anchors.fill: parent
                anchors.leftMargin: 5
                anchors.rightMargin: 5
            }
        }
    }
    Component{
        id: doubleComp
        Item{
            id: root
            anchors.fill: parent
            Text{
                text: modelData.object[ modelData.name ]
                visible: modelData.isWritable ? false : true
                color: styleData.textColor
                clip: true
                anchors.fill: parent
                anchors.leftMargin: 5
                anchors.rightMargin: 5
                verticalAlignment: Text.AlignVCenter
                font.bold: true
            }
            SpinBox{
                stepSize: 0.01
                decimals: 2
                anchors.fill: parent
                value: modelData.object[ modelData.name ]
                maximumValue: 999999999
                minimumValue: -999999999
                onValueChanged: if( modelData.isWritable ) modelData.object[ modelData.name ] = value
                visible: !modelData.isWritable ? false : true
                clip: true
                anchors.leftMargin: 5
                anchors.rightMargin: 5
            }
        }
    }

    Component{
        id: intComp
        Item{
            id: root
            anchors.fill: parent
            Text{
                text: modelData.object[ modelData.name ]
                visible: modelData.isWritable ? false : true
                color: styleData.textColor
                clip: true
                anchors.fill: parent
                anchors.leftMargin: 5
                anchors.rightMargin: 5
                verticalAlignment: Text.AlignVCenter
                font.bold: true
            }
            SpinBox{
                anchors.fill: parent
                value: modelData.object[ modelData.name ]
                maximumValue: 999999999
                minimumValue: -999999999
                onValueChanged: if( modelData.isWritable ) modelData.object[ modelData.name ] = value
                visible: !modelData.isWritable ? false : true
                clip: true
                anchors.leftMargin: 5
                anchors.rightMargin: 5
            }
        }
    }

    Component{
        id: stringComp
        Item{
            id: root
            anchors.fill: parent
            Text{
                text: modelData.object[ modelData.name ]
                visible: modelData.isWritable ? false : true
                color: styleData.textColor
                clip: true
                anchors.fill: parent
                anchors.leftMargin: 5
                anchors.rightMargin: 5
                verticalAlignment: Text.AlignVCenter
                font.bold: true
            }
            TextField{
                anchors.fill: parent
                text: modelData.object[ modelData.name ]
                onTextChanged: if( modelData.isWritable ) modelData.object[ modelData.name ] = text
                visible: !modelData.isWritable ? false : true
                clip: true
                anchors.leftMargin: 5
                anchors.rightMargin: 5
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
