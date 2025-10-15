import QtQuick 2.5
import QtQuick.Controls 1.4
import Test 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.2

ColumnLayout{
    id: root
//    property alias datasource: __dataModel.dataSource
//    property alias dataModel: __dataModel
    property PartModel __dataModel
    property alias proxyMmodel: __proxyModel
    property var fieldData: __dataModel.fieldData
    property alias selection: table.selection

    property alias userFunctions: userRowContextMenu.model

    property alias editDialogLeyout: editDialogLoader.sourceComponent

    property var selectedData:  null
//    onSelectedDataChanged: console.log( 'changed data')

    spacing: 0
    Rectangle {
        Layout.fillWidth: true
        height: 36
        color: 'transparent'
        RowLayout {
            anchors.fill: parent
            Item{ width:5 }
            Button {
                text: 'сбросить все фильтры'
                onClicked: {
                    table.filters = []
                    table.filtersChanged()
                    __proxyModel.removeAllFilters()
                }
            }
            Button {
                text: 'Автоподбор ширины'
                onClicked: {
                    table.resizeColumnsToContents()
                }
            }
//            Button {
//                text: 'test1'
//                onClicked: {
//                }
//            }
            // TODO: placeholder for outer components like filters and stuff ??
            Item{ Layout.fillWidth: true }
        }
    }
    TableView {
        id: table
        model: __proxyModel
        Layout.fillWidth: true
        Layout.fillHeight: true

        property var filters: [] // holds ammount of filters per role
        readonly property int __tableScale: uiStyle.fontSizeMed * 0.9

        headerDelegate: tableHeaderDelegate
        rowDelegate: tableRowDelegate


        // reset selection on sort
        onSortIndicatorColumnChanged: selection.clear()
        onSortIndicatorOrderChanged: selection.clear()

        Connections{ // on selection changed
            target: table.selection
            onSelectionChanged: {
                //                console.log( 'selection changed' )
                //                root.selectedData = __proxyModel.getItemData( )
                root.selectedDataChanged()
                table.selection.forEach( function( i ){
                    root.selectedData =  __proxyModel.getItemData( i );
//                    console.log( 'selection has row', i, __proxyModel.getItemData( i ).name )
                } )

            }
        }

        Instantiator { // column instantiator
            id: inst
            model: __dataModel.fieldData
            delegate: loaderComp
//asynchronous : true
            onObjectAdded: if( !object.hidden ) table.addColumn( object.item )
            onObjectRemoved: if( !object.hidden ) table.removeColumn( 0 )
            Component { // column delegate loader for instantitor
                id: loaderComp
                Loader {
                    id: root
                    property bool hidden: modelData.hidden === undefined ? false : modelData.hidden
                    source: if( hidden )                                        return ''
                            else if( modelData.tableDelegate === 'text' )            return 'TableBasicColumn.qml'
                            else if( modelData.tableDelegate === 'int' )             return 'TableIntColumn.qml'
                            else if( modelData.tableDelegate === 'real' )            return 'TableRealColumn.qml'
                            else if( modelData.tableDelegate === 'date' )            return 'TableDateColumn.qml'
                            else if( modelData.tableDelegate === 'bool' )            return 'TableBoolColumn.qml'
                            else if( modelData.tableDelegate === 'transportType' )   return 'TableTransportColumn.qml'
                            else                                                     return 'TableBasicColumn.qml'
                    onLoaded: item.width = modelData.width === undefined ? 110 : modelData.width
                }
            }
        }

//        PartModel { // data model
//            id: __dataModel
//            modelService: service
//            loadingText: 'загрузка...'
//            windowSize: 5000
//            onRowCountChanged: __proxyModel.invalidate()
//        }

        ProxyModel { // proxy model
            id: __proxyModel
            source: __dataModel
            // FIXME:afteer all is loaded we have sort indicator  but on clicking on it nothing happens. we have to switch role to get it work
            sortRole: table.getColumn( table.sortIndicatorColumn ) === null ? '' : table.getColumn( table.sortIndicatorColumn ).role
            sortOrder: table.sortIndicatorOrder
        }

        Component { // table header delegate
            id: tableHeaderDelegate
            Item {
                id: headerCell
                height: Math.round( table.__tableScale * 1.5 )
                clip: true
                Rectangle { // header background
                    anchors.fill: parent
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: '#ffffff' }
                        GradientStop { position: 0.5; color: '#f4f4f4' }
                        GradientStop { position: 0.500001; color: '#ececec' }
                        GradientStop { position: 1.0; color: '#ededed' }
                    }
                    Rectangle {
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.bottomMargin: 1
                        height: 1
                        color: '#8e8e8e'
                    }
                    Rectangle {
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.bottomMargin: 1
                        height: 1
                        color: styleData.pressed ? '#eee' : '#fff'
                    }
                    Rectangle {
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.topMargin: 1
                        height: 1
                        color: '#b6b6b6'
                    }


                    Rectangle {
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 3
                        anchors.topMargin: 3
                        width: 1
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: '#bdbdbd' }
                            GradientStop { position: 0.5; color: '#bdbdbd' }
                            GradientStop { position: 0.500001; color: '#c5c5c5' }
                            GradientStop { position: 1.0; color: '#c5c5c5' }
                        }
                        //                    color: "#ccc"
                    }
                    Rectangle{
                        anchors.fill: parent
                        color: styleData.pressed ? '#20000000' : 'transparent'
                    }
                }
                RowLayout { // header row layout
                    anchors.fill: parent
                    Item { width: 5 }
                    Text {
                        text: styleData.value
                        color: '#000'
                        elide: Text.ElideMiddle
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        renderType: Text.NativeRendering
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: table.__tableScale
                    }
                    Item { width: 5; visible: table.sortIndicatorColumn === styleData.column }
                    Image {
                        id: columnFilterIndicator


                        visible: table.columnCount !== 0 && table.getColumn( styleData.column ) !== null ?
                                     table.filters[ table.getColumn( styleData.column ).role ] !== undefined ?
                                         table.filters[ table.getColumn( styleData.column ).role ] > 0 :
                                         false :
                        false




                        source: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAABHNCSVQICAgIfAhkiAAAAAlwSFlzAAAAOgAAADoBai3sFgAAABl0RVh0U29mdHdhcmUAd3d3Lmlua3NjYXBlLm9yZ5vuPBoAAACnSURBVDiNvZExCoNAEEWfYmNn6/1yitzCJgcIXsDC3kIkrXcIBDxCAvlpRtnEjWxWyIeF2c+8t8UCNIAiT5MCA/EZkARQRbxeSWIWJED9A1wDySIwSQa0AXALZAs3DybJgX4D7oH8jXEvJimA0QOPQLHa/yxMUgKTA09A6dtNfX8j6QZ0TtVZt4pXYHl+mYMFQfmb4L5H8ABOewQHSZcYwRU4Sjpv2V/4Sbqe9VUBsAAAAABJRU5ErkJgggc0495ec3b4dec02323da115321b64574"
                    }
                    Image {
                        id: columnSortIndicator
                        visible: table.sortIndicatorColumn === styleData.column
                        rotation: table.sortIndicatorOrder === Qt.DescendingOrder ? 0 : 180
                        source: 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAABHNCSVQICAgIfAhkiAAAAAlwSFlzAAAAggAAAIIBsKhZvgAAABl0RVh0U29mdHdhcmUAd3d3Lmlua3NjYXBlLm9yZ5vuPBoAAADISURBVDiNjZK9DcIwEEZfB02akAHYIn/MwG4ZhZIqExB+BoiYAClJEVGZ5iydLJ9D8VV379nyZ5xzxALkwEWSm3sJeACcZLAkMfgA3AX8SkxJDH4o+CwxJUlYzUyJXyiAZwzeknj4ZZycAVlKAtAbcAl8JKUhuSJXXwO4AiZV4wRUgWQFbgB7oFDDWsFviZfUaq8AduFD1cCs4KPES2YtCWtsQljNQkkT1thacELS+hpPwJKCDckiLOM/sCEZATr5C5twIOmB7gcWe5oqyhsj1QAAAABJRU5ErkJggg79c994811fbb55b052c3b498da115bb8'
                    }
                    Item { width: 5 }
                }
                Connections { // connection for floater
                    target: styleData
                    onContainsMouseChanged: {
                        if (styleData.containsMouse) {
                            var pos = table.mapFromItem(headerCell.parent)
                            floatFilterBar.x = pos.x
                            floatFilterBar.y = pos.y
                            floatFilterBar.width = headerCell.parent.width
                            floatFilterBar.height = headerCell.parent.height
                            floatFilterBar.currentColumn = styleData.column
                        }
                    }
                }
            }
        }

        Item { // header floater delegate
            id: floatFilterBar
            property int currentColumn: -1
            function reset(){
                width = 0
                currentColumn = -1
            }
            MouseArea {
                id: mousearea
                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons: Qt.RightButton
                onContainsMouseChanged: containsMouse ? parent.opacity = 1 : parent.opacity = 0
                onClicked: {
                    //                    console.log( 'header floater clicked', table.getColumn( parent.currentColumn ).role )
                    headerContextMenu.popup()
                    mouse.accepted = true
                }
            }
        }

        Component { // generic filterObject componet
            id: filterObject
            FilterObject {}
        }

        Dialog { // add filter dialog
            id: addFilterDialog
            title: 'Добавить фильтр'
            property FilterDefinition fd: null
            property string role
            width: 400
            onFdChanged: addfilterdialoginsidesloader.sourceComponent = fd.delegate

            Loader {
                id: addfilterdialoginsidesloader
                anchors.left: parent.left
                anchors.right: parent.right


                height: parent.fd === null || item === null ? 60 : item.implicitHeight

                onLoaded: {

                    item.labeltext =  addFilterDialog.fd.delegateLabel
                    item.cr.forceActiveFocus()
                }
            }
            onVisibleChanged: if( visible ) addfilterdialoginsidesloader.item.cr.forceActiveFocus()

            standardButtons: StandardButton.Ok | StandardButton.Cancel
            onAccepted: {
                __proxyModel.addFilter( filterObject.createObject( __proxyModel, {
                                                                      role: role,
                                                                      criteria: addfilterdialoginsidesloader.item.criteria,
                                                                      filter: fd
                                                                  } ) )
                table.filters[ role ] = __proxyModel.filterCount( role )
                table.filtersChanged()
            }
        }

        Menu { // header context menu
            id: headerContextMenu
            MenuItem { // column title
                id: columnText
                enabled: false
                text: floatFilterBar.currentColumn !== -1 ? table.getColumn( floatFilterBar.currentColumn ).title : ''
            }
            MenuSeparator {}
            Menu { // filter types roster
                id: filtersMenu
                title: 'Добавить фильтр'
                Instantiator {
                    model: filterRepository.filters
                    onObjectAdded: filtersMenu.insertItem( index, object )
                    onObjectRemoved: filtersMenu.removeItem( object )
                    MenuItem {
                        text: modelData.text
                        visible:
                            if( table.getColumn( floatFilterBar.currentColumn ) !== null ) {
                                if( modelData.textFilterType ) {
                                    var numberType = ( table.getColumn( floatFilterBar.currentColumn ).type === 'int' || table.getColumn( floatFilterBar.currentColumn ).type === 'real' || table.getColumn( floatFilterBar.currentColumn ).type === 'date' )
                                    return !numberType
                                }else{
                                    var numberType = ( table.getColumn( floatFilterBar.currentColumn ).type === 'int' || table.getColumn( floatFilterBar.currentColumn ).type === 'real' || table.getColumn( floatFilterBar.currentColumn ).type === 'date' )
                                    return numberType
                                }
                            }else{ return false }

                        onTriggered: {
                            addFilterDialog.fd = modelData
                            addFilterDialog.role = table.getColumn( floatFilterBar.currentColumn ).role
                            addFilterDialog.open()
                        }
                    }
                }
            }
            Menu {
                id: currentFilters
                visible:  table.getColumn( floatFilterBar.currentColumn ) === null ? false : table.filters[ table.getColumn( floatFilterBar.currentColumn ).role ] !== undefined || table.filters[ table.getColumn( floatFilterBar.currentColumn ).role ] > 0
                title: table.getColumn( floatFilterBar.currentColumn ) === null ? 'Установленные фильтры' : 'Установленные фильтры (%1)'.arg( table.filters[ table.getColumn( floatFilterBar.currentColumn ).role ] )

                Instantiator { // current filters
                    model: __proxyModel.filters
                    onObjectAdded: currentFilters.insertItem( index, object )
                    onObjectRemoved: currentFilters.removeItem( object )
                    MenuItem {
                        visible: modelData.role === table.getColumn( floatFilterBar.currentColumn ).role
                        text: 'Убрать фильтр ' + modelData.filter.text + ' "' + modelData.criteria + '"'
                        onTriggered: {
                            table.filters[ modelData.role ] -= 1
                            table.filtersChanged()
                            __proxyModel.removeFilter( modelData )
                        }
                    }
                }
            }
            MenuItem {
                text: 'Сбросить все фильтры для этого столбца'
                visible: if( table.columnCount > 0 && table.getColumn( floatFilterBar.currentColumn ) !== null ) return table.filters[ table.getColumn( floatFilterBar.currentColumn ).role ] > 0
                         else return false
                onTriggered: {
                    var role = table.getColumn( floatFilterBar.currentColumn ).role
                    table.filters[ role ] = 0
                    table.filtersChanged()
                    __proxyModel.removeAllFilters( role )
                }
            }
            MenuSeparator {}
            Menu { // columns toggle menu
                id: headerMenuColumns
                title: 'Столбцы'
                Instantiator {
                    model: __dataModel.fieldData
                    onObjectAdded: {
//                        headerContextMenu.insertItem( index, object )
                        headerMenuColumns.insertItem( index, object )
                    }
                    onObjectRemoved: {
//                        headerContextMenu.removeItem( object )
                        headerMenuColumns.removeItem( object )
                    }
                    MenuItem {
                        visible: !modelData.hidden && modelData.name !== __dataModel.readableField
                        text: modelData.caption
                        checkable: true
                        checked: true
                        property string role: modelData.name
                        property var mylocaldata: modelData
                        onToggled: {
                            for( var i = 0; i < table.columnCount; ++i )
                                if( table.getColumn( i ).role === mylocaldata.name ) {
                                    table.getColumn( i ).visible = checked
                                    floatFilterBar.reset()
                                    break
                                }
                        }
                    }
                }
            }
            MenuItem { // column hide menu
                text: 'Скрыть столбец'
                enabled: floatFilterBar.currentColumn !== -1 ? table.getColumn( floatFilterBar.currentColumn ).role !== __dataModel.readableField : false
                onTriggered: {
                    table.getColumn( floatFilterBar.currentColumn ).visible = false
                    for( var i = 0; i < headerMenuColumns.items.length; ++i )
                        if( headerMenuColumns.items[ i ].role === table.getColumn( floatFilterBar.currentColumn ).role ){
                            headerMenuColumns.items[ i ].checked = false
                            break
                        }
                    floatFilterBar.reset()
                }
            }
        }

        Component { // table row delegate
            id: tableRowDelegate
            Rectangle {
                height: Math.round( table.__tableScale * 1.3 )
                color: styleData.selected ? '#3679d7' : styleData.alternate ? '#edf3fe': '#fff'
                MouseArea { // table row right click mouse area
                    anchors.fill: parent
                    enabled: __dataModel.editable
                    acceptedButtons: Qt.RightButton
                    onClicked: {
                        table.currentRow = styleData.row === undefined ? -1 : styleData.row
                        tableContextMenu.row = ( (styleData.row === undefined) ? -1 : styleData.row )
                        tableContextMenu.currentItemText = styleData.row === undefined ? '' :__proxyModel.getItemData( styleData.row )[ __dataModel.readableField ]
                        tableContextMenu.currentItemId = styleData.row === undefined ? '' :__proxyModel.getItemData( styleData.row )[ __dataModel.indexField ]
                        tableContextMenu.popup()
                        mouse.accepted = true
                    }
                }
            }
        }

        Menu { // table context menu
            id: tableContextMenu
            property int row: -1
            property alias currentItemText: tableContextMenuCurrentItemText.text
            property int currentItemId: -1
            MenuItem {
                id: tableContextMenuCurrentItemText
                enabled: false
                visible: tableContextMenu.currentItemId > 0
//                visible: tableContextMenuCurrentItemText.text !== ''
            }
            MenuSeparator {
                visible: tableContextMenu.currentItemId > 0
//                visible: tableContextMenuCurrentItemText.text !== ''
            }

            Instantiator {
                id: userRowContextMenu

                model: []
                onObjectAdded: {

                    tableContextMenu.insertItem( index, object )
                }
                onObjectRemoved: {
                    tableContextMenu.removeItem( object )
                }
                MenuItem {
                    text: modelData.text
                    onTriggered: {
                        if( modelData.pano ) {
                            var blob = __dataModel.readBlob( __proxyModel.item( tableContextMenu.row )['data'] )
                            console.log( 'pano =====>>>>>>>>', blob.dirx, blob.diry )
                            modelData.obj.position = '%1,%2'.arg(blob.pointx).arg(blob.pointy)
                            modelData.obj.dir = '%1,%2'.arg(blob.dirx).arg(blob.diry)
                            modelData.obj.show()
                        }else{

                            modelData.calcObj.ksname = tableContextMenuCurrentItemText.text
                            console.log('=======>>>>> ', __proxyModel.item( tableContextMenu.row )[ 'site' ] , __proxyModel.item( tableContextMenu.row )[ 'is_tram' ],__proxyModel.item( tableContextMenu.row )[ 'id' ] )
                            modelData.calcObj.istram = __proxyModel.item( tableContextMenu.row )[ 'is_tram' ]
                            var id = __proxyModel.item( tableContextMenu.row )[ 'id' ]
//                            console.log( ' ==>>>', id, calcModelProxy.getcount() )
                            calcModelSiteFilter.criteria = Number(id)

                            modelData.calcObj.onTriggered()
                        }
                    }
                }
            }
            MenuSeparator { visible: root.userFunctions.lenght > 0 }

            MenuItem {
                text: 'Добавить новую запись'
                onTriggered: {

                    editDialog.init( -1 )
                    editDialog.open()

                }
            }
            MenuItem {
//                enabled: ok.text !== ''
                visible: tableContextMenu.currentItemId > 0
//                visible: tableContextMenuCurrentItemText.text !== ''
                text: 'Редактировать запись'
                onTriggered: {
                    root.selectedData = __proxyModel.getItemData( table.currentRow )
                    editDialog.init( table.currentRow )
                    editDialog.open()
                }
            }
            MenuItem {
                visible: tableContextMenu.currentItemId > 0
                enabled: tableContextMenu.currentItemId > 0
//                enabled: tableContextMenuCurrentItemText.text !== ''
//                visible: tableContextMenuCurrentItemText.text !== ''
                text: 'Удалить запись'
                onTriggered: {
                    root.selectedData = __proxyModel.getItemData( table.currentRow )
                    editDialog.initr( table.currentRow )
                    editDialog.open()
                }
            }
        }

        Dialog {
            id: editDialog
            title: editDialogLoader.item.title
            standardButtons: StandardButton.Cancel | StandardButton.Ok
            width: cont.width + 25
            height: cont.height + 50 + 20
            property int row
            property int newRecord
            function init( row ){
                editDialog.row = row
                if( row === -1) {
//                    editDialogLoader.item.clearData()
                    newRecord = 0
                    editDialogLoader.item.init( 'Создание новой записи', 'Создать новую запись', __dataModel.fieldData, {} )
                }else{
//                    editDialogLoader.item.setData( __dataModel.item( row ) )
                    newRecord = 1
                    editDialogLoader.item.init( 'Редактирование записи', 'Редактировать запись', __dataModel.fieldData, __proxyModel.getItemData( row ) )
                }
                editDialogLoader.item.enabled = true
            }
            function initr( row ){
                editDialog.row = row
                newRecord = 2
                editDialogLoader.item.init( 'Удаление записи', 'Удалить запись', __dataModel.fieldData, __proxyModel.getItemData( row ) )
                editDialogLoader.item.enabled = false
            }

            ColumnLayout {
                id: cont
                Loader {
                    id: editDialogLoader
                    sourceComponent: defaultLay
                    onLoaded: {
                        item.fieldsModel = Qt.binding( function(){ return __dataModel.fieldData } )
                    }
                }
                Item{ height: 10 }
                GroupBox {
                    Layout.fillWidth: true
                    Layout.minimumHeight: note.paintedHeight + 25
                    Label {
                        id: note
                        text: editDialogLoader.item.note
                    }
                }
            }

            onAccepted: {
                var x = editDialogLoader.item.onAcceptFunction()

                console.log( 'accepted somethin', x, x.name, x.parent_id, x.si_id )

                if( newRecord == 0 ) {
                    __dataModel.addItem( x )
                }else if( newRecord == 1 ){
                    __dataModel.editItem( x )
                }else if( newRecord == 2 )
                    __dataModel.delItem( x )

                __dataModel.updateData()
            }
            onRejected: editDialogLoader.item.onRejectFunction()
        }
    }
}
