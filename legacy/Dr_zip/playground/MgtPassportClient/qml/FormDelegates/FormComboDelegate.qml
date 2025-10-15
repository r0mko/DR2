import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import Test 1.0
FormGenericDelegate {
    id: root
    property alias labelText: l.text
    property int value
    property alias v: v
    property alias l: l

    property string indexRole
    property alias textRole: v.textRole
    property alias datasource: m.dataSource
    property alias modelService: m.modelService
    property alias proxymodel: pm
    property alias datamodel: m
    property var rowData

    onValueChanged:{
        var index = pm.getRowByData( root.indexRole, root.value )
        v.currentIndex = index
//        console.log( 'value changed', value, index )
    }

    Label {
        id: l
        Layout.minimumWidth: __labelWidth
        font.pixelSize: __fontSize
    }
    ComboBox {
        id: v
//        property int vid : model.item( currentIndex ).id
        Layout.minimumWidth: __widgetWidth
        Layout.fillWidth: true
        currentIndex: pm.getRowByData( root.indexRole, root.value )
        onCurrentIndexChanged: {
            root.rowData = pm.getItemData( currentIndex )
            root.value = pm.getItemData( currentIndex )[root.indexRole] === undefined ? -1 : pm.getItemData( currentIndex )[root.indexRole]
        }
        model: pm
        ProxyModel {
            id: pm
            source: m
            onModelReset: v.currentIndex = pm.getRowByData( root.indexRole, root.value )
        }
        PartModel {
            id: m
            onDataUpdated: {
                console.log( 'inVALIDATED')
                pm.invalidate()
            }
        }
    }
}
