import QtQuick 2.3
import DataReactor.UI 1.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
Target {
    id: _propertyEditor
    targetName: 'propertyEditor'
    Delegate {
        typeName: 'QObject*'
        component: TableView {
            id: root
            smooth: true
            antialiasing: true
            anchors.fill: parent

            model: mo.properties

            style: tableViewStyleComponent
            rowDelegate: rowDelegateComponent
            itemDelegate: itemDelegateComponent

            MetaObject {
                id: mo
                object: root.parent.value
            }

            TableViewColumn {
                id: propertyNameColumn
                role: 'name'
                title: 'Property'
                width: parent.width * 0.35
                delegate: propertyNameColumnComponent

            }
            TableViewColumn {
                id: propertyValueColumn
                role: 'value'
                title: 'Value'
                width: parent.width * 0.64
                delegate: UniDelegate {
                    targetName: "propertyEditorElement"
                    value: modelData.value
                    anchors.fill: parent
                    anchors.margins: 3
                }
            }

            Component{
                id: tableViewStyleComponent
                TableViewStyle{
                    textColor: '#ccc'
                }
            }


            Component{
                id: rowDelegateComponent
                Rectangle{
                    height: 28
                    color: styleData.selected ? '#558' : ( styleData.alternate ? ( ( styleData.row & 1 == 0 ) ? '#333' : '#383838' ) : '#333' )
                }
            }
            Component{
                id: itemDelegateComponent
                Text{
                    clip: true
                    anchors.fill: parent
                    anchors.leftMargin: 5
                    anchors.rightMargin: 5
                    verticalAlignment: Text.AlignVCenter
                    color: styleData.textColor
                    text: styleData.value
                    font.bold: true
                }
            }
            Component{
                id: propertyNameColumnComponent
                Text{
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
            }
        }

    }

}
