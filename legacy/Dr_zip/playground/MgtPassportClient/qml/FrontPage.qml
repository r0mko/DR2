import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.2
import Test 1.0
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

RowLayout {
    id: mainscreen

    property int currentSelectedSiteId: -1

    property var idToIdf: {
                430: 465188,
                431: 465195,
                432: 465202,
                433: 465209,
                434: 465271,
                435: 465278
    }
    onCurrentSelectedSiteIdChanged: {
        var sd = currentSelectedSiteId;
        if(( sd >= 430) && (sd <= 435) ) {
            map.da.centerSceneOn(8229.333333333332, 4410.666666666666)
            var idf = idToIdf[sd]
            map.myways.setVisible(idf)
            for( var key in map.ways )
                map.ways[key].forEach( function(e){
                    var md = e.blob$metadata
                    md.active = key === idf
                    e.blob$metadata = md
                })
        }else{
            map.myways.setVisible(-1)
            for( var key in map.ways )
                map.ways[key].forEach( function(e){
                    var md = e.blob$metadata
                    md.active = false
                    e.blob$metadata = md
                })
        }
    }

    anchors.fill: parent
    state: 'siteSelection'
    spacing: 0
    Keys.onPressed: {
        if (event.key == Qt.Key_1 ) {
            state = 'siteSelection'
            event.accepted = true;
        }
        else if (event.key == Qt.Key_2 ) {
            state = 'pasportView'
            event.accepted = true;
        }
        else if (event.key == Qt.Key_3 ) {
            state = 'mapView'
            event.accepted = true;
        }
    }
    states: [
        State {
            name: 'pasportView'
            PropertyChanges {
                target: ks
                implicitWidth: 0
            }
        },
        State {
            name: 'siteSelection'
            PropertyChanges {
                target: ks
                implicitWidth: mainscreen.width - 30
            }
        }
    ]


    Rectangle {
        clip: true
        id: ks
        Layout.fillHeight: true

        height: parent.parent.height

        implicitWidth: 0
        color: 'transparent'
        Behavior on implicitWidth {
            NumberAnimation { duration: 300; easing.type: Easing.OutExpo }
        }
        Table {
            id: sites
            anchors.fill: parent
//                datasource: 'treeSite'\

            __dataModel: treeSite
            onSelectedDataChanged: {
                if( selectedData !== null ) {
                            console.log( 'selectedDataChanged', selectedData, selectedData.id, selectedData.data, treeSite.readBlob(selectedData.data), selectedData.site_number )
                    mainscreen.currentSelectedSiteId = selectedData.id
                }
            }
        }
    }
    Rectangle {
        Layout.fillHeight: true
        height: parent.parent.height
        width: 30
        implicitWidth: 30
        border.color: '#ccc'
        border.width: 4
        color: ma.containsMouse ? '#b0b0b0' : '#a0a0a0'
        Behavior on color {
            ColorAnimation {
                duration: 100
            }
        }
        Text {
            anchors.centerIn: parent
            text: 'Участки КС%1'.arg( sites === null ? '' : sites === undefined ? '' : sites.selectedText === '' ? '' : sites.selectedData === null ? '' : sites.selectedData === undefined ? '' : ( ' - ' + sites.selectedData.site ) )


            rotation: -90
            font.pixelSize: uiStyle.fontSizeMed // 18
        }
        MouseArea {
            id: ma
            hoverEnabled: true
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: if ( mainscreen.state === 'siteSelection' ) mainscreen.state = 'pasportView'; else mainscreen.state = 'siteSelection'

        }
    }
    Rectangle {
        height: parent.parent.height
        //            color: '#77f'
        color: 'transparent'
        clip: true
        Layout.fillWidth: true
        Layout.fillHeight: true
        SplitView {
            anchors.fill: parent
            Rectangle {
                id: r2
                clip: true
                color: 'transparent'
                Layout.minimumWidth: 200
                width: 800
                Behavior on width { NumberAnimation { duration: 300; easing.type: Easing.OutExpo }}
                ColumnLayout{
                    anchors.fill: parent
                    spacing: 0
                    Rectangle {
                        Layout.fillWidth: true
                        height: 36
                        color: 'transparent'
                        RowLayout {
                            anchors.fill: parent
                            Item{ width:5 }
                            BigButton {
                                text: 'Развернуть всё'
                                onClicked: {
                                    tree.expandAll()
                                }
                                fontSize: uiStyle.fontSizeNormal // 14
                                margin: 5
                            }
                            BigButton {
                                text: 'Свернуть всё'
                                onClicked: {
                                    tree.collapseAll()
                                }
                                fontSize: uiStyle.fontSizeNormal //14
                                margin: 5
                            }
                            BigButton {
                                text: 'Печать...'
                                onClicked: {
                                    //                                tree.collapseAll()
                                }
                                fontSize: uiStyle.fontSizeNormal //14
                                margin: 5
                            }
                            CheckBox {
                                text:'Демонтированные'
                            }
                            Item{ Layout.fillWidth: true }

                        }
                        Switch {
                            //                                    id: stateSwitch
                            anchors.top: parent.top
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            checked:  true
                            onCheckedChanged: if( checked ) r2.width = 800; else r2.width = 450
                        }
                    }
                    Tree {
                        id: tree
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        currentSelectedSiteId: sites.selectedData === null ? -1 : sites.selectedData.id//Number( sites.currentSelection )
//                        onCurrentSelectedSiteIdChanged: console.log(currentSelectedSiteId)
                    }

//                    PartModel {
//                        id: treeModel
//                        modelService: service
//                        dataSource: 'tree'
//                        currentSelectedSiteId: sites.selectedData === null ? -1 : sites.selectedData.id
//                    }
//                    TreeProxy {
//                        id: treeProxyModel
//                        sourceModel: treeModel
//                    }
//                    Tree2 {
//                        id: tree
//                        Layout.fillWidth: true
//                        Layout.fillHeight: true
//                        model: treeProxyModel
//                        TableViewColumn{
//                            role: 'name'
//                            title: 'Элементы КС'
//                            width: 400
//                        }
//                    }
                }
            }
            Rectangle {
                id: r3
                color: 'transparent'
                width: root.width * 0.3
                Layout.minimumWidth: 10
                clip: true
                ColumnLayout{
                    anchors.fill: parent
                    spacing: 0
                    Rectangle {
                        Layout.fillWidth: true
                        height: 36
                        color: 'transparent'
                        RowLayout {
                            anchors.fill: parent
                            Item{ width:5 }
                            Button {
                                text: '+'
                                onClicked: {

                                }
                            }
                            Button {
                                text: '-'
                                onClicked: {

                                }
                            }
                            Button {
                                text: 'Вывести участок в ремонт'
                                visible: currentSelectedSiteId >= 420 && currentSelectedSiteId <= 425
                                onClicked: {
                                    var sd = mainscreen.currentSelectedSiteId;
                                    if(( sd >= 420) && (sd <= 425) ) {
                                        var idf = idToIdf[sd]
                                        map.ways[idf].forEach( function(e){
                                            var md = e.blob$metadata
                                            md.repairs = true
                                            e.blob$metadata = md
                                        })

//                                        for( var key in map.ways )
//                                            map.ways[key].forEach( function(e){
//                                                var md = e.blob$metadata
//                                                console.log( 'keyidf', key, idf, key == idf )
//                                                md.repairs = key == idf
//                                                e.blob$metadata = md
//                                            })
                                    }
                                }
                            }
                            Button {
                                text: 'Закончить ремонт'
                                visible: currentSelectedSiteId >= 420 && currentSelectedSiteId <= 425
                                onClicked: {
                                    var sd = mainscreen.currentSelectedSiteId;
                                    if(( sd >= 420) && (sd <= 425) ) {
                                        var idf = idToIdf[sd]
                                        map.ways[idf].forEach( function(e){
                                            var md = e.blob$metadata
                                            md.repairs = false
                                            e.blob$metadata = md
                                        })

//                                        for( var key in map.ways )
//                                            map.ways[key].forEach( function(e){
//                                                var md = e.blob$metadata
//                                                console.log( 'keyidf', key, idf, key == idf )
//                                                md.repairs = key == idf
//                                                e.blob$metadata = md
//                                            })
                                    }
                                }
                            }
                            Button {
                                id: posterSetter
                                property var p
                                text: 'Установить плакат'
                                visible: currentSelectedSiteId >= 420 && currentSelectedSiteId <= 425
                                onClicked: {
                                    if( p == null ) {
                                        p = poster.createObject( this )
                                        p.objectName = 'poster'
                                        p.text = 'Poster REPAIRS'


//                                        map.marker.x = map.da.scrollArea.contentX * map.da.scale +map.da.width * 0.5
//                                        map.marker.y = map.da.scrollArea.contentY * map.da.scale +map.da.height * 0.5


                                        p.posX = Qt.binding( function(){return map.marker.x})
                                        p.posY = Qt.binding( function(){return map.marker.y})
                                        map.marker.text = Qt.binding(function() {return p.text})
                                        map.engine.registerObject( p, true )
                                        map.marker.visible = true



                                    }
                                }
                            }
                            Button {
                                text: 'Убрать плакат'
                                visible: currentSelectedSiteId >= 420 && currentSelectedSiteId <= 425
                                onClicked: {
                                    map.engine.unRegisterObject( posterSetter.p, true )
                                    map.marker.visible = false
                                    posterSetter.p.destroy();
                                    posterSetter.p = null
                                }
                            }


                            Component {
                                id: poster
                                QtObject {
                                    id: root
                                    property real posX
                                    property real posY
                                    property string text
                                }
                            }

//                            Button {
//                                text: 'save location'
//                                onClicked: {
//                                    var aw = map.width / map.ls
//                                    var ah = map.height / map.ls
//                                    console.log('saving location to:',
//                                                map.sa.contentX,
//                                                map.sa.contentY,
//                                                map.sa.contentWidth,
//                                                map.sa.contentHeight,
//                                                map.sa.visibleArea.xPosition,
//                                                map.sa.visibleArea.widthRatio,
//                                                map.sa.visibleArea.yPosition,
//                                                map.sa.visibleArea.heightRatio,
//                                                aw, ah
//                                                )
//                                }
//                                Component.onCompleted: map.mi.model = mm
//                            }
//                            ListModel {
//                                id: mm
//                                ListElement {
//                                    px: 100
//                                    py: 6000
//                                }
//                            }

//                            Button {
//                                text: 'restore location'
//                                onClicked: {
//                                    map.da.centerSceneOn(8229.333333333332, 4410.666666666666)
////                                    map.da.fitInView( Qt.rect(8229.333333333332-500, 4410.666666666666-500, 8229.333333333332+500, 4410.666666666666+500))
//                                    console.log('restoring location to:', map.lx, map.ly, map.ls)
////                                    map.sa.contentX = 16204
////                                    map.sa.contentY = 8534
////                                    map.ls = 2
//                                    map.sa.visibleArea.xPosition = 0.5061649820410606
//                                    map.sa.visibleArea.widthRatio = 0.01644100580270793
//                                    map.sa.visibleArea.yPosition = 0.27490538289721184
//                                    map.sa.visibleArea.heightRatio = 0.014842778737984778
//                                }
//                            }
                            Item{ Layout.fillWidth: true }
                            Switch {
                                //                                    id: stateSwitch2
                                Layout.fillHeight: true
                                rotation: 180
                                onCheckedChanged: if( checked ) mapSideBar.implicitWidth = 250; else mapSideBar.implicitWidth = 0
                            }
                        }
                    }
                    Rectangle {
                        height: 1
                        color: '#a8a8a8'
                        Layout.fillWidth: true
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        RowLayout {
                            anchors.fill: parent
                            spacing: 0
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                clip: true
//                                Flickable {
//                                    anchors.fill: parent
//                                    contentWidth: image.width; contentHeight: image.height
//                                    Image {
//                                        id: image
//                                        //                    anchors.fill: parent
//                                        opacity: imgOp.value
//                                        source: 'maps.jpg'
//                                    }
//                                }

                                Map {
                                    id: map
                                    anchors.fill: parent
                                    onWidthChanged: {
                                        if( mainscreen.currentSelectedSiteId >= 420 || mainscreen.currentSelectedSiteId <= 425 )
                                            map.da.centerSceneOn(8229.333333333332, 4410.666666666666)
                                    }
                                }

                            }
                            Rectangle {
                                Layout.fillHeight: true
                                color: '#111'
                                width: 1
                            }


                            Rectangle {
                                id: mapSideBar
                                Layout.fillHeight: true
                                height: parent.height
                                //                                    implicitWidth: mainscreen.state === 'mapView' ? 300 : 0
                                Behavior on implicitWidth { NumberAnimation {  duration: 300; easing.type: Easing.OutExpo } }
                                color: '#333'



                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 10
                                    Rectangle {
                                        id: minimap
                                        color: '#555'
                                        height: 120
                                        Layout.fillWidth: true
                                        ColumnLayout {
                                            anchors.fill: parent
                                            Text {
                                                Layout.fillWidth: true
                                                height: 15
                                                text: 'Навигатор'
                                                color: '#eee'
                                            }
                                            Rectangle {
                                                Layout.fillWidth: true
                                                Layout.fillHeight: true
                                                Image {
                                                    source: 'maps.jpg'
                                                    width: parent.width
                                                    height: parent.height
                                                    Rectangle {
                                                        width: 30 * 3
                                                        height: 30 * 2
                                                        color: 'transparent'
                                                        border.color: '#6f6'
                                                        border.width: 3
                                                    }
                                                }

                                            }
                                        }
                                    }
                                    Item{ height: 15 }
                                    Rectangle {
                                        color: '#555'
                                        height: 80
                                        Layout.fillWidth: true
                                        ColumnLayout {
                                            anchors.fill: parent
                                            width: parent.width
                                            spacing: 0
                                            RowLayout {
                                                width: parent.width
                                                spacing: 0
                                                Layout.fillWidth: true
                                                Layout.minimumHeight: 30
                                                height: 30
                                                Rectangle {
                                                    width: parent.width
                                                    height: parent.height
                                                    Layout.fillWidth: true
                                                    color: "#62b6ff"
                                                    Text {
                                                        anchors.fill: parent
                                                        anchors.leftMargin: 5
                                                        verticalAlignment: Text.AlignVCenter
                                                        text: 'Схема участка'
                                                        font.pointSize: 18
                                                        color: '#000'
                                                    }
                                                    MouseArea {
                                                        anchors.fill: parent
                                                        cursorShape: Qt.PointingHandCursor
                                                        //                                                            onClicked: if( parent.color == '#555' ) parent.color = '#565'; else parent.color = '#555'
                                                    }
                                                }

                                            }
                                            Rectangle {
                                                Layout.fillWidth: true
                                                height: 1
                                                color: '#ccc'
                                            }
                                            Item {
                                                Layout.fillWidth: true
                                                Layout.fillHeight: true
                                                ColumnLayout {
                                                    anchors.fill: parent
                                                    //                                                        RowLayout {
                                                    //                                                            Button {
                                                    //                                                                checkable: true
                                                    //                                                                iconSource: 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAABHNCSVQICAgIfAhkiAAAAAlwSFlzAAAA6gAAAOoB+Ss1JgAAABl0RVh0U29mdHdhcmUAd3d3Lmlua3NjYXBlLm9yZ5vuPBoAAAHGSURBVFiFvdc9bhNBGAbgx8iiIxZ94gRoaPgRCqKKxB3yY8ddDsMNED1po1wglcsU4QAoie1wAEB0yAlDsZuwrMdrr72bkb5ix6N5Xs14xmshBHUUHuEjhviBE7ycGFcTvoqvCLn6jd1aA6CBswh+W2N06l6Bd/hZEOIavUoDYB19PJ0zxBhvqsSH6cTHJVbipGr8FK20/+EcIb7Xhe/gHBszQoyWwdsYRPDddH8ToDjEpzrx2zqfsh0DtKrE9yL4GDuRI/oF7dLHcAF8N/28hWP/jmjjbs4S+BouF8RP0/7+xLxL4h3JrZbH9yL4EOulA9SJzwxQgHcj+HVZvDCA5Cf1ogTeieCDInxqgAJ8vyTenrnFFeDdRfGJAFiR3Fz3gscCbEbw3hR8P4JflsGnbcEzNBfE18rg/wXAAZ5nnrfqxvMBrnCDV+nzUQTvVYnfBcCTDLSd9n2YA79YBs8GOMhgh2nf4zTEEbam4KvL4NkAn3PL3Y18OZtV49kAo1yAgENs43XmdATJPVEJns5rI4Jn6w9epIM3sVIVHkLwAO8Vt294CyGEsxDCrxnjS7VmJMCV5B9OX/IGM6oSzLdGCo3uC8y3vxQIPvWH/xFSAAAAAElFTkSuQmCCd71ab3ba416e5b6696c19164b075846b'
                                                    //                                                                style: ButtonStyle {
                                                    //                                                                    background: Rectangle {
                                                    //                                                                        border.width: control.activeFocus ? 2 : 1
                                                    //                                                                        border.color: "#888"
                                                    //                                                                        radius: 4
                                                    //                                                                        gradient: Gradient {
                                                    //                                                                            GradientStop { position: 0 ; color: control.checked ? "#cfc" : "#eee" }
                                                    //                                                                            GradientStop { position: 1 ; color: control.checked ? "#afa" : "#ccc" }
                                                    //                                                                        }
                                                    //                                                                    }
                                                    //                                                                }
                                                    //                                                            }
                                                    //                                                        }
                                                    Label {
                                                        text: 'Видимость слоя'
                                                        color: '#eee'
                                                    }
                                                    Slider {
                                                        Layout.fillWidth: true
                                                        value : 1
                                                        updateValueWhileDragging : true
                                                        style: SliderStyle{
                                                            handle: Rectangle {

                                                                anchors.centerIn: parent
                                                                implicitWidth: 90
                                                                width: 40
                                                                implicitHeight: 30
                                                                height: 20
                                                                radius: 5
                                                                color: control.pressed ? "white" : "lightgray"
                                                                border.color: "gray"
                                                                border.width: 1
                                                                Text {
                                                                    anchors.centerIn: parent
                                                                    text: Math.round( control.value * 100 ) + '%'
                                                                }

                                                            }
                                                        }
                                                    }
                                                    Item {Layout.fillHeight: true}
                                                }
                                            }
                                        }
                                    }
                                    Rectangle {
                                        color: '#555'
                                        height: 220
                                        Layout.fillWidth: true
                                        ColumnLayout {
                                            anchors.fill: parent
                                            spacing: 0
                                            RowLayout {
                                                spacing: 0
                                                Layout.fillWidth: true
                                                Layout.minimumHeight: 30
                                                height: 30
                                                Rectangle {
                                                    width: parent.width
                                                    height: parent.height
                                                    Layout.fillWidth: true
                                                    color: '#555'
                                                    Text {
                                                        anchors.fill: parent
                                                        anchors.leftMargin: 5
                                                        text: 'Исходная схема'
                                                        color: '#eee'
                                                        font.pointSize: 18
                                                        verticalAlignment: Text.AlignVCenter
                                                    }
                                                    MouseArea {
                                                        anchors.fill: parent
                                                        cursorShape: Qt.PointingHandCursor
                                                        //                                                            onClicked: if( parent.color == '#555' ) parent.color = '#565'; else parent.color = '#555'
                                                    }
                                                }
                                            }
                                            Rectangle {
                                                Layout.fillWidth: true
                                                height: 1
                                                color: '#ccc'
                                            }
                                            Item {
                                                Layout.fillWidth: true
                                                Layout.fillHeight: true
                                                ColumnLayout {
                                                    anchors.fill: parent
                                                    Label {
                                                        text: 'Видимость слоя'
                                                        color: '#eee'
                                                    }
                                                    Slider {
                                                        id: imgOp
                                                        value : 1
                                                        Layout.fillWidth: true

                                                        updateValueWhileDragging : true
                                                        style: SliderStyle{
                                                            handle: Rectangle {

                                                                anchors.centerIn: parent
                                                                implicitWidth: 90
                                                                width: 40
                                                                implicitHeight: 30
                                                                height: 20
                                                                radius: 5
                                                                color: control.pressed ? "white" : "lightgray"
                                                                border.color: "gray"
                                                                border.width: 1
                                                                Text {
                                                                    anchors.centerIn: parent
                                                                    text: Math.round( control.value * 100 ) + '%'
                                                                }
                                                            }
                                                        }
                                                    }
                                                    //                                                        RowLayout {
                                                    //                                                            Button {
                                                    //                                                                checkable: true
                                                    //                                                                iconSource: 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAABHNCSVQICAgIfAhkiAAAAAlwSFlzAAAA6gAAAOoB+Ss1JgAAABl0RVh0U29mdHdhcmUAd3d3Lmlua3NjYXBlLm9yZ5vuPBoAAAHGSURBVFiFvdc9bhNBGAbgx8iiIxZ94gRoaPgRCqKKxB3yY8ddDsMNED1po1wglcsU4QAoie1wAEB0yAlDsZuwrMdrr72bkb5ix6N5Xs14xmshBHUUHuEjhviBE7ycGFcTvoqvCLn6jd1aA6CBswh+W2N06l6Bd/hZEOIavUoDYB19PJ0zxBhvqsSH6cTHJVbipGr8FK20/+EcIb7Xhe/gHBszQoyWwdsYRPDddH8ToDjEpzrx2zqfsh0DtKrE9yL4GDuRI/oF7dLHcAF8N/28hWP/jmjjbs4S+BouF8RP0/7+xLxL4h3JrZbH9yL4EOulA9SJzwxQgHcj+HVZvDCA5Cf1ogTeieCDInxqgAJ8vyTenrnFFeDdRfGJAFiR3Fz3gscCbEbw3hR8P4JflsGnbcEzNBfE18rg/wXAAZ5nnrfqxvMBrnCDV+nzUQTvVYnfBcCTDLSd9n2YA79YBs8GOMhgh2nf4zTEEbam4KvL4NkAn3PL3Y18OZtV49kAo1yAgENs43XmdATJPVEJns5rI4Jn6w9epIM3sVIVHkLwAO8Vt294CyGEsxDCrxnjS7VmJMCV5B9OX/IGM6oSzLdGCo3uC8y3vxQIPvWH/xFSAAAAAElFTkSuQmCCd71ab3ba416e5b6696c19164b075846b'
                                                    //                                                                style: ButtonStyle {
                                                    //                                                                    background: Rectangle {
                                                    //                                                                        border.width: control.activeFocus ? 2 : 1
                                                    //                                                                        border.color: "#888"
                                                    //                                                                        radius: 4
                                                    //                                                                        gradient: Gradient {
                                                    //                                                                            GradientStop { position: 0 ; color: control.checked ? "#cfc" : "#eee" }
                                                    //                                                                            GradientStop { position: 1 ; color: control.checked ? "#afa" : "#ccc" }
                                                    //                                                                        }
                                                    //                                                                    }
                                                    //                                                                }
                                                    //                                                            }
                                                    //                                                        }
                                                    Button {
                                                        Layout.fillWidth: true
                                                        text: 'Добавить схему...'
                                                    }

                                                    ListView {
                                                        clip: true
                                                        Layout.fillHeight: true
                                                        Layout.fillWidth: true
                                                        spacing: 3
                                                        model: ListModel {
                                                            ListElement { name:'scheme1.jpg'; ison: true }
                                                            ListElement { name:'scheme2.png'; ison: false}
                                                            ListElement { name:'scheme3.png'; ison: true}
                                                            ListElement { name:'scheme4.png'; ison: false}
                                                        }

                                                        delegate: Rectangle {
                                                            height: 30
                                                            width: parent.width
                                                            color: ListView.isCurrentItem ? '#ccf' : '#eee'
                                                            RowLayout {
                                                                anchors.fill: parent
                                                                Image {

                                                                    Layout.fillHeight: true
                                                                    opacity: !ison ? 0.3 : 1
                                                                    fillMode: Image.PreserveAspectFit
                                                                    source: 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAYAAADgdz34AAAABHNCSVQICAgIfAhkiAAAAAlwSFlzAAAAiwAAAIsBRmWzPwAAABl0RVh0U29mdHdhcmUAd3d3Lmlua3NjYXBlLm9yZ5vuPBoAAAELSURBVEiJ3dU9TgMxEAXgz4gyN6AEmuQEXCGiSZ0q16DYS2wF10iZMseAIg0tRdLHKXAkY7xsULSRYKRpZsbvzY89DjFGQ8rVoOj/guC6LyCEEHCDe9wl8xte8R77hhhjrCrGeMYOsUN3KWbciVMBnmBVAG3QYpG0TbY8ZoXJjwSYVTJ+waiSyCj5yopmVQI02JfgXaVn50qSPZovBJhX+rspM8cTlpgiZJWU7YqYH+d/i20loK1k22T+aWZvK+e3Cdu644YsiswbfGT+ZeZfdGCse99Bkgc8nhj7TXpb5LPnpf+0Fp0yZIREsvTbIV/kml7koV1kVQyx7I6D6pRz13Uvwbny97/MwQkOqjHzKmBCG6UAAAAASUVORK5CYII615297f20a7431067eeef9e41f473b25'
                                                                }
                                                                Text{

                                                                    Layout.fillWidth: true
                                                                    Layout.fillHeight: true
                                                                    text: name
                                                                    color: '#444'
                                                                }
                                                                Item{
                                                                    Layout.fillHeight: true
                                                                    width: 20
                                                                    Image {
                                                                        anchors.centerIn: parent
                                                                        width: 15
                                                                        height: 15
                                                                        fillMode: Image.PreserveAspectFit
                                                                        source: 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAYAAADgdz34AAAABHNCSVQICAgIfAhkiAAAAAlwSFlzAAAAtQAAALUBOdDOnwAAABl0RVh0U29mdHdhcmUAd3d3Lmlua3NjYXBlLm9yZ5vuPBoAAAElSURBVEiJndY5TsNAGIDRJ+6AcIHEldKlYClCNnZuRQFIQMMpaJBQCog4BC1kKLCRCZ7x2JamsX+/z5E9UoQQYAtXeMIghKDPwgGWeMROec4mXhDK9YndHvgRVjVniW24qZ3sFWnAq/WgLK1fyI4k8IBX2E8MJCMt+ArDavA4MhSNZOCjEIL6DSe5kQz88Hd27alO2yIZ+PiP2fDTzxKRrxZ88s+LvLxUJIZPG63EF3LeAZ9FnZZv/CIDn6eMDenjo4RiRyhnEhPxpz8Uf6H5mzGCjzPx1kgTPkngqWjzjl/Dpy34CJddInV81obXZrMj1Q3zXLxrBPa64h0iA3jrg2dGnuGuL54RuYYCi754IrJAUV0scOvn38WwK762+99xjyKE4BswnKLOHDFe4gAAAABJRU5ErkJggga43b5a872f5ecd3724441b2ad9e81050'
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    Rectangle {
                                        color: '#555'
                                        height: 128
                                        Layout.fillWidth: true
                                        ColumnLayout {
                                            spacing: 0
                                            anchors.fill: parent
                                            RowLayout {
                                                spacing: 0
                                                Layout.fillWidth: true
                                                height: 15
                                                Text {
                                                    anchors.fill: parent
                                                    anchors.leftMargin: 5
                                                    text: 'Карта Москвы'
                                                    color: '#eee'
                                                    verticalAlignment: Text.AlignVCenter
                                                }
                                            }
                                            Rectangle {
                                                Layout.fillWidth: true
                                                height: 1
                                                color: '#ccc'
                                            }
                                            GridLayout {
                                                Layout.fillWidth: true
                                                Layout.fillHeight: true
                                                columns: 2
                                                rows: 2
                                                ExclusiveGroup {id: eg1}
                                                RadioButton {
                                                    exclusiveGroup: eg1
                                                    text: 'Карта'
                                                    style: RadioButtonStyle {
                                                        label: Text {
                                                            text: control.text
                                                            color: '#ccc'
                                                        }
                                                    }
                                                }
                                                CheckBox {
                                                    text: 'Оттенки серого 50шт.'
                                                    style: CheckBoxStyle {
                                                        label: Text {
                                                            text: control.text
                                                            color: '#ccc'
                                                        }
                                                    }
                                                }
                                                RadioButton {
                                                    exclusiveGroup: eg1
                                                    text: 'Цвет'
                                                    style: RadioButtonStyle {
                                                        label: Text {
                                                            text: control.text
                                                            color: '#ccc'
                                                        }
                                                    }

                                                }
                                                RowLayout {
                                                    Rectangle {
                                                        width: 15
                                                        height: 15
                                                        border.width: 1
                                                        border.color: '#ccc'
                                                        color: colorDialog.color
                                                        ColorDialog {
                                                            id: colorDialog
                                                            color: '#fff'
                                                        }
                                                        MouseArea {
                                                            anchors.fill: parent
                                                            onClicked: {
                                                                colorDialog.setColor( parent.color )
                                                                colorDialog.open()
                                                            }
                                                        }
                                                    }
                                                    Text{
                                                        text: 'цвет заливки'
                                                        color: '#ccc'
                                                    }
                                                }
                                            }
                                            Item{ Layout.fillHeight: true}
                                        }
                                    }
                                    Item{ Layout.fillHeight: true }
                                }
                            }
                        }
                    }
                }
            }
        }
    }


}
