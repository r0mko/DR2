import QtQuick 2.5
import QtQuick.Controls 1.4
import Test 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.2

ColumnLayout{
    property alias datasource: datamodel.dataSource
    property alias model: datamodel
    property int currentSelection: 1
    property string selectedText: ''
//    onCurrentSelectionChanged: console.log(currentSelection)
    spacing: 0
    Rectangle {
        Layout.fillWidth: true
        height: 36
        color: 'transparent'
        RowLayout {
            anchors.fill: parent
            Item{ width:5 }
            BigButton {
                text: 'сбросить все фильтры'
                onClicked: {
                    datamodel.removeAllFilters()
                    datamodel.updateData()
                }
                fontSize: 14
                margin: 5
            }
            BigButton {
                text: 'test'
                onClicked: {
                    pm.test()
                }
                fontSize: 14
                margin: 5
            }
            // TODO: placeholder for outer components like filters and stuff ??
            Item{ Layout.fillWidth: true }
        }
    }


    TableView {
        id: root
        Layout.fillWidth: true
        Layout.fillHeight: true
//        model: datamodel
        model: pm
        rowDelegate: rowDelegateComponent
        headerDelegate: headerDelegateComponent

        onClicked: {
            console.log( 'selection changed', datamodel.item( row )[ datamodel.readableField ], datamodel.readableField, row )
            parent.currentSelection = Number( datamodel.item( row ).id )
            parent.selectedText = datamodel.item( row )[ datamodel.readableField ]

        }

        property alias datasource: datamodel.dataSource

        Component.onCompleted: {
            datamodel.fieldDataChanged.connect( updateViewColumns )
        }

        onSortIndicatorColumnChanged: {
            datamodel.sortField = getColumn( sortIndicatorColumn ).role
            datamodel.sortAscending = sortIndicatorOrder == Qt.AscendingOrder
            datamodel.updateData()
        }
        onSortIndicatorOrderChanged: {
            datamodel.sortField = getColumn( sortIndicatorColumn ).role
            datamodel.sortAscending = sortIndicatorOrder == Qt.AscendingOrder
            datamodel.updateData()
        }

        function updateViewColumns( a ){
            var setSort = root.columnCount > 0;
            while( root.columnCount > 0 ) root.removeColumn( 0 )
            for( var i = 0; i < a.length; ++i )
                if( !a[ i ][ 'hidden' ] )
                    if( a[ i ].hasOwnProperty( 'caption' ) && a[ i ].hasOwnProperty( 'name' ) )
                        if( a[ i ][ 'control' ] === 'bool' ) {
                            addColumn( boolColumn.createObject( root, { title: a[ i ][ 'caption' ], role: a[ i ][ 'name' ], filterType: a[ i ][ 'filter' ], width: a[ i ][ 'width' ] === undefined ? 50 : a[ i ][ 'width' ] } ) )
                        }
                        else if(a[ i ][ 'control' ] === 'transportType'){
                            addColumn( transportTypeColumn.createObject( root, { title: a[ i ][ 'caption' ], role: a[ i ][ 'name' ], filterType: a[ i ][ 'filter' ], width: a[ i ][ 'width' ] === undefined ? 50 : a[ i ][ 'width' ] } ) )
                        }
                        else if(a[ i ][ 'control' ] === 'date'){
                            addColumn( dateColumn.createObject( root, { title: a[ i ][ 'caption' ], role: a[ i ][ 'name' ], filterType: a[ i ][ 'filter' ], width: a[ i ][ 'width' ] === undefined ? 50 : a[ i ][ 'width' ] } ) )
                        }
                        else{
                            var x = addColumn( basicColumn.createObject( root, { title: a[ i ][ 'caption' ], role: a[ i ][ 'name' ], filterType: a[ i ][ 'filter' ], width: a[ i ][ 'width' ] === undefined ? 150 : a[ i ][ 'width' ] } ) )
                        }

            selection.select( 0 )
            selection.selectionChanged()
            parent.currentSelection = datamodel.item(0).id


        }

        ProxyModel {
            id: pm
            source:  datamodel


        }

        PartModel {
            id: datamodel
            modelService: service
            loadingText: 'загрузка...'
            windowSize: 5000
            onRowCountChanged: pm.invalidate()
        }

        Dialog {
            id: dialog
            width: 600
//            height: childrenRect.height
            property int id: -1
            ColumnLayout {
                width: parent.width
                Layout.fillWidth: true
//                Layout.fillHeight: true
                RowLayout {
                    Layout.fillWidth: true
//                    Layout.fillHeight: true
                    ColumnLayout {
                        id: layout
                        Layout.fillWidth: true
//                        Layout.fillHeight: true
                        property var rows: []
                    }
                }
                Label {
                    Layout.fillWidth: true
                }
                Label {
                    id: dialogRowName
                    Layout.fillWidth: true
                }
                Label {
                    id: dialogText
                    Layout.fillWidth: true
                }
                Label {
                    Layout.fillWidth: true
                    height: 20
                }
            }

            property string action: ''
            onApply: {
                var item = { values: [] }
                var i = 0
                var fn = datamodel.fieldData[ i ]
                if( action == 'add' ){
                    for( i = 0; i < layout.rows.length; ++i ) {
                        if( layout.rows[ i ].type === 'combo' ) item[ 'values' ].push( layout.rows[ i ].selectedRealIndex )
                        else item[ 'values' ].push( layout.rows[ i ].dataValue )
                    }
                    datamodel.addItem( item )
                    id = -1
                    action = ''
                    datamodel.updateData()
                }
                else if( action == 'edit' ) {
                    if( id == -1 ) console.log( 'id is wrong!!!!' )
                    //                var item = { values: [] }
                    for( i = 0; i < layout.rows.length; ++i ) {
                        if( layout.rows[ i ].type === 'combo' ) item[ 'values' ].push( layout.rows[ i ].selectedRealIndex )
                        else item[ 'values' ].push( layout.rows[ i ].dataValue )
                        console.log( item.values, id )
                    }
                    item[ 'values' ].push( id )
                    datamodel.editItem( item )
                    id = -1
                    action = ''
                    datamodel.updateData()
                }
                else if( action == 'del' ){
                    if( id == -1 ) console.log( 'id is wrong!!!!' )
                    datamodel.delItem( id )
                    id = -1
                    action = ''
                    datamodel.updateData()
                }
            }

            function initAdd(){
                action = 'add'
                standardButtons = StandardButton.Cancel | StandardButton.Apply
                var it = datamodel.item( root.currentRow )
                id = -1
                title = 'Вы точно хотите добавить запись?'
                dialogText.text = title
                dialogRowName.text = ''
                while( layout.rows.length > 0 ) layout.rows.pop().destroy()
                for( var i = 0; i < datamodel.fieldData.length; ++i ) {
                    var fn = datamodel.fieldData[ i ]

                    if( !fn.hasOwnProperty( 'control' ) ) continue
                    var x = fn[ 'control' ]
                    if( x === 'bool' || x === 'transportType' ) {
                        layout.rows.push( dialogRowBoolComponent.createObject( layout, {
                                                                                  labelText: fn[ 'caption' ]
                                                                              } ) )
                    }
                    else if( x === 'date' ) {
                        layout.rows.push( dialogRowDateComponent.createObject( layout, {
                                                                                  labelText: fn[ 'caption' ]
                                                                              } ) )
                    }
                    else if( x === 'text' ) {
                        layout.rows.push( dialogRowTextComponent.createObject( layout, {
                                                                                  labelText: fn[ 'caption' ]
                                                                              } ) )
                    }
                    else if( x === 'transportType' ) {
                        layout.rows.push( dialogRowTransportComponent.createObject( layout, {
                                                                                  labelText: fn[ 'caption' ]
                                                                              } ) )
                    }
                    else if( x === 'pass' ) {
                        layout.rows.push( dialogRowPassComponent.createObject( layout, {
                                                                                  labelText: fn[ 'caption' ]
                                                                              } ) )
                    }
                    else if( x === 'int' ) {
                        layout.rows.push( dialogRowIntComponent.createObject( layout, {
                                                                                 labelText: fn[ 'caption' ],
                                                                                 dataValue: 2014,
                                                                                 min:  fn[ 'min' ],
                                                                                 max:  fn[ 'max' ]
                                                                             } ) )
                    }
                    else if( x === 'combo' ) {
                        layout.rows.push( dialogRowComboComponent.createObject( layout, {
                                                                                   labelText: fn[ 'caption' ],
                                                                                   datasource: fn[ 'datasource' ],
                                                                                   textRole: fn[ 'textRole' ],
                                                                                   // TODO: dataValue chould be set somewhere else like from settings
                                                                                   dataValue: 2
                                                                               } ) )
                    }
                    else if( x === 'comboEdit' ) {
                        layout.rows.push( dialogRowComboTextComponent.createObject( layout, {
                                                                                   labelText: fn[ 'caption' ],
                                                                                   datasource: fn[ 'datasource' ],
                                                                                   textRole: fn[ 'textRole' ],
                                                                                   // TODO: dataValue chould be set somewhere else like from settings
                                                                                   dataValue: 2
                                                                               } ) )
                    }
                }
                open()
            }
            function initEdit(){
                action = 'edit'
                standardButtons = StandardButton.Cancel | StandardButton.Apply
                var it = datamodel.item( root.currentRow )
                id = it[ datamodel.indexField ]
                title = 'Вы точно хотите изменить запись?'
                dialogText.text = title
                dialogRowName.text = it[ datamodel.readableField ]
                while( layout.rows.length > 0 ) layout.rows.pop().destroy()
                for( var i = 0; i < datamodel.fieldData.length; ++i ) {
                    var fn = datamodel.fieldData[ i ]
                    if( !fn.hasOwnProperty( 'control' ) ) continue
                    var x = fn[ 'control' ]
                    if( x === 'bool' ) {
                        layout.rows.push( dialogRowBoolComponent.createObject( layout, {
                                                                                  labelText: fn[ 'caption' ],
                                                                                  dataValue: it[ fn[ 'name' ] ]
                                                                              } ) )
                    }
                    else if( x === 'date' ) {
                        layout.rows.push( dialogRowDateComponent.createObject( layout, {
                                                                                  labelText: fn[ 'caption' ],
                                                                                  dataValue: it[ fn[ 'name' ] ]
                                                                              } ) )
                    }
                    else if( x === 'text' ) {
                        layout.rows.push( dialogRowTextComponent.createObject( layout, {
                                                                                  labelText: fn[ 'caption' ],
                                                                                  dataValue: it[ fn[ 'name' ] ]
                                                                              } ) )
                    }
                    else if( x === 'transportType' ) {
                        layout.rows.push( dialogRowTransportComponent.createObject( layout, {
                                                                                  labelText: fn[ 'caption' ]//,
//                                                                                  dataValue: it[ fn[ 'name' ] ]
                                                                              } ) )
                    }
                    else if( x === 'pass' ) {
                        layout.rows.push( dialogRowPassComponent.createObject( layout, {
                                                                                  labelText: fn[ 'caption' ],
                                                                                  dataValue: it[ fn[ 'name' ] ]
                                                                              } ) )
                    }
                    else if( x === 'int' ) {
                        layout.rows.push( dialogRowIntComponent.createObject( layout, {
                                                                                 labelText: fn[ 'caption' ],
                                                                                 dataValue: it[ fn[ 'name' ] ],
                                                                                 min:  fn[ 'min' ],
                                                                                 max:  fn[ 'max' ]

                                                                             } ) )
                    }
                    else if( x === 'combo' ) {
                        layout.rows.push( dialogRowComboComponent.createObject( layout, {
                                                                                   labelText: fn[ 'caption' ],
                                                                                   datasource: fn[ 'datasource' ],
                                                                                   textRole: fn[ 'textRole' ],
                                                                                   dataValue: datamodel.listIndex( it[ fn[ 'id' ] ], fn[ 'id' ] )
                                                                               } ) )
                    }
                }
                open()
            }
            function initDelete(){
                action = 'del'
                standardButtons = StandardButton.Cancel | StandardButton.Apply
                var it = datamodel.item( root.currentRow )
                id = it[ datamodel.indexField ]
                console.log( 'indexField', id, datamodel.indexField )
                title = 'Вы точно хотите удалить запись?'
                dialogText.text = title
                dialogRowName.text = it[ datamodel.readableField ]
                while( layout.rows.length > 0 ) layout.rows.pop().destroy()
                open()
            }
            Component {
                id: dialogRowDateComponent
                RowLayout {
                    id: row
                    Layout.fillWidth: true
                    property alias labelText: label.text
                    property alias dataValue: data.date
                    readonly property string type: 'text'
                    //                property var dataText
                    Label {
                        id: label
                    }
                    Item{
                        Layout.fillWidth: true
                    }
                    DatePicker {
                        id: data
                    }
//                    CalendarComponent {
//                        id: data
//                        labelVisible: false
//                    }
                }
            }
            Component {
                id: dialogRowTextComponent
                RowLayout {
                    id: row
                    Layout.fillWidth: true
                    property alias labelText: label.text
                    property alias dataValue: data.text
                    readonly property string type: 'text'
                    //                property var dataText
                    Label {
                        id: label
                    }
                    Item{
                        Layout.fillWidth: true
                    }
                    TextField {
                        id: data
                        implicitWidth: 400
                    }
                }
            }
            Component {
                id: dialogRowTransportComponent
                RowLayout {
                    id: row
                    Layout.fillWidth: true
                    property alias labelText: label.text
//                    property alias dataValue: data.text
                    readonly property string type: 'text'
                    //                property var dataText
                    Label {
                        id: label
                    }
                    Item{
                        Layout.fillWidth: true
                    }
                    ExclusiveGroup{ id: tr }
                    RadioButton {
                        text: 'троллейбус'
                        checked: true
                        exclusiveGroup: tr
                    }
                    RadioButton {
                        exclusiveGroup: tr
                        text: 'трамвай'
                    }
//                    TextField {
//                        id: data
//                        implicitWidth: 400
//                    }
                }
            }
            Component {
                id: dialogRowPassComponent
                RowLayout {
                    id: row
                    Layout.fillWidth: true
                    property alias labelText: label.text
                    property alias dataValue: data.text
                    readonly property string type: 'pass'
                    //                property var dataText
                    Label {
                        id: label
                    }
                    Item{
                        Layout.fillWidth: true
                    }
                    TextField {
                        id: data
                        implicitWidth: 400
                        echoMode: TextInput.Password
                    }
                }
            }
            Component {
                id: dialogRowBoolComponent
                RowLayout {
                    id: row
                    Layout.fillWidth: true
                    property alias labelText: label.text
                    property alias dataValue: data.checked
                    readonly property string type: 'bool'
                    Label {
                        id: label
                    }
                    Item{
                        Layout.fillWidth: true
                    }
                    CheckBox {
                        id: data
                        implicitWidth: 400
                    }
                }
            }
            Component {
                id: dialogRowIntComponent
                RowLayout {
                    id: row
                    Layout.fillWidth: true
                    property alias labelText: label.text
                    property alias dataValue: data.value
                    property alias min: data.minimumValue
                    property alias max: data.maximumValue
                    readonly property string type: 'int'
                    Label {
                        id: label
                    }
                    Item{
                        Layout.fillWidth: true
                    }
                    SpinBox {
                        id: data
                        implicitWidth: 400

                    }
                }
            }
            Component {
                id: dialogRowComboComponent
                RowLayout {
                    id: row
                    Layout.fillWidth: true
                    property alias labelText: label.text
                    property alias datasource: model.dataSource
                    property alias dataValue: data.currentIndex
                    property alias textRole: data.textRole
                    readonly property string type: 'combo'
                    property int selectedRealIndex: -1

                    Label {
                        id: label
                    }
                    Item{
                        Layout.fillWidth: true
                    }
                    ComboBox {
                        id: data
                        implicitWidth: 400
                        model: model
                        onCurrentIndexChanged: {
                            row.selectedRealIndex = model.realIndex( currentIndex, model.indexField )
                            console.log( currentIndex, model.realIndex( currentIndex, model.indexField ) )
                        }
                    }
                    PartModel {
                        id: model
                        modelService: service
                    }
                }
            }
            Component {
                id: dialogRowComboTextComponent
                RowLayout {
                    id: row
                    Layout.fillWidth: true
                    property alias labelText: label.text
                    property alias datasource: model.dataSource
                    property alias dataValue: data.currentIndex
                    property alias textRole: data.textRole
                    readonly property string type: 'combo'
                    property int selectedRealIndex: -1

                    Label {
                        id: label
                    }
                    Item{
                        Layout.fillWidth: true
                    }
                    ComboBox {
                        id: data
                        implicitWidth: 400
                        model: model
                        editable: true

                        onCurrentIndexChanged: {
                            row.selectedRealIndex = model.realIndex( currentIndex, model.indexField )
                            console.log( currentIndex, model.realIndex( currentIndex, model.indexField ) )
                        }
                        onEditTextChanged: {
//                        onAccepted: {
                            if( editText == '' ) model.removeFilter( 'text' )
                            else model.addFilter( 'text', editText )
                            model.updateData()
                        }
                    }
                    PartModel {
                        id: model
                        modelService: service
                    }
                }
            }

        }

        Menu {
            id: contextMenu

            MenuItem {
                id: nameid
                enabled: false
                text: String( datamodel.item( root.currentRow )[ datamodel.readableField ] )
            }
            MenuSeparator {}
            MenuItem {
                text: qsTr('Добавить запись')
                onTriggered: dialog.initAdd()
            }
            MenuItem {
                text: qsTr('Редактировать запись')
                onTriggered: dialog.initEdit()
            }
            MenuItem {
                text: qsTr('Удалить запись')
                onTriggered: dialog.initDelete()
            }
        }

        Component {
            id: rowDelegateComponent
            Item {
                height: 24
                Rectangle {
                    anchors {
                        left: parent.left
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                    height: parent.height
                    color: styleData.selected ? '#805566cc' : styleData.alternate ? '#f4f4f4' : 'white'
                    MouseArea {
                        anchors.fill: parent
                        enabled: datamodel.editable
                        acceptedButtons: Qt.RightButton
                        onPressed: {
                            if (typeof styleData.row === 'number') {
                                root.currentRow = styleData.row
                                if (mouse.button === Qt.RightButton) contextMenu.popup()
                            }
                            mouse.accepted = false
                        }
                        onClicked: {
                            if( contextMenu.visible ) contextMenu.visible = false
                            mouse.accepted = false
                        }
                        onReleased: {
                            if( contextMenu.visible ) contextMenu.visible = false
                            mouse.accepted = false
                        }
                    }
                }
            }
        }
        Component {
            id: basicColumn
            TableViewColumn {
                // TODO: on creation load width and position if any
                property int position
                property string filterType
                onWidthChanged: console.log( width )
                delegate: Text {
                    clip: true
//                    anchors.fill: parent
                    anchors.leftMargin: 5
                    anchors.rightMargin: 5
                    elide: Text.ElideMiddle
//                    Layout.fillWidth: true
//                    Layout.fillHeight: true
                    verticalAlignment: Text.AlignVCenter
                    text: String( styleData.value )
                    font.pixelSize: 18
                }
            }
        }
        Component {
            id: dateColumn
            TableViewColumn {
                // TODO: on creation load width and position if any
                property int position
                property string filterType
                onWidthChanged: console.log( width )
                delegate: Text {
                    clip: true
                    anchors.fill: parent
                    anchors.leftMargin: 5
                    anchors.rightMargin: 5
                    elide: Text.ElideMiddle
//                    Layout.fillWidth: true
//                    Layout.fillHeight: true
                    verticalAlignment: Text.AlignVCenter


                    text: Qt.formatDate( styleData.value, 'dd.MM.yyyy' )
                    font.pixelSize: 18
                }
            }
        }
        Component {
            id: transportTypeColumn
            TableViewColumn {
                // TODO: on creation load width and position if any
                property int position
                property string filterType
                onWidthChanged: console.log( width )
                delegate: Text {
                    clip: true
//                    anchors.fill: parent
                    anchors.leftMargin: 5
                    anchors.rightMargin: 5
                    elide: Text.ElideMiddle
//                    Layout.fillWidth: true
//                    Layout.fillHeight: true
                    verticalAlignment: Text.AlignVCenter
                    text: String( styleData.value == true ? 'т-й' : 'т-с'  )
                    font.pixelSize: 18
                }
            }
        }
        Component {
            id: boolColumn
            TableViewColumn {
                property int position
                property string filterType
                delegate: CheckBox{
                    enabled: false
                    visible: styleData.value === undefined ? false : true
                    checked: styleData.value === undefined ? false : styleData.value
                }
                onWidthChanged: console.log( width )
            }
        }
        Component {
            id: headerDelegateComponent
            Rectangle {
                id: headerCell
                clip: true
                height: textItem.implicitHeight + textItem.anchors.topMargin + textItem.anchors.bottomMargin
                width: textItem.implicitWidth

                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#fff" }
                    GradientStop { position: 1.0; color: "#ddd" }
                }
                Connections {
                    target: styleData
                    onContainsMouseChanged: {
                        //                    console.log( 'header contains mouse changed', styleData.containsMouse )
                        if (styleData.containsMouse) {
                            var pos = root.mapFromItem(headerCell.parent)
                            var x = pos.x
                            var y = pos.y

                            floatFilterBar.x = pos.x
                            floatFilterBar.y = pos.y
                            floatFilterBar.width = headerCell.parent.width
                            floatFilterBar.height = headerCell.parent.height
                            floatFilterBar.currentColumn = styleData.column
                            floatFilterBar.visible = root.getColumn( floatFilterBar.currentColumn ).filterType !== undefined && root.getColumn( floatFilterBar.currentColumn ).filterType !== "none"
                            //                        console.log( 'filtertype: ', root.getColumn( floatFilterBar.currentColumn ).filterType )
                            floatFilterBar.opacity = 1
                        }
                    }
                }

                Text {
                    id: textItem
                    anchors.fill: parent
                    anchors.topMargin: 3
                    anchors.bottomMargin: 3
                    anchors.rightMargin: 33
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: styleData.textAlignment
                    anchors.leftMargin: 5
                    text: styleData.value
                    elide: Text.ElideRight
                    renderType: Text.NativeRendering
                    font.pixelSize: 14
                }
                Image {
                    id: filterIndicator
                    anchors.right: sortIndicator.left
                    scale: 0.75
                    y: ( parent.height - 16 ) * 0.5
                    width: 16
                    height: 16
                    visible: root.getColumn( styleData.column ) !== null ? datamodel.hasFilter( root.getColumn( styleData.column  ).role ) : false
                    source: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAABHNCSVQICAgIfAhkiAAAAAlwSFlzAAAAOgAAADoBai3sFgAAABl0RVh0U29mdHdhcmUAd3d3Lmlua3NjYXBlLm9yZ5vuPBoAAACnSURBVDiNvZExCoNAEEWfYmNn6/1yitzCJgcIXsDC3kIkrXcIBDxCAvlpRtnEjWxWyIeF2c+8t8UCNIAiT5MCA/EZkARQRbxeSWIWJED9A1wDySIwSQa0AXALZAs3DybJgX4D7oH8jXEvJimA0QOPQLHa/yxMUgKTA09A6dtNfX8j6QZ0TtVZt4pXYHl+mYMFQfmb4L5H8ABOewQHSZcYwRU4Sjpv2V/4Sbqe9VUBsAAAAABJRU5ErkJgggc0495ec3b4dec02323da115321b64574"
                }
                Image {
                    id: sortIndicator
                    anchors.right: spacerheader.left
                    scale: 0.75
                    y: ( parent.height - 16 ) * 0.5
                    width: 16
                    height: 16
                    visible: root.sortIndicatorColumn === styleData.column
                    rotation: root.sortIndicatorOrder === Qt.DescendingOrder ? 0 : 180
                    source: 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAABHNCSVQICAgIfAhkiAAAAAlwSFlzAAAAggAAAIIBsKhZvgAAABl0RVh0U29mdHdhcmUAd3d3Lmlua3NjYXBlLm9yZ5vuPBoAAADISURBVDiNjZK9DcIwEEZfB02akAHYIn/MwG4ZhZIqExB+BoiYAClJEVGZ5iydLJ9D8VV379nyZ5xzxALkwEWSm3sJeACcZLAkMfgA3AX8SkxJDH4o+CwxJUlYzUyJXyiAZwzeknj4ZZycAVlKAtAbcAl8JKUhuSJXXwO4AiZV4wRUgWQFbgB7oFDDWsFviZfUaq8AduFD1cCs4KPES2YtCWtsQljNQkkT1thacELS+hpPwJKCDckiLOM/sCEZATr5C5twIOmB7gcWe5oqyhsj1QAAAABJRU5ErkJggg79c994811fbb55b052c3b498da115bb8'
                }
                Item {
                    id:spacerheader
                    anchors.right: parent.right
                    width:5
                }

                Rectangle {
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.bottomMargin: 1
                    height: 1
                    color: "#a8a8a8"
                }
                Rectangle {
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.topMargin: 1
                    height: 1
                    color: "#bbb"
                }
                Rectangle {
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 1
                    anchors.topMargin: 1
                    width: 1
                    color: "#ccc"
                }
            }
        }

        Rectangle {
            id: floatFilterBar
            property int currentColumn
            opacity: 0
            color: 'transparent'
//            color: '#4055ff55'
            Behavior on opacity { NumberAnimation { duration: 100 } }

            MouseArea {
                id: mousearea
                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons: Qt.RightButton
                onContainsMouseChanged: containsMouse ? parent.opacity = 1 : parent.opacity = 0
                onClicked: {
                    if (mouse.button == Qt.RightButton) {
                        filterMenu.popup()
                    }
                }
            }

            Menu {
                id: filterMenu
                Menu {
                    title: 'Добавить фильтр'
                    MenuItem {
                        text: 'фильтр по тексту'
                    }
                    MenuItem {
                        text: 'больше или равно'
                    }
                    MenuItem {
                        text: 'меньше или равно'
                    }
                }
                MenuSeparator {}
                MenuItem {
                    text: 'убрать фильтр по тексту "ко"'
                }
            }
        }

        Dialog {
            id: filterDialog
            width: 529
            height: 350
            standardButtons: StandardButton.Ok | StandardButton.Reset | StandardButton.Cancel
            property string currentFilterName

            function init( role, currentFilter ) {
                while( filterRowsLayout.filterRows.length > 0 ) filterRowsLayout.filterRows.pop().destroy()
                var filterType = datamodel.fieldData[ datamodel.fieldDataIndexByRole( role ) ][ 'filter' ]


                var filter = datamodel.getFilter( role )
                currentFilterName = role
//                console.log( 'filter settings by role', role, filter, filter.value, filter.type )
                if( filterType === 'string' ) {
                    height = 100
                    filterRowsLayout.filterRows.push( filterStringComp.createObject( filterRowsLayout, { currentFilter: filter } ) )
                }
                else if( filterType === 'bool' ) {
                    height = 100
                    filterRowsLayout.filterRows.push( filterBoolComp.createObject( filterRowsLayout, { currentFilter: filter } ) )
                }
                else if( filterType === 'dateRange' ) {
                    height = 370
                    filterRowsLayout.filterRows.push( filterDateRangeComp.createObject( filterRowsLayout, { currentFilter: filter } ) )
                }
                else if( filterType === 'combo' ) {
                    height = 150
                    filterRowsLayout.filterRows.push( filterComboComp.createObject( filterRowsLayout, { currentFilter: filter, ds: datamodel.fieldData[ datamodel.fieldDataIndexByRole( role ) ][ 'comboModel' ] } ) )

                }

                else return
                open()
            }

            onRejected: {
                while( filterRowsLayout.filterRows.length > 0 ) filterRowsLayout.filterRows.pop().destroy()
            }
            onAccepted: {
                // TODO: set filter pass some column so we can manage icons and filter settings could be stored in column
                var t = filterRowsLayout.filterRows[ 0 ].filterObject()
                if( t === null ) {
                    while( filterRowsLayout.filterRows.length > 0 ) filterRowsLayout.filterRows.pop().destroy()
                    return
                }

                t[ 'field' ] = currentFilterName
                datamodel.addFilter( currentFilterName, t )
                //            filterRowsLayout.filterRows[ 0 ].hasFilter = true

                while( filterRowsLayout.filterRows.length > 0 ) filterRowsLayout.filterRows.pop().destroy()
            }
            onReset: {
                // TODO: clear filter reset icons refresh data
                //            filterswitch.checked = false
                //            filterRowsLayout.filterRows[ 0 ].hasFilter = false
                datamodel.removeFilter( currentFilterName )
                while( filterRowsLayout.filterRows.length > 0 ) filterRowsLayout.filterRows.pop().destroy()
                // remove current filter
            }
            RowLayout {
                id: filterRowsLayout
                property var filterRows: [] // TODO: do we need array here?

            }


            Component {
                id: filterBoolComp
                RowLayout {
                    id: root
                    property var column: null
                    property alias value: bf.checked

                    property var currentFilter
                    function filterObject(){
                        return { 'type': 'string', 'value': bf.text }
                    }
                    Label {
                        text: 'Текстовый фильтр'
                    }
                    CheckBox {
                        id: bf
                        checked: root.currentFilter.value === undefined ? false : root.currentFilter.value
                    }
                    Component.onCompleted: console.log( 'filterSting creted with passed stuff', currentFilter, currentFilter.value, currentFilter.type )
                }
            }
            Component {
                id: filterStringComp
                RowLayout {
                    id: root
                    property var column: null
                    property alias value: tf.text

                    property var currentFilter
                    function filterObject(){
                        return { 'type': 'string', 'value': tf.text }
                    }
                    Label {
                        text: 'Текстовый фильтр'
                    }
                    TextField {
                        id: tf
                        placeholderText: 'фильтр'
                        text: root.currentFilter.value === undefined ? '' : root.currentFilter.value
                    }
                }
            }
            Component {
                id: filterComboComp
                ColumnLayout {
                    id: root
                    property var column: null
                    property alias value: tf.currentText
                    property alias ds:model.dataSource
                    property alias all: en.checked
                    property var currentFilter
                    function filterObject(){
                        return { 'type': 'string', 'value': tf.currentText }
                    }
                    Label {
                        text: 'Комбофильтр'
                    }
                    CheckBox {
                        id: en
                        text: 'Все'
                        checked: root.currentFilter.value === undefined
                    }
                    ComboBox {
                        id: tf
                        enabled: !en.checked
                        model: PartModel {
                            id: model
                            modelService: service
                        }
                    }
                }
            }
            Component {
                id: filterDateRangeComp

                ColumnLayout {
                    id: root
                    property alias value1: dateFrom.date
                    property alias value2: dateTo.date
                    property var column: null
                    ComboBox {
                        id: combo
                        implicitWidth: 400
                        model: [
                            'без фильтра',
                            'конкретная дата',
                            'больше или равно',
                            'меньше или равно',
                            'диапазон'
                        ]
                    }
                    RowLayout {
                        CalendarComponent {
                            id: dateFrom
                            visible: combo.currentIndex !== 0
                            labelText: combo.currentIndex === 4 ? 'начало диапазона' : ''
                            maximumDate: combo.currentIndex === 4 ? dateTo.date : '2050-01-01'
                        }
                        CalendarComponent {
                            id: dateTo
                            visible: combo.currentIndex === 4
                            labelText: combo.currentIndex === 4 ? 'конец диапазона' : ''
                            minimumDate: combo.currentIndex === 4 ? dateFrom.date : '1901-01-01'
                        }
                    }
                }
            }
        }
    }
}
