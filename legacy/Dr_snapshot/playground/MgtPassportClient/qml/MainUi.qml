import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Window 2.2
//import QtQml.StateMachine 1.0
import QtGraphicalEffects 1.0
import QtQuick.Controls.Styles 1.4
import Test 1.0

Item {
    id: root
    anchors.fill: parent

    visible: false
    Item { // background
        anchors.fill: parent
        Image {
            id: background
            anchors.fill: parent
            fillMode: Image.Tile
            source: '250.gif'
        }
        HueSaturation {
            anchors.fill: parent
            source: background
            lightness: +0.3
        }
    }


    Action { // logout action
        id: logoutAction
        onTriggered: service.logout()
    }
//    Action {
//        id: viewAction
//        onTriggered: {
//            readView.visible = true
//            writeView.visible = false
//            userView.visible = false
//            readModeButton.checked = true
//            writeModeButton.checked = false

//            userModeButton.checked = false
//        }
//    }
//    Action {
//        id: editAction
//        onTriggered: {
//            readView.visible = false
//            writeView.visible = true
//            userView.visible = false
//            readModeButton.checked = false
//            writeModeButton.checked = true
//            userModeButton.checked = false
//        }
//    }

//    Action {
//        id: userAction
//        onTriggered: {
//            readView.visible = false
//            writeView.visible = false
//            userView.visible = true
//            readModeButton.checked = false
//            writeModeButton.checked = false
//            userModeButton.checked = true
//        }
//    }

    Action {
        id: viewAction
        onTriggered: {
            mainViewArea.state = 'frontPage'
        }
    }
    Action {
        id: dataAction
        onTriggered: {
            mainViewArea.state = 'dataPage'
        }
    }
    Action {
        id: editAction
        onTriggered: {
            mainViewArea.state = 'dictPage'
        }
    }

    Action {
        id: userAction
        onTriggered: {
            mainViewArea.state = 'serverPage'
        }
    }


    ColumnLayout {


        anchors.fill: parent
        ToolBar { // toolbar
            Layout.fillWidth: true
            Layout.preferredHeight: 37
            RowLayout {
                property int maintabfontsize: uiStyle.fontSizeMed
                spacing:  5
                BigButton {
                    text: 'Выход'
                    onClicked: logoutAction.trigger( this )
                    fontSize: parent.maintabfontsize
                    margin: 10
                }
                BigButton {
                    id: readModeButton
                    checkable: true
                    text: 'Паспорта участков КС'
                    onClicked: { checked = false;viewAction.trigger( this )}
                    enabled: service.userInfo[ 'canRead' ] === undefined ? false : service.userInfo[ 'canRead' ]
                    checked: false//readView.visible
                    fontSize: parent.maintabfontsize
                    margin: 10
                }
                BigButton {
                    id: dataModeButton
                    checkable: true
                    text: 'Ведение паспортов'
                    onClicked: { checked = false;dataAction.trigger( this )}
                    enabled: service.userInfo[ 'canWrite' ] === undefined ? false : service.userInfo[ 'canWrite' ]
                    checked: false//writeView.visible
                    fontSize: parent.maintabfontsize
                    margin: 10
                }
                BigButton {
                    id: writeModeButton
                    checkable: true
                    text: 'Справочники'
                    onClicked: { checked = false;editAction.trigger( this )}
                    enabled: service.userInfo[ 'canWrite' ] === undefined ? false : service.userInfo[ 'canWrite' ]
                    checked: false//writeView.visible
                    fontSize: parent.maintabfontsize
                    margin: 10
                }
                BigButton {
                    id: userModeButton
                    checkable: true
                    text: 'Администрирование'
                    onClicked: { checked = false;userAction.trigger( this )}
                    enabled: service.userInfo[ 'canManageUsers' ] === undefined ? false : service.userInfo[ 'canManageUsers' ]
                    checked: false//userView.visible
                    fontSize: parent.maintabfontsize
                    margin: 10
                }
            }
        }
        Item{ // main view area
            id: mainViewArea
            Layout.fillWidth: true
            Layout.fillHeight: true
            state: 'frontPage'
            states: [
                State{
                    name: 'frontPage'
                    PropertyChanges {
                        target: readView
                        visible: true
                    }
                    PropertyChanges {
                        target: readModeButton
                        checked: true
                    }
                },
                State{
                    name: 'dataPage'
                    PropertyChanges {
                        target: dataEditView
                        visible: true
                    }
                    PropertyChanges {
                        target: dataModeButton
                        checked: true
                    }
                },
                State{
                    name: 'dictPage'
                    PropertyChanges {
                        target: writeView
                        visible: true
                    }
                    PropertyChanges {
                        target: writeModeButton
                        checked: true
                    }
                },
                State{
                    name: 'serverPage'
                    PropertyChanges {
                        target: userView
                        visible: true
                    }
                    PropertyChanges {
                        target: userModeButton
                        checked: true
                    }
                }

            ]

            Rectangle {
                id: readView
                anchors.fill: parent
                visible: false
                FrontPage {
                    anchors.fill: parent
                }
                color: '#dcdcdc'
            }


            TabView {
                id: dataEditView
                anchors.fill: parent
                visible: false
                style: TabViewStyle{

                    frameOverlap: 1
                    tabOverlap: 1
                    tab: Rectangle {
                        //                        color: styleData.selected ? '#555' : '#999'
                        //                        border.color: styleData.selected ? '#dcdcdc' : '#b1b1b1'
                        border.color: '#b1b1b1'
                        implicitWidth: Math.max(text2.width + 30, 80)
                        implicitHeight: 30
                        radius: 2
                        gradient: Gradient{
                            GradientStop { position: 0.0; color: styleData.selected ? '#ededed' : '#e3e3e3' }
                            GradientStop { position: 1.0; color: styleData.selected ? '#dcdcdc' : '#d3d3d3' }
                        }

                        Text {
                            id: text2
                            anchors.centerIn: parent
                            anchors.leftMargin: 10
                            font.pixelSize: uiStyle.fontSizeNormal * 0.9//16
                            renderType: Text.NativeRendering
                            text: styleData.title
                            //                            color: styleData.selected ? "white" : "black"
                        }
                        Rectangle {
                            anchors.bottom: parent.bottom
                            anchors.left: parent.left
                            anchors.right: parent.right
                            height: 1
                            color: styleData.selected ? '#dcdcdc' : 'transparent'
                        }
                    }
                    frame: Rectangle { color: '#dcdcdc' }
                }
                //                Editor {}

                Tab{
                    title: '● Участки КС'

                    Table {
                        Calculator{
                            id: calc
//                            modality: Window.ApplicationModal
                        }
                        QtObject{
                            id: calcHelper
                            property string ksname:'undef'
                            property real siteLength: 10
                            property real k1pl: 0
                            property real k1ps: 0
                            property bool k1pc: false
                            property real k1ml: 0
                            property real k1ms: 0
                            property bool k1mc: false
                            property bool istram: false


                            function onTriggered(){
                                calc.ksname = ksname

                                var it0 = calcModelProxy.item(0)
                                var it1 = calcModelProxy.item(1)
                                if( it0 !== null && it0 !== undefined ) {
//                                    console.log('<<<<<<<<<<>>>>>>>>>>', it0.site_lenght, it0 )
                                    calc.siteLength = it0.site_lenght === undefined ? 0 : it0.site_lenght * 1000
                                    calc.k1pl = it0.length === undefined ? 0 : it0.length
                                    calc.k1ps = it0.section === undefined ? 0 : it0.section
                                    calc.k1pc = it0.material === 'Медь' || it0.material === 'медь'
                                    calc.k1ml = it1.length === undefined ? 0 : it1.length
                                    calc.k1ms = it1.section === undefined ? 0 : it1.section
                                    calc.k1mc = it1.material === 'Медь' || it1.material === 'медь'
                                    calc.istram = istram
                                }
                                calc.show()
                            }
                        }

                        userFunctions: [
                            {
                                text:'Расчёт',
                                calcObj: calcHelper,
                                pano: false
//                                onTriggered: function(){
//                                    console.log( 'lirbvcl sirbtvlwrebvlisrbtvelriwb', t )
//                                }
                            },
                            {
                                text: 'Панорама',
                                obj: www,
                                pano: true

                            }


                        ]
                        WWW {
                            id: www
//                            position: '37.555331,55.747183'
                        }
                        anchors.fill: parent
//                        datasource: 'site'
                        __dataModel: siteModel

                        editDialogLeyout: ColumnLayout {
                            id: sitesDialogeLayout
                            property string title: 'title'
                            property string note: 'note'
                            property var fieldsModel
                            property var rowData: null
                            function onAcceptFunction() {
                                console.log( 'accept' )
                                var returnData
                                if( rowData !== null ) {
                                    rowData.site = siteDialogSiteCombo.value
                                    rowData.year_constr = siteDialogeLayoutTypeText.value
                                    rowData.is_tram = istramdialogthingie.value
                                    rowData.area_id = siteDialogAreaCombo.value
                                    rowData.catenaries_id = siteDialogcatenaryCombo.value
                                    rowData.street_code = siteDialogstreetCombo.value
                                    rowData.el_site_id = siteElSiteDelegateCombo.value
                                    rowData.site_number = siteNumberDelegate.value
//                                    rowData.codename = '%1/%2/%3/%4'.arg( siteDialogcatenaryCombo.value ).arg( siteDialogstreetCombo.value ).arg( siteDialogAreaCombo.value ).arg( siteNumberDelegate.value )
                                    returnData = rowData
                                }else{
                                    returnData = {
                                        site: siteDialogSiteCombo.value,
                                        year_constr: siteDialogeLayoutTypeText.value,
                                        is_tram: istramdialogthingie.value,
                                        area_id: siteDialogAreaCombo.value,
                                        catenaries_id: siteDialogcatenaryCombo.value,
                                        street_code: siteDialogstreetCombo.value,
                                        el_site_id: siteElSiteDelegateCombo.value,
                                        site_number: siteNumberDelegate.value
//                                        codename: '%1/%2/%3/%4'.arg( siteDialogcatenaryCombo.value ).arg( siteDialogstreetCombo.value ).arg( siteDialogAreaCombo.value ).arg( siteNumberDelegate.value )
                                    }
                                }
                                clearData()
                                return returnData
                            }
                            function onRejectFunction() {
                                console.log( 'reject' )
                                clearData()
                            }

                            function init(title, note, fields, data){
                                clearData()
                                sitesDialogeLayout.title = title
                                sitesDialogeLayout.note = note
                                sitesDialogeLayout.rowData = data

                                siteNumberDelegate.value = data.site_number === undefined ? 0 : data.site_number

                                siteElSiteDelegateCombo.value = data.el_site_id === undefined ? -1 : data.el_site_id
                                siteDialogAreaCombo.value = data.area_id === undefined ? -1 : data.area_id
                                siteDialogSiteCombo.value = data.site === undefined ? '' : data.site
                                siteDialogeLayoutTypeText.value = data.year_constr === undefined ? 1901 : data.year_constr
                                istramdialogthingie.value = data.is_tram === undefined ? false : data.is_tram
                                siteDialogstreetCombo.value = data.street_code === undefined ? -1 : data.street_code
                                siteDialogcatenaryCombo.value = data.catenaries_id === undefined ? -1 : data.catenaries_id

                                rowDataChanged()
                                sitesDialogeLayout.fieldsModel = fields
                            }
                            function clearData() {
                                rowData = undefined
                                rowDataChanged()
                                //                re.model = null
                            }
                            FormIntDelegate {
                                id: siteNumberDelegate
                                labelText: 'Номер участка'
                                v.minimumValue: 0
                                v.maximumValue: 99999999
                                value: if( siteDialogeLayoutTypeText.rowData === null ) return ''
                                       else if( siteDialogeLayoutTypeText.rowData === undefined ) return ''
                                       else return siteDialogeLayoutTypeText.rowData.site_number
                            }

                            FormComboDelegate2 {
                                id: siteDialogAreaCombo
                                labelText: 'Район КС'
                                m: cn_areaModel
//                                modelService: service
//                                datasource: 'cn_area'
                                textRole: 'name'
                                indexRole: 'id'
                            }
                            FormComboDelegate2 {
                                id: siteElSiteDelegateCombo
                                labelText: 'Электрический участок'
                                m: el_siteModel
                                textRole: 'name'
                                indexRole: 'id'
                            }
                            FormTextDelegate {
                                id: siteDialogSiteCombo
                                labelText: 'Участок КС'
                                value: if( siteDialogeLayoutTypeText.rowData === null ) return ''
                                       else if( siteDialogeLayoutTypeText.rowData === undefined ) return ''
                                       else return siteDialogeLayoutTypeText.rowData.site
                            }
                            FormIntDelegate {
                                id: siteDialogeLayoutTypeText
                                labelText: 'Год постройки'
                                v.minimumValue: 1901
                                v.maximumValue: 2999
                                value: if( siteDialogeLayoutTypeText.rowData === null ) return ''
                                       else if( siteDialogeLayoutTypeText.rowData === undefined ) return ''
                                       else return siteDialogeLayoutTypeText.rowData.year_constr
                            }
                            FormTransportTypeDelegate {
                                id: istramdialogthingie
                                labelText: 'Трамвай'
                                value: if( siteDialogeLayoutTypeText.rowData === null ) return ''
                                       else if( siteDialogeLayoutTypeText.rowData === undefined ) return ''
                                       else return siteDialogeLayoutTypeText.rowData.is_tram
                            }
                            FormComboDelegate2 {
                                id: siteDialogstreetCombo
                                labelText: 'Название улицы'
//                                modelService: service
//                                datasource: 'omkum_p1'
                                m: omkum_p1Model
                                textRole: 'UM_NAMEF'
                                indexRole: 'UM_CODE'

                            }
                            FormComboDelegate2 {
                                id: siteDialogcatenaryCombo
                                labelText: 'Наименование подвески'
                                m: catenariesModel
//                                modelService: service
//                                datasource: 'catenaries'
                                textRole: 'name'
                                indexRole: 'id'
                            }

                        }

                    }
                }
                Tab{
                    title: '● Элементы КС'
                    Table {
                        anchors.fill: parent
//                        datasource: 'elements'
                        __dataModel: elementsModel




                        editDialogLeyout: ColumnLayout {
                            id: elementsDialogeLayout
                            property string title: 'title'
                            property string note: 'note'
                            property var fieldsModel
                            property var rowData: null
                            function onAcceptFunction() {
                                console.log( 'accept' )
                                var returnData
                                if( rowData !== null ) {
                                    rowData.name =              elementsDelegateName.value
                                    rowData.mount_date =        elementsDelegateDateBegin.value
                                    rowData.dismount_date =     elementsDelegateDateEnd.value
                                    rowData.site1_id =          elementsDialogSiteCombo1.value === -1 ? 404 : elementsDialogSiteCombo1.value
                                    rowData.site2_id =          elementsDialogSiteCombo2.value === -1 ? 404 : elementsDialogSiteCombo2.value
                                    rowData.class_id =          elementsDelegateClass.value
                                    rowData.type_id =           elementsDelegateType.value

                                    rowData.accounting_num =           elementsDelegateaccounting_num.value
                                    rowData.length =           elementsDelegatelength.value
                                    rowData.section =           elementsDelegatesection.value
                                    rowData.material =           elementsDelegatematerial.value

                                    returnData = rowData
                                }else{
                                    returnData = {
                                        name:              elementsDelegateName.value,
                                        mount_date:        elementsDelegateDateBegin.value   ,
                                        dismount_date:     elementsDelegateDateEnd.value     ,
                                        site1_id:          elementsDialogSiteCombo1.value === -1 ? 404 : elementsDialogSiteCombo1.value    ,
                                        site2_id:          elementsDialogSiteCombo2.value === -1 ? 404 : elementsDialogSiteCombo2.value   ,
                                        class_id:          elementsDelegateClass.value       ,
                                        type_id:           elementsDelegateType.value,


                               accounting_num :           elementsDelegateaccounting_num.value,
                               length :           elementsDelegatelength.value,
                               section :           elementsDelegatesection.value,
                               material :           elementsDelegatematerial.value
                                    }
                                }
                                clearData()
                                return returnData
                            }
                            function onRejectFunction() {
                                console.log( 'reject' )
                                clearData()
                            }

                            function init(title, note, fields, data){
                                clearData()
                                elementsDialogeLayout.title = title
                                elementsDialogeLayout.note = note
                                elementsDialogeLayout.rowData = data


                                elementsDialogAreaCombo.value = data.area_id === undefined ? -1 : data.area_id
                                elementsDialogSiteCombo1.value = data.site1_id === undefined ? -1 : data.site1_id
                                elementsDialogSiteCombo2.value = data.site2_id === undefined ? -1 : data.site2_id
                                elementsDelegateClass.value = data.class_id === undefined ? -1 : data.class_id
                                elementsDelegateType.value = data.type_id === undefined ? -1 : data.type_id

                                rowDataChanged()
                                elementsDialogeLayout.fieldsModel = fields
                            }
                            function clearData() {
                                rowData = undefined
                                rowDataChanged()
                                //                re.model = null
                            }
                            FormComboDelegate2 {
                                id: elementsDialogAreaCombo
                                labelText: 'Район КС'
//                                modelService: service
//                                datasource: 'cn_area'
                                m: cn_areaModel
                                textRole: 'name'
                                indexRole: 'id'
                            }
                            FormComboDelegate2 {
                                id: elementsDialogSiteCombo1
                                labelText: 'Участок КС троллейбус'
//                                modelService: service
//                                datasource: 'site'
                                m: siteModel
                                textRole: 'site'
                                indexRole: 'id'
                                Component.onCompleted: {
                                    proxymodel.addStaticFilter( elementsitefiltebyarea )
                                    proxymodel.addStaticFilter( elementsitefiltebytrolley )
                                }
                                FilterObject {
                                    id: elementsitefiltebyarea
                                    criteria: elementsDialogAreaCombo.value
                                    role: 'area_id'
                                    filter: f_equal
                                }

                                FilterObject {
                                    id: elementsitefiltebytrolley
                                    criteria: false
                                    role: 'is_tram'
                                    filter: f_equal
                                }
                            }
                            FormComboDelegate2 {
                                id: elementsDialogSiteCombo2
                                labelText: 'Участок КС трамвай'
//                                modelService: service
//                                datasource: 'site'
                                m: siteModel
                                textRole: 'site'
                                indexRole: 'id'
//                                v.enabled: elementsDialogSiteCombo1.value !== -1
                                Component.onCompleted: {
                                    proxymodel.addStaticFilter( elementsitefiltebyarea )
                                    proxymodel.addStaticFilter( elementsitefiltebytram )
                                }
                                FilterObject {
                                    id: elementsitefiltebytram
                                    criteria: true
                                    role: 'is_tram'
                                    filter: f_equal
                                }

                            }

                            FormTextDelegate {
                                id: elementsDelegateName
                                labelText: 'Название Элемента'
                                value: if( elementsDialogeLayout.rowData === null ) return ''
                                       else if( elementsDialogeLayout.rowData === undefined ) return ''
                                       else return elementsDialogeLayout.rowData.name
                            }
                            FormDateDelegate {
                                id: elementsDelegateDateBegin
                                labelText: 'Дата установки'
                                value: if( elementsDialogeLayout.rowData === null ) return ''
                                       else if( elementsDialogeLayout.rowData === undefined ) return ''
                                       else return elementsDialogeLayout.rowData.mount_date
                            }
                            FormDateDelegate {
                                id: elementsDelegateDateEnd
                                labelText: 'Дата демонтажа'
                                value: if( elementsDialogeLayout.rowData === null ) return ''
                                       else if( elementsDialogeLayout.rowData === undefined ) return ''
                                       else return elementsDialogeLayout.rowData.dismount_date
                            }


                            FormComboDelegate2 {
                                id: elementsDelegateClass
                                labelText: 'Класс'
//                                modelService: service
//                                datasource: 'el_class'
                                m: el_classModel
                                textRole: 'name'
                                indexRole: 'id'
                                Component.onCompleted: proxymodel.addStaticFilter( elementsitefiltebywired )
                                FilterObject {
                                    id: elementsitefiltebywired
                                    criteria: false
                                    role: 'is_meter'
                                    filter: f_equal
                                }
                            }
                            FormComboDelegate2 {
                                id: elementsDelegateType
                                labelText: 'Тип'
//                                modelService: service
//                                datasource: 'el_type'
                                m: el_typeModel
                                textRole: 'name'
                                indexRole: 'id'
                                Component.onCompleted: proxymodel.addStaticFilter( elementsitefiltebyClass )
                                FilterObject {
                                    id: elementsitefiltebyClass
                                    criteria: elementsDelegateClass.value
                                    role: 'parent_id'
                                    filter: f_equal
                                }
                            }
                            FormIntDelegate {
                                id: elementsDelegateaccounting_num
                                labelText: 'Бух. №'
                                value: if( elementsDialogeLayout.rowData === null ) return ''
                                       else if( elementsDialogeLayout.rowData === undefined ) return ''
                                       else return elementsDialogeLayout.rowData.accounting_num
                            }
                            FormIntDelegate {
                                id: elementsDelegatelength
                                labelText: 'Длина, м'
                                value: if( elementsDialogeLayout.rowData === null ) return ''
                                       else if( elementsDialogeLayout.rowData === undefined ) return ''
                                       else return elementsDialogeLayout.rowData.length
                            }
                            FormIntDelegate {
                                id: elementsDelegatesection
                                labelText: 'Сечение, мм'
                                value: if( elementsDialogeLayout.rowData === null ) return ''
                                       else if( elementsDialogeLayout.rowData === undefined ) return ''
                                       else return elementsDialogeLayout.rowData.section
                            }
                            FormTextDelegate {
                                id: elementsDelegatematerial
                                labelText: 'Материал'
                                value: if( elementsDialogeLayout.rowData === null ) return ''
                                       else if( elementsDialogeLayout.rowData === undefined ) return ''
                                       else return elementsDialogeLayout.rowData.material
                            }

                        }

                    }
                }
                Tab{
                    title: '● Ремонты'
                    Table {
                        anchors.fill: parent
//                        datasource: 'repairs_data'
                        __dataModel: repairs_dataModel

                        editDialogLeyout: ColumnLayout {
                            id: repairsDialogeLayout
                            property string title: 'title'
                            property string note: 'note'
                            property var fieldsModel
                            property var rowData: null
                            function onAcceptFunction() {
                                console.log( 'accept' )
                                var returnData
                                if( rowData !== null ) {
                                    rowData.site_id = repairsDialogSiteCombo.value
                                    rowData.date_begin = repairsDelegateDateBegin.value
                                    rowData.date_end = repairsDelegateDateEnd.value
                                    rowData.made_by = repairsDelegateMadeBy.value
                                    rowData.comment = repairsDelegateComment.value
                                    rowData.repair_type_id = repairsDelegateRepairType.value

                                    returnData = rowData
                                }else{
                                    returnData = {
                                        site_id: repairsDialogSiteCombo.value,
                                        date_begin: repairsDelegateDateBegin.value,
                                        date_end: repairsDelegateDateEnd.value,
                                        made_by: repairsDelegateMadeBy.value,
                                        comment: repairsDelegateComment.value,
                                        repair_type_id: repairsDelegateRepairType.value
                                    }
                                }
                                clearData()
                                return returnData
                            }
                            function onRejectFunction() {
                                console.log( 'reject' )
                                clearData()
                            }

                            function init(title, note, fields, data){
                                clearData()
                                repairsDialogeLayout.title = title
                                repairsDialogeLayout.note = note
                                repairsDialogeLayout.rowData = data


                                repairsDialogAreaCombo.value = data.area_id === undefined ? -1 : data.area_id
                                repairsDialogSiteCombo.value = data.site_id === undefined ? -1 : data.site_id
                                repairsDelegateRepairType.value = data.repair_type_id === undefined ? -1 : data.repair_type_id

                                rowDataChanged()
                                repairsDialogeLayout.fieldsModel = fields
                            }
                            function clearData() {
                                rowData = undefined
                                rowDataChanged()
                                //                re.model = null
                            }
                            FormComboDelegate2 {
                                id: repairsDialogAreaCombo
                                labelText: 'Район КС'
//                                modelService: service
//                                datasource: 'cn_area'
                                m: cn_areaModel
                                textRole: 'name'
                                indexRole: 'id'
                            }
                            FormComboDelegate2 {
                                id: repairsDialogSiteCombo
                                labelText: 'Участок КС'
//                                modelService: service
//                                datasource: 'site'
                                m: siteModel
                                textRole: 'name'
                                indexRole: 'id'
                                Component.onCompleted: proxymodel.addStaticFilter( repairsitefiltebyarea )
                                FilterObject {
                                    id: repairsitefiltebyarea
                                    criteria: repairsDialogAreaCombo.value
                                    role: 'area_id'
                                    filter: f_equal
                                }
                            }
                            FormDateDelegate {
                                id: repairsDelegateDateBegin
                                labelText: 'Дата начала'
                                value: if( repairsDialogeLayout.rowData === null ) return ''
                                       else if( repairsDialogeLayout.rowData === undefined ) return ''
                                       else return repairsDialogeLayout.rowData.date_begin
                            }
                            FormDateDelegate {
                                id: repairsDelegateDateEnd
                                labelText: 'Дата окончания'
                                value: if( repairsDialogeLayout.rowData === null ) return ''
                                       else if( repairsDialogeLayout.rowData === undefined ) return ''
                                       else return repairsDialogeLayout.rowData.date_end
                            }
                            FormTextDelegate {
                                id: repairsDelegateMadeBy
                                labelText: 'Производитель'
                                value: if( repairsDialogeLayout.rowData === null ) return ''
                                       else if( repairsDialogeLayout.rowData === undefined ) return ''
                                       else return repairsDialogeLayout.rowData.made_by
                            }
                            FormTextAreaDelegate {
                                id: repairsDelegateComment
                                labelText: 'Комментарий'
                                value: if( repairsDialogeLayout.rowData === null ) return ''
                                       else if( repairsDialogeLayout.rowData === undefined ) return ''
                                       else return repairsDialogeLayout.rowData.comment

                            }
                            FormComboDelegate2 {
                                id: repairsDelegateRepairType
                                labelText: 'Название работ'
//                                modelService: service
//                                datasource: 'repair_type'
                                m: repair_typeModel
                                textRole: 'name'
                                indexRole: 'id'
                            }

                        }

                    }
                }
                Tab{
                    title: '● Метрические данные'
                    Table {
                        anchors.fill: parent
//                        datasource: 'meter_data'
                        __dataModel: meter_dataModel
                        editDialogLeyout: ColumnLayout {
                            id: meterDialogLayout
                            property string title: 'title'
                            property string note: 'note'
                            property var fieldsModel
                            property var rowData: null
                            function onAcceptFunction() {
                                console.log( 'accept' )
                                var returnData
                                if( rowData !== null ) {
                                    rowData.value = meterDelegateValue.value
                                    rowData.site_id = meterDialogSiteCombo.value
                                    rowData.class_id = meterDialogClassCombo.value
                                    returnData = rowData
                                }else{
                                    returnData = {
                                        value: meterDelegateValue.value,
                                        site_id: meterDialogSiteCombo.value,
                                        class_id: meterDialogClassCombo.value
                                    }
                                }
                                clearData()
                                return returnData
                            }
                            function onRejectFunction() {
                                console.log( 'reject' )
                                clearData()
                            }

                            function init(title, note, fields, data){
                                clearData()
                                meterDialogLayout.title = title
                                meterDialogLayout.note = note
                                meterDialogLayout.rowData = data


                                meterDialogAreaCombo.value = data.area_id === undefined ? -1 : data.area_id
                                meterDialogSiteCombo.value = data.site_id === undefined ? -1 : data.site_id
                                meterDialogClassCombo.value = data.class_id === undefined ? -1 : data.class_id
                                meterDelegateValue.value = data.value === undefined ? 0 : data.value
//                                meterDelegateValue.value = data.is_tram === undefined ? 0 : data.is_tram
//                                meterDialogSiCombo.value = data.si_id === undefined ? -1 : data.si_id

                                rowDataChanged()
                                meterDialogLayout.fieldsModel = fields
                            }
                            function clearData() {
                                rowData = undefined
                                rowDataChanged()
                            }
                            FormComboDelegate2 {
                                id: meterDialogAreaCombo
                                labelText: 'Район КС'
//                                modelService: service
//                                datasource: 'cn_area'
                                m: cn_areaModel
                                textRole: 'name'
                                indexRole: 'id'
                                Component.onCompleted: proxymodel.addStaticFilter( metersitefiltebysitecount )
                                FilterObject {
                                    id: metersitefiltebysitecount
                                    criteria: 0
                                    role: 'sites'
                                    filter: f_greater
                                }
                            }
                            FormComboDelegate2 {
                                id: meterDialogSiteCombo
                                labelText: 'Участок КС'
//                                modelService: service
//                                datasource: 'site'
                                m: siteModel
                                textRole: 'site'
                                indexRole: 'id'
                                Component.onCompleted: proxymodel.addStaticFilter( metersitefiltebyarea )
                                FilterObject {
                                    id: metersitefiltebyarea
                                    criteria: meterDialogAreaCombo.value
                                    role: 'area_id'
                                    filter: f_equal
                                }
                            }
                            FormTransportTypeDelegate {
                                enabled: false
                                id: meterDialogSitebox
                                labelText: 'Тип сети'
                                value: meterDialogSiteCombo.proxymodel.getItemData( meterDialogSiteCombo.v.currentIndex).is_tram === undefined ? false : meterDialogSiteCombo.proxymodel.getItemData( meterDialogSiteCombo.v.currentIndex).is_tram
                            }

                            FormComboDelegate2 {
                                id: meterDialogClassCombo
                                labelText: 'Класс'
//                                modelService: service
//                                datasource: 'el_class'
                                m: el_classModel
                                textRole: 'name'
                                indexRole: 'id'
                                Component.onCompleted: proxymodel.addStaticFilter( metersitefiltebywiredclass )
                                FilterObject {
                                    id: metersitefiltebywiredclass
                                    criteria: 1
                                    role: 'is_meter'
                                    filter: f_equal
                                }
                                onValueChanged: {
//                                    console.log(meterDialogClassCombo.rowdata.si )
                                    meterDialogSiCombo.value = meterDialogClassCombo.rowdata === undefined ? '' : meterDialogClassCombo.rowdata.si
                                }
                            }
                            FormRealDelegate {
                                id: meterDelegateValue
                                labelText: 'Значение'
                                v.minimumValue: 0
                                v.decimals: 3
                                value: if( meterDialogLayout.rowData === null ) return ''
                                       else if( meterDialogLayout.rowData === undefined ) return ''
                                       else return meterDialogLayout.rowData.name

                            }
                            FormLabelDelegate {
                                id: meterDialogSiCombo
                                labelText: 'Единица измерения'

                                value: meterDialogClassCombo.proxymodel.getItemData( meterDialogClassCombo.v.currentIndex ).si
                            }
                        }
                    }
                }


            }
            TabView {
                id: writeView
                anchors.fill: parent
                visible: false
                style: TabViewStyle{

                    frameOverlap: 1
                    tabOverlap: 1
                    tab: Rectangle {
                        //                        color: styleData.selected ? '#555' : '#999'
                        //                        border.color: styleData.selected ? '#dcdcdc' : '#b1b1b1'
                        border.color: '#b1b1b1'
                        implicitWidth: Math.max(text2.width + 30, 80)
                        implicitHeight: 30
                        radius: 2
                        gradient: Gradient{
                            GradientStop { position: 0.0; color: styleData.selected ? '#ededed' : '#e3e3e3' }
                            GradientStop { position: 1.0; color: styleData.selected ? '#dcdcdc' : '#d3d3d3' }
                        }

                        Text {
                            id: text2
                            anchors.centerIn: parent
                            anchors.leftMargin: 10
                            font.pixelSize: uiStyle.fontSizeNormal * 0.9//16
                            renderType: Text.NativeRendering
                            text: styleData.title
                            //                            color: styleData.selected ? "white" : "black"
                        }
                        Rectangle {
                            anchors.bottom: parent.bottom
                            anchors.left: parent.left
                            anchors.right: parent.right
                            height: 1
                            color: styleData.selected ? '#dcdcdc' : 'transparent'
                        }
                    }
                    frame: Rectangle { color: '#dcdcdc' }
                }
                //                Editor {}


                Tab{
                    title: '☰ Классификатор'
                    SplitView{
                        anchors.fill: parent
                        Rectangle {
                            width: 1050
                            color: 'transparent'
                            Table {
                                id: classes
                                anchors.fill: parent
//                                datasource: 'el_class'
                                __dataModel: el_classModel


                            }
                        }
                        Rectangle {
                            color: 'transparent'
                            Table {
                                id: types
                                anchors.fill: parent
//                                datasource: 'el_type'
                                __dataModel: el_typeModel

                                Connections {
                                    target: types.__dataModel
                                    onDataUpdated: classes.__dataModel.updateData()
                                }

                                editDialogLeyout: ColumnLayout {
                                    id: typesDialogeLayout
                                    property string title: 'title'
                                    property string note: 'note'
                                    property var fieldsModel
                                    property var rowData: null
                                    function onAcceptFunction() {
                                        console.log( 'accept' )
                                        var returnData
                                        if( rowData !== null ) {
                                            rowData.parent_id = typesDialogeLayoutClassCombo.value
                                            rowData.name = typesDialogeLayoutTypeText.value
                                            rowData.si_id = typesDialogeLayoutClassCombo.rowData.si_id
                                            returnData = rowData
                                        }else{
                                            returnData = { parent_id: typesDialogeLayoutClassCombo.value, nameL: typesDialogeLayoutTypeText.value, si_id: typesDialogeLayoutClassCombo.rowData.si_id }
                                        }
                                        clearData()
                                        return returnData
                                    }
                                    function onRejectFunction() {
                                        console.log( 'reject' )
                                        clearData()
                                    }

                                    function init(title, note, fields, data){
                                        clearData()
                                        typesDialogeLayout.title = title
                                        typesDialogeLayout.note = note
                                        typesDialogeLayout.rowData = data

                                        if( data.parent_id === null || data.parent_id === undefined) {
                                            if( classes.selectedData === null || classes.selectedData === undefined ){
                                                typesDialogeLayoutClassCombo.value = -1
                                            }else{
                                                typesDialogeLayoutClassCombo.value = classes.selectedData.parent_id
                                            }
                                        }else{
                                            typesDialogeLayoutClassCombo.value = data.parent_id
                                        }
                                        //                                        typesDialogeLayoutClassCombo.value = data.parent_id !== null ? data.parent_id !== undefined ? data.parent_id : -1 : -1

                                        rowDataChanged()
                                        typesDialogeLayout.fieldsModel = fields
                                    }
                                    function clearData() {
                                        rowData = undefined
                                        rowDataChanged()
                                        //                re.model = null
                                    }
                                    FormComboDelegate2 {
                                        id: typesDialogeLayoutClassCombo
                                        labelText: 'Принадлежность классу'
//                                        modelService: service
//                                        datasource: 'el_class'
                                        m: el_classModel
                                        textRole: 'name'
                                        indexRole: 'id'
                                        Connections {
                                            target: classes.__dataModel
                                            onDataUpdated: {
                                                console.log( 'yey new stuff' )
                                                typesDialogeLayoutClassCombo.datamodel.updateData()
                                            }
                                        }

                                    }
                                    FormTextDelegate {
                                        id: typesDialogeLayoutTypeText
                                        labelText: 'Название типа'
                                        value: if( typesDialogeLayout.rowData === null ) return ''
                                               else if( typesDialogeLayout.rowData === undefined ) return ''
                                               else return typesDialogeLayout.rowData.name

                                    }
                                }

                                Component.onCompleted: proxyMmodel.addStaticFilter( typefilterbyclass )
                                FilterObject {
                                    id: typefilterbyclass
                                    criteria: classes.selectedData === null ? null : classes.selectedData.parent_id
                                    role: 'parent_id'
                                    filter: f_equal
                                }
                            }
                        }
                    }
                }
                Tab{
                    title: '☰ Районы КС'
                    Table {
                        anchors.fill: parent
//                        datasource: 'cn_area'
                        __dataModel: cn_areaModel
                    }
                }
                Tab{
                    title: '☰ Эл участок'
                    Table {
                        anchors.fill: parent
                        __dataModel: el_siteModel
                    }
                }
                Tab{
                    title: '☰ Подвески'
                    Table {
                        anchors.fill: parent
//                        datasource: 'catenaries'
                        __dataModel: catenariesModel
                    }
                }

                Tab{
                    title: '☰ Типы ремонтов'
                    Table {
                        anchors.fill: parent
//                        datasource: 'repair_type'
                        __dataModel: repair_typeModel
                    }
                }
                Tab{
                    title: '☰ Ед. изм.'
                    Table {
                        anchors.fill: parent
//                        datasource: 'si'
                        __dataModel: siModel
                    }
                }
                Tab{
                    title: '☰ ОМКУМ 1'
                    Table {
                        anchors.fill: parent
//                        datasource: 'omkum_p1'
                        __dataModel: omkum_p1Model
                    }
                }
                Tab{
                    title: '☰ ОМКУМ 2'
                    Table {
                        anchors.fill: parent
//                        datasource: 'omkum_p2'
                        __dataModel: omkum_p2Model
                    }
                }

            }
            TabView {
                id: userView
                anchors.fill: parent
                visible: false
                style: TabViewStyle{
                    frameOverlap: 1
                    tabOverlap: 1
                    tab: Rectangle {
                        //                        color: styleData.selected ? '#555' : '#999'
                        //                        border.color: styleData.selected ? '#dcdcdc' : '#b1b1b1'
                        border.color: '#b1b1b1'
                        implicitWidth: Math.max(text1.width + 50, 80)
                        implicitHeight: 30
                        radius: 2
                        gradient: Gradient{
                            GradientStop { position: 0.0; color: styleData.selected ? '#ededed' : '#e3e3e3' }
                            GradientStop { position: 1.0; color: styleData.selected ? '#dcdcdc' : '#d3d3d3' }
                        }

                        Text {
                            id: text1
                            anchors.centerIn: parent
                            anchors.leftMargin: 10
                            font.pixelSize: uiStyle.fontSizeNormal//16
                            renderType: Text.NativeRendering
                            text: styleData.title
                            //                            color: styleData.selected ? "white" : "black"
                        }
                        Rectangle {
                            anchors.bottom: parent.bottom
                            anchors.left: parent.left
                            anchors.right: parent.right
                            height: 1
                            color: styleData.selected ? '#dcdcdc' : 'transparent'
                        }
                    }
                    frame: Rectangle { color: '#dcdcdc' }
                }
                //                UserEditor {}
                ServerControl {}


            }

        }
    }


    //    Table2 {}
}

