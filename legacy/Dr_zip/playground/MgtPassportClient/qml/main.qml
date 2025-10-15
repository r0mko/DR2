/*



UPDATE `mgt2`.`class` SET `parent_id`='63' WHERE `id`='63';
UPDATE `mgt2`.`class` SET `parent_id`='64' WHERE `id`='64';
UPDATE `mgt2`.`class` SET `parent_id`='65' WHERE `id`='65';
UPDATE `mgt2`.`class` SET `parent_id`='65' WHERE `id`='66';
UPDATE `mgt2`.`class` SET `parent_id`='65' WHERE `id`='67';
UPDATE `mgt2`.`class` SET `parent_id`='65' WHERE `id`='68';
UPDATE `mgt2`.`class` SET `parent_id`='69' WHERE `id`='69';
UPDATE `mgt2`.`class` SET `parent_id`='69' WHERE `id`='70';
UPDATE `mgt2`.`class` SET `parent_id`='69' WHERE `id`='71';
UPDATE `mgt2`.`class` SET `parent_id`='69' WHERE `id`='72';
UPDATE `mgt2`.`class` SET `parent_id`='73' WHERE `id`='73';
UPDATE `mgt2`.`class` SET `parent_id`='74' WHERE `id`='74';








*/

import Dr.T2C 1.1
import QtQuick 2.5
import QtQuick.Window 2.2
import Test 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
Window {
    id: mainWindow
    property string programName: 'Система паспортизации'
    property string programVersion: '0.1a'
    color: '#888'

    /*release*/
    //Component.onCompleted: showFullScreen()
    Component.onCompleted: showMaximized()
    /**/

    FilterRepository{
        id: filterRepository
        FilterDefinition {
            id: f_greater
            text: 'Больше'
            compareFunction: function() { return this.value > this.criteria }
            readonly property bool textFilterType: false
            property string delegateLabel: 'Отфильтровать значения больше чем:'
            property Component delegate: intfilterdialoginsides
        }
        FilterDefinition {
            id: f_less
            text: 'Меньше'
            compareFunction: function() { return this.value < this.criteria }
            readonly property bool textFilterType: false
            property string delegateLabel: 'Отфильтровать значения меньше чем:'
            property Component delegate: intfilterdialoginsides
        }
        FilterDefinition {
            id: f_greaterEqual
            text: 'Больше или равно'
            compareFunction: function() { return this.value >= this.criteria }
            readonly property bool textFilterType: false
            property string delegateLabel: 'Отфильтровать значения больше или равно:'
            property Component delegate: intfilterdialoginsides
        }
        FilterDefinition {
            id: f_lessEqual
            text: 'Меньше или равно'
            compareFunction: function() { return this.value <= this.criteria }
            readonly property bool textFilterType: false
            property string delegateLabel: 'Отфильтровать значения меньше или равно:'
            property Component delegate: intfilterdialoginsides
        }
        FilterDefinition {
            id: f_equal
            text: 'равно'
            compareFunction: function() { return this.value == this.criteria }
            readonly property bool textFilterType: false
            property string delegateLabel: 'Отфильтровать значения равные:'
            property Component delegate: intfilterdialoginsides
        }
        FilterDefinition {
            id: f_substring
            text: 'По тексту'
            compareFunction: function() { return this.value.toLowerCase().indexOf( this.criteria.toLowerCase() ) >= 0 }
            readonly property bool textFilterType: true
            property string delegateLabel: 'Фильтровать по содержанию строки'
            property Component delegate: textfilterdialoginsides
        }
        FilterDefinition {
            text: 'По отсутствию текста'
            compareFunction: function() { return this.value.toLowerCase().indexOf( this.criteria.toLowerCase() ) < 0 }
            readonly property bool textFilterType: true
            property string delegateLabel: 'Фильтровать по отсутствию строки'
            property Component delegate: textfilterdialoginsides
        }
        FilterDefinition {
            id: f_regexp
            text: 'Регулярное выражение'
            compareFunction: function() { return RegExp( this.criteria, 'gi' ).test( this.value ) }
            readonly property bool textFilterType: true
            property string delegateLabel: 'Фильтровать используя регулярные выражения'
            property Component delegate: textfilterdialoginsides
        }
    }
    Component {
        id: textfilterdialoginsides
        ColumnLayout {
            property alias criteria: cr.text
            property alias labeltext: label.text
            property alias cr: cr
            anchors.fill: parent
            Label {
                id: label
                Layout.fillWidth: true
            }
            TextField {
                id: cr
                Layout.fillWidth: true
            }
        }
    }
    Component {
        id: intfilterdialoginsides
        ColumnLayout {
            property alias criteria: cr.value
            property alias labeltext: label.text
            property alias cr: cr
            anchors.fill: parent
            Label {
                id: label
                Layout.fillWidth: true
            }
            SpinBox {
                id: cr
                Layout.fillWidth: true
                minimumValue: -99999999999999
                maximumValue: 99999999999999
                value: 0
            }
        }
    }

    PrintView  {
        id: printView
    }

    Userdata {
        id: userdata
        file: '~/user.dat'
    }

    QtObject {
        id: uiStyle
        property int fontSizeBig: 25
        property int fontSizeMed: 18
        property int fontSizeNormal: 14
        property int fontSizeSmall: 12
        //        property color colorBGDefault: '#dcdcdc'
        //        property color colorFGDefault: '#000'
    }

    Service {
        id: service
        host: '192.168.1.11'
//        host: '10.0.0.61'
        port: 33667
//        onAuthenticatedChanged: console.log( 'auth changed', authenticated, isConnected, databaseOnline )

    }

    PartModel {
        id: treeModel
        modelService: service
        dataSource: 'tree'
        windowSize: 5000
    }
    PartModel {
        id: usersModel
        modelService: service
        dataSource: 'users'
        windowSize: 5000
    }
    PartModel {
        id: treeSite
        modelService: service
        dataSource: 'treeSite'
        windowSize: 5000
    }
    PartModel {
        id: siteModel
        modelService: service
        dataSource: 'site'
        windowSize: 5000
    }
    PartModel {
        id: elementsModel
        modelService: service
        dataSource: 'elements'
        windowSize: 5000
        onDataUpdated: calcModel.updateData()
    }
    PartModel {
        id: repairs_dataModel
        modelService: service
        dataSource: 'repairs_data'
        windowSize: 5000
    }
    PartModel {
        id: meter_dataModel
        modelService: service
        dataSource: 'meter_data'
        windowSize: 5000
        onDataUpdated: calcModel.updateData()
    }
    PartModel {
        id: cn_areaModel
        modelService: service
        dataSource: 'cn_area'
        windowSize: 5000
    }
    PartModel {
        id: catenariesModel
        modelService: service
        dataSource: 'catenaries'
        windowSize: 5000
    }
    PartModel {
        id: el_classModel
        modelService: service
        dataSource: 'el_class'
        windowSize: 5000
    }
    PartModel {
        id: el_typeModel
        modelService: service
        dataSource: 'el_type'
        windowSize: 5000
    }
    PartModel {
        id: repair_typeModel
        modelService: service
        dataSource: 'repair_type'
        windowSize: 5000
    }

    PartModel {
        id: siModel
        modelService: service
        dataSource: 'si'
        windowSize: 5000
    }

    PartModel {
        id: omkum_p1Model
        modelService: service
        dataSource: 'omkum_p1'
        windowSize: 5000
    }

    PartModel {
        id: omkum_p2Model
        modelService: service
        dataSource: 'omkum_p2'
        windowSize: 5000
    }
    PartModel {
        id: rolesModel
        modelService: service
        dataSource: 'roles'
        windowSize: 5000
    }
    PartModel {
        id: el_siteModel
        modelService: service
        dataSource: 'el_site'
        windowSize: 5000
    }
    PartModel {
        id: calcModel
        modelService: service
        dataSource: 'calc'
        windowSize: 5000
    }
    ProxyModel {
        id: calcModelProxy
        source: calcModel
//        onFiltersChanged: console.log( 'onFiltersChanged', getcount() )
        FilterObject {
            id: calcModelSiteFilter
            criteria: -1
            role: 'site_id'
            filter: f_equal

            onCriteriaChanged: {
                calcModelProxy.invalidate()
                console.log( 'criteria changed', criteria, calcModelProxy.getcount() )
    //                        calcModelProxy.removeAllFilters()
    //                        calcModelProxy.addFilter(calcModelSiteFilter)
            }

        }
    }




    Component {
        id: defaultLay
        ColumnLayout {
            id: root
            property string title: 'title'
            property string note: 'note'
            property alias fieldsModel: re.model
            property var rowData
            //            onWidthChanged: console.log( 'lay width changed', width, implicitWidth )
            //            onImplicitWidthChanged: console.log( 'lay implicitWidth changed', width, implicitWidth )
            //            Component.onCompleted: console.log( 'lay loaded its width is', width, implicitWidth )
            //            onFieldsModelChanged: console.log( 'fields model changed ')
            function onAcceptFunction() {
                console.log( 'accept' )
                var returnData = {}
                for( var i = 0; i < re.count; ++i ) {
                    var t = re.itemAt( i ).item === null ? rowData[ fieldsModel[ i ].name ] : re.itemAt( i ).item.value


                    returnData[ fieldsModel[ i ].name ] = t
                    console.log( 'item', i, fieldsModel[ i ].name, t, returnData[ fieldsModel[ i ].name ]  )
                }
                clearData()
                return returnData
            }
            function onRejectFunction() {
                console.log( 'reject' )
                //                for( var i = 0; i < re.count; ++i ) {
                //                    console.log( 'item', i, fieldsModel[ i ].name,
                //                                re.itemAt( i ).item === null ? rowData[fieldsModel[ i ].name] === undefined ? 'null' : rowData[fieldsModel[ i ].name] : re.itemAt( i ).item.value
                //                                )
                //                }

                clearData()
            }

            function init(title, note, fields, data){
                clearData()
                root.title = title
                root.note = note
                root.rowData = data
                rowDataChanged()
                root.fieldsModel = fields
                //                fieldsModelChanged()
                //                if( data.length === 0 )
                //                    for( var i = 0; i < re.count; ++i ) {
                //                        re.itemAt( i ).item.value = ''
                //                    }
            }
            //            onRowDataChanged: {
            //                fieldsModelChanged()
            //                re.model = fieldsModel
            //            }

            onRowDataChanged: {
                for( var i = 0; i < re.count; ++i ) {
                    var it = re.itemAt( i ).item
                    if( it !== null ) {
                        if( fieldsModel[ i ].formDelegate === 'combo' ) {
                            //                            console.log( '===>> ', fieldsModel[ i ].name, rowData[ fieldsModel[ i ].localIdField ] )
                            it.value = rowData[ fieldsModel[ i ].localIdField ]
                        }else{
                            //                            console.log( '===>> ', fieldsModel[ i ].name, rowData[ fieldsModel[ i ].name ] )
                            it.value = rowData[ fieldsModel[ i ].name ]
                        }
                    }
                }
            }

            function setData( data ){
                rowData = data
                rowDataChanged()
            }
            function clearData() {
                rowData = undefined
                rowDataChanged()
                //                re.model = null
            }

            Repeater {
                id: re
//                onModelChanged:  console.log( 're model changed', model.length )
                Loader {
                    source: if( modelData.formDelegate === 'text' )     return 'FormTextDelegate.qml'
                            else if( modelData.formDelegate === 'bool' )     return 'FormBoolDelegate.qml'
                            else if( modelData.formDelegate === 'int' )      return 'FormIntDelegate.qml'
                            else if( modelData.formDelegate === 'real' )     return 'FormRealDelegate.qml'
                            else if( modelData.formDelegate === 'combo' )    return 'FormComboDelegate2.qml'
                            else if( modelData.formDelegate === 'date' )     return 'FormDateDelegate.qml'
                            else if( modelData.formDelegate === 'pass' )     return 'FormPassDelegate.qml'
                            else if( modelData.formDelegate === 'textarea' ) return 'FormTextAreaDelegate.qml'
                            else if( modelData.formDelegate === 'none' )     return ''
                            else if( modelData.formDelegate === undefined )  return ''
                            else                                             return 'FormTextDelegate.qml'
                    onLoaded: {
                        item.labelText = modelData.caption
                        //                        if( modelData.formDelegate === 'combo' ) item.datasource = modelData.datasource
                        //                        if( modelData.formDelegate === 'combo' ) item.modelService = service
                        if( modelData.formDelegate === 'combo' ) {
                            if( modelData.datasource === 'users' ) item.m = usersModel
                            else if( modelData.datasource === 'treeSite' ) item.m = treeSite
                            else if( modelData.datasource === 'site' ) item.m = siteModel
                            else if( modelData.datasource === 'elements' ) item.m = elementsModel
                            else if( modelData.datasource === 'repairs_data' ) item.m = repairs_dataModel
                            else if( modelData.datasource === 'meter_data' ) item.m = meter_dataModel
                            else if( modelData.datasource === 'cn_area' ) item.m = cn_areaModel
                            else if( modelData.datasource === 'catenaries' ) item.m = catenariesModel
                            else if( modelData.datasource === 'el_class' ) item.m = el_classModel
                            else if( modelData.datasource === 'el_type' ) item.m = el_typeModel
                            else if( modelData.datasource === 'repair_type' ) item.m = repair_typeModel
                            else if( modelData.datasource === 'si' ) item.m = siModel
                            else if( modelData.datasource === 'omkum_p1' ) item.m = omkum_p1Model
                            else if( modelData.datasource === 'omkum_p2' ) item.m = omkum_p2Model
                            else if( modelData.datasource === 'roles' ) item.m = rolesModel

                        }

                        if( modelData.formDelegate === 'combo' ) item.textRole = modelData.textRole
                        if( modelData.formDelegate === 'combo' ) item.indexRole = modelData.relationIdField
                        item.value = Qt.binding( function(){
                            //                            console.log( '!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!', modelData.name, root.rowData[ modelData.name ] )
                            if( modelData.formDelegate !== 'combo'){
                                if( modelData === undefined ) return ''
                                if( root.rowData === null ) return ''
                                if( root.rowData === undefined ) return ''
                                if( modelData.name === undefined ) return ''
                                return root.rowData[ modelData.name ]
                            }else{
                                if( modelData === undefined ) return -1
                                if( root.rowData === null ) return -1
                                if( root.rowData === undefined ) return -1
                                if( modelData.localIdField === undefined ) return -1
                                return root.rowData[ modelData.localIdField ]
                            }

                        } )
                    }
                }
            }
        }
    }



    Item {
        id: root
        anchors.fill: parent
        state: 'unauthorized'
//        onStateChanged: console.log( 'state changed', state )
        states: [
            State {
                name: "unauthorized"
                when: !service.authenticated || !service.isConnected
                PropertyChanges {
                    target: loginForm
                    visible: true
                }
                PropertyChanges {
                    target: mainWindow
                    title: '%1 %2'.arg( programName ).arg( programVersion )
                }
            },
            State {
                name: "authorized"
                when: service.authenticated
                PropertyChanges {
                    target: mainUi
                    visible: true
                    enabled: true
                }
                PropertyChanges {
                    target: mainWindow
                    title: '%1 - %2 %3'.arg( service.userInfo[ 'userName' ] ).arg( programName ).arg( programVersion )
                }
            }
        ]

        Login {
            id: loginForm
        }
        MainUi {
            id: mainUi
        }
    }



    ListModel {
        id: noticeModel
    }
    Connections {
        target: service
        onNewNotice: {
            console.log( message )
            noticeModel.append( message )
        }
    }
    ColumnLayout{
        anchors.right: parent.right
        anchors.rightMargin: 5
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 25
        Repeater {
            model: noticeModel
            HToolTip { }
        }
    }

    T2CManager {
        id: manager
//        host: '192.168.1.78'
        host: '10.0.0.34'
        port: 47001
        Component.onCompleted: start()
    }


    /**
    Window {
        width: 500
        height: 500
        visible: true
        ColumnLayout {
            Rectangle {
                id: csSwitch
                width: 100
                height: 50
                color: 'black'
                property var    colors: [ 'black', 'red', 'green', 'purple' ]
                property int    idf:    167188
                property string type:   'I08'

                function set(v) {
                    manager.setValue(csSwitch.idf, csSwitch.type, v)
                }

                function setcolor(c){
                    var match  = /DAT 0 (\d+) \S+ (\S+) (\S+) (\d+\.\d+)/.exec(c)
                    if( match != null )
                        color = colors[match[2]]
                }

                Component.onCompleted: {
                    manager.subscribe(idf)
                    manager.gotMessage.connect(setcolor)
                }
            }
            Button {
                text: 'undef'
                onClicked: csSwitch.set(0)
            }
            Button {
                text: 'off'
                onClicked: csSwitch.set(1)
            }
            Button {
                text: 'on'
                onClicked: csSwitch.set(2)
            }
            Button {
                text: 'invalid'
                onClicked: csSwitch.set(3)
            }
        }
    }
    /**/
}
