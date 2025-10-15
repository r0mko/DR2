import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.2
import Test 1.0



TableView {
    id: root
    property alias currentSelectedSiteId: treeModel.currentSelectedSiteId
    //    onCurrentSelectedSiteIdChanged: treeModel.setСurrentSelectedSiteId( currentSelectedSiteId )
    property var treeData: []
    model: treeModel

    headerDelegate: headerDelegateComponent
    onDoubleClicked: expandCollapse( row )
    function expandCollapse( row ) {
        var i = 1
        var rows = Number( root.model.item( row ).count )
        if( rows < 1 ) return
        var lvl = Number( root.model.item( row ).level )
        if( lvl > 2 ) return

        if( treeData[ row ].expanded ){
            treeData[ row ].expanded = false
            for( i = row + 1; i < model.rowCount(); ++i ) {
                if( model.item( i ).level <= lvl ) break;
                treeData[ i ].expanded = false
                treeData[ i ].visible = false
            }
        }else{
            treeData[ row ].expanded = true
            while( rows !== 0 ) {
                if( model.item( row + i ).level == lvl + 1 ) {
                    treeData[ row + i ].visible = true
                    rows--
                }
                i++
            }
        }
        treeDataChanged()
    }

    function collapseAll(){
        positionViewAtRow( 0 )
        for( var i = 0; i < treeData.length; ++i ) {
            treeData[ i ].visible = model.item( i ).level == 1 ? true : false
            treeData[ i ].expanded = false
        }
        treeDataChanged()
    }
    function expandAll(){
//        console.log()
        for( var i = 0; i < treeData.length; ++i ) {
            if( model.item( i ).count > 0 )
                treeData[ i ].expanded = true
            treeData[ i ].visible = true
        }
        treeDataChanged()
    }
/**/
    rowDelegate: Rectangle {
        visible: root.treeData[ styleData.row ] !== undefined ? root.treeData[ styleData.row ].visible : false
        height: root.treeData[ styleData.row ] !== undefined ? root.treeData[ styleData.row ].visible ? 24 : 0 : 0

//        Behavior on height { NumberAnimation { duration: 20} }


        color: if( root.model.item( styleData.row ).type == 1 ) return '#10aaaaff'
               else if( root.model.item( styleData.row ).type == 2 ) return '#10aaffaa'
        //            else if( root.model.item( styleData.row ).type === 'wire' ) return '#10ffffaa'
               else if( root.model.item( styleData.row ).type == 3 ) return '#10ffaaaa'
               else return '#20ffffff'

        Rectangle {
            anchors.bottom: parent.bottom
            width: parent.width
            color: '#20000000'
            height: root.treeData[ styleData.row ] !== undefined ? root.treeData[ styleData.row ].expanded || root.model.item( styleData.row ).count == 0 ? 1 : 3 : 0
        }
        MouseArea {
            anchors.fill: parent
            enabled: true
            acceptedButtons: Qt.RightButton
            onPressed: {
                var i = root.model.item( styleData.row )
//                console.log( i.name )
                if( mouse.button === Qt.RightButton && i.wired == '1') {
                    contextMenuWired.itemName = i.name
                    contextMenuWired.popup()
                }
                else if( mouse.button === Qt.RightButton && i.type == '2' ) {
                    contextMenuType.itemName = i.name
                    contextMenuType.popup()
                }
                else if( mouse.button === Qt.RightButton && i.type == '1'  &&  ( i.count == 0 || ( i.count > 0 && root.model.item( styleData.row + 1 ).type == '3' ) ) ) {
                    contextMenuType.itemName = i.name
                    contextMenuType.popup()
                }
                else if( mouse.button === Qt.RightButton && i.type == '3' ) {
                    contextMenuElement.itemName = i.name
                    contextMenuElement.popup()
                }

                mouse.accepted = false
            }
            onClicked: {
                if( contextMenuElement.visible ) contextMenuElement.visible = false
                if( contextMenuWired.visible ) contextMenuWired.visible = false
                if( contextMenuType.visible ) contextMenuType.visible = false
                mouse.accepted = false
            }
            onReleased: {
                if( contextMenuElement.visible ) contextMenuElement.visible = false
                if( contextMenuWired.visible ) contextMenuWired.visible = false
                if( contextMenuType.visible ) contextMenuType.visible = false
                mouse.accepted = false
            }
        }
    }
    /**/
    PartModel {
        id: treeModel
        modelService: service
        dataSource: 'tree'
        currentSelectedSiteId: -1
        onCurrentSelectedSiteIdChanged: {root.collapseAll()}
//        root.treeData = []


        onDataUpdated: {

//            console.log( 'ROW COUNT IN TREE', rowCount() )
            root.treeData = []
            var td = []
            for( var i = 0; i < rowCount(); ++i ){
                td[ i ] = {
                    expanded: false,
                    visible: root.model.item( i ).level == 1 ? true : false,
                }
            }
            root.treeData = td
            for( var i = 0; i < rowCount(); ++i ){
                if( root.model.item( i ).level == 1 && root.model.item( i ).count > 0 )
                    if( root.model.item( i + 1 ).type == 2 )
                        root.expandCollapse( i );
            }

        }
    }

    TableViewColumn{
        role: 'name'
        title: 'Элементы КС'
        width: 400
        delegate: RowLayout {
            anchors.fill: parent

            anchors.leftMargin: 5 + root.model.item( styleData.row ).level * 20 - 20
            visible: root.treeData[ styleData.row ] !== undefined ? root.treeData[ styleData.row ].visible : false
            clip: true
            Item {
                Layout.fillHeight: true
                width: 12
                Image {
                    anchors.centerIn: parent
                    visible: root.treeData[ styleData.row ] !== undefined ? root.model.item( styleData.row ).count > 0 && root.treeData[ styleData.row ].visible : false
//                    transform: Translate { x:0.5; y: 0.5}
                    width: 10
                    height: 10
                    fillMode: Image.PreserveAspectFit
                    source: root.treeData[ styleData.row ] !== undefined ? root.treeData[ styleData.row ].expanded ? "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQBAMAAADt3eJSAAAAA3NCSVQICAjb4U/gAAAACXBIWXMAAAB9AAAAfQHHSA8AAAAAGXRFWHRTb2Z0d2FyZQB3d3cuaW5rc2NhcGUub3Jnm+48GgAAABhQTFRF////AAAAAAAAAAAEAAADAgACAQACAQACQBy5sgAAAAd0Uk5TACowQFeI0NUc1BEAAAArSURBVAjXYyiHAoYSJTBwZyhiAAN14hhhaUCQCmQIGwOBoTpp2uGWwpwBAP9JFQ5bjaspAAAAAElFTkSuQmCC114d8c1e9f35a33ec205d35cfbc05a54" : "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQBAMAAADt3eJSAAAAA3NCSVQICAjb4U/gAAAACXBIWXMAAAClAAAApQFwxN5VAAAAGXRFWHRTb2Z0d2FyZQB3d3cuaW5rc2NhcGUub3Jnm+48GgAAACRQTFRF////AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAxyY+ZQAAAAt0Uk5TACowM0BXf4im0Olv4LtHAAAAO0lEQVQIHWPYDQUMW5XAIJphEwMDWwIDgzaIwV5AiFE5c9bKmdOBikVcPFpcHInUxcDAbAAyGW4pzBkAFAwjROfMhgsAAAAASUVORK5CYII101a1c940c44df601ecea7ab8d758499" : ''
                    MouseArea {
                        enabled: root.model.item( styleData.row ).count > 0
                        anchors.fill: parent
                        onClicked: root.expandCollapse( styleData.row )
                    }
                }
            }
            Item {
                Layout.fillHeight: true
                width: 18
                Image {
                    anchors.centerIn: parent
                    visible: root.treeData[ styleData.row ] !== undefined ? root.treeData[ styleData.row ].visible : false
                    width: 18
                    height: 18
                    fillMode: Image.PreserveAspectFit
                    source: if( root.model.item( styleData.row ).type == 1 && root.model.item( styleData.row ).wired == 1 ) return "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAQAAABKfvVzAAAAAnNCSVQICFXsRgQAAAAJcEhZcwAAAIsAAACLAUZlsz8AAAAZdEVYdFNvZnR3YXJlAHd3dy5pbmtzY2FwZS5vcmeb7jwaAAACLklEQVQ4T43TW2gWZBgH8N/n3EJdRGqygYspYhpSVAjeTI2mXjQrFCmzokjJUBARg0iwBFEj8xQ0oZCCsuyigyXK0orohClZWsLMkEYYOnUpJHPf/l7s4PcNBHku3sPD7/nzXryitEy10gdanfeVjR43srwfpdsan4ooOuYbHSLOmHcd4DFnxTumqY5QMN5Cp8WHpTl9y9PiH00K5trmJyd95AWjjbBT/KqqDBjtglNGqLdfxFnHFUWHhWGVWFsO9ooZ7nBRt2ZjIlSbo01sVeFHXSb3A7NFswo/KHowTLbGm551k1t9r1ujCf733TWwXkyyWGwIqxX19P5wu7H+81f4XGfPO4QWl1TYpcswjeKIOSZ6XbSETWKsl8V9faDdt+FPv4W3xP294YfEbRaIeZrEogiGivcVXLYn7NNpaC94Q9xjqljuLrGxL6HV0fCztvBa/6Qq/7piiKWi0RNicx/YqajadjFenfM6veclJ8Xa8LEYbkspWCEe0KDb1wqmaBVR9KpKD4vd4WApuNMVhw22SWw3TKV7zTIqzHRGu1oLpASEV8RqQ3wpTliuwQTzvSs6NKnVPhBU+kWnRxQscUn6q0WdkQ6I5jIQ7nZO7HCLm02zwjqPGhceclq8rX4ACLU+E3/b7CmT1JjlRV+IdvMN8ozYrb4ERHhSW9lF0SdqwnO952MGlfUj1Jlrgx2Waej5e2GwgyKmD0i4ftkmulTdOFgqTsSNgzq/ez7iKq+Dqq41tN9gAAAAAElFTkSuQmCCbeaf4e3e3fae02f550408b5f6ba61e17"
                            else if( root.model.item( styleData.row ).type == 1 ) return "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAYAAADgdz34AAAABHNCSVQICAgIfAhkiAAAAAlwSFlzAAAA5AAAAOQBXaJ52AAAABl0RVh0U29mdHdhcmUAd3d3Lmlua3NjYXBlLm9yZ5vuPBoAAAIqSURBVEiJvda/b1ZlFMDxzzm3ATGavLZoKwmJJm4MDISBQpiMm+gfwMzooIz8C5owsfAHOLa4mbAYKQthYYREEg32VVqaaICY9jkO7229wMtAf/gkZ3ru8z2553vOfW5UlWkrIj4mLyRnShyjFvBhv/07sRrqUeM27UZV/TIVVFUvBN3FkPdC1hvGPbqLr/D+A5sLsbQL8EsRS5h7JUHKq3uHTyLl1W1uVJWIyJBrGE2t45uvjdLmqqpl76EFy/sEFyxXVdPD4RBOhu7O3kvU3cFJHNpxgFHI+zhBXgn5ZBfwJ+QVnOhZo6oyM3izT0LeDb5v2qfMzKb2ZXEW83gfXf/sFv7EOLjV5BKb6ym/Knm3r8ikXL3kIyE3hht4GPzUxM/Er2yu9WCTRDNz1PFU54rz+Ghw9p/SRlX1bNim1/exTa+/MAc4jFOhW9kHySs4hcMvS/4Np8nLIce7gI/Jyzjds0bDQRtNukCjlkt9i5byQokz1DEs4N2+xn9hlXgU6nbTbiBDfEN8gSztvara2E7wVsh1HBmIWp1I1kveeoyn/d7bdEcnkm1LXhicfVbabFU9H0q+to+Sr71Gctzce4K4+TrJq1ikuxTywS7gD+guYbFnTZWM+rF037E5Jj8PdZaYxwd9wB+TqHGJW7QfmJkPW18Tn8E0yWsTeTvrb2qldibZY7bW+0GexVHqeKhzxCLeGZx92n+unx/4hfP/XZkHdenHQf+2/At9gE9+XnvqJAAAAABJRU5ErkJggg99fe824f3567165e7abf6f9e84d22211"
                            else if( root.model.item( styleData.row ).type == 2 ) return "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAMAAADXqc3KAAAAA3NCSVQICAjb4U/gAAAACXBIWXMAAACmAAAApgHdff84AAAAGXRFWHRTb2Z0d2FyZQB3d3cuaW5rc2NhcGUub3Jnm+48GgAAAE5QTFRF////AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAxAnpMQAAABl0Uk5TAAcSGB4jJygwO0hRYWJrbnDK09bY8fL7/tafhCAAAABgSURBVCjPlZFJEoAgEAOjoghuuEL+/1HvTOVAH5OqZBagmS2lEUuqWUEy4aFBGx+54TT6q8tDjD3mWBNQyAO3iSq6PJM7LqNnOO87DFON04aMkuVyXLmgRB6x/R/ytc383DYfgfgDzEIAAAAASUVORK5CYIIa81f0b3ee53a44dd718151920d91c2a6"
                            else if( root.model.item( styleData.row ).type == 3 ) return "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAYAAADgdz34AAAABHNCSVQICAgIfAhkiAAAAAlwSFlzAAAArAAAAKwBhgk01AAAABl0RVh0U29mdHdhcmUAd3d3Lmlua3NjYXBlLm9yZ5vuPBoAAAF8SURBVEiJrdS/S5dRFMfx15NWiIsuBRGJtKlDRsN3CGlxcBUaGgRxcuwfkFycIveGlsagLVpyaKmGpsgpKSI0B38hGBWEt6FrPT18n/u9z9fvgcPD5Zz7eT/nnHuvEIJeOIawii08wkgIQU/EI2AJoeSP/wJQYBoP0OoSsF4B7JYBVfpcQ/HzbQAPy4DNSvBVA/FBrMV9L/EeK7hQBqxVAC8yxYfxJu5ZbpsTE6ewUQJ8w80O4hfxDse4W5tX2lBgIvo2DnGjRvwKPuAX5pM/UiMwgV3sYRb38AQLaOELfmK2YxsTLbgeqwht/AjTWXNKAPqwUwN4nn3KEoCpGvGTCs7lAM6ot/5EbABnE/F/lqigHwc1Fbw+dYsi5A6+V8T3ca0ngAi5FYU/4j4u54rnAloRsNhEOGfIJzYev+tZQ61YDmDsNIBOF23RnyfjB65206IU4Lb/T8+nXs9gprIeLYriUtMOpQDPKuu3IYSvTQGdbvISPuMpJrtp0W8/SgqVBm8W9AAAAABJRU5ErkJggge039368d21ee6c1a2423ab722b510bf2"
                    else return ''
                }
            }
            Text {
                Layout.fillWidth: true
                Layout.fillHeight: true
                verticalAlignment: Text.AlignVCenter
                text: String( styleData.value )
                elide: Text.ElideMiddle
                font.pixelSize: 18
            }
        }
    }
    TableViewColumn {
        role: 'el_count'
        title: 'Кол-во'
        width: 100
        delegate: Text {
            font.pixelSize: 18
            visible: root.treeData[ styleData.row ] !== undefined ? root.treeData[ styleData.row ].visible && root.model.item( styleData.row ).el_count > 0 && 0 != styleData.value : false
            Layout.fillWidth: true
            Layout.fillHeight: true
            anchors.rightMargin: 10
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignRight
            text: String( styleData.value ) + '   '
            elide: Text.ElideMiddle
        }
    }
    TableViewColumn {
        role: 'si'
        title: 'Ед. Изм.'
        width: 100
        delegate: Text {
            font.pixelSize: 18
            visible: root.treeData[ styleData.row ] !== undefined ? root.treeData[ styleData.row ].visible && root.model.item( styleData.row ).el_count > 0 && 0 != styleData.value : false
            Layout.fillWidth: true
            Layout.fillHeight: true
            verticalAlignment: Text.AlignVCenter
            text: String( styleData.value )
            elide: Text.ElideMiddle
        }

    }
    TableViewColumn {
        role: 'mount_date'
        title: 'Установлен'
        width: 120
        delegate: Text {
            font.pixelSize: 18
            visible: root.treeData[ styleData.row ] !== undefined ? root.treeData[ styleData.row ].visible : false
            Layout.fillWidth: true
            Layout.fillHeight: true
            verticalAlignment: Text.AlignVCenter
            text: Qt.formatDate( styleData.value, 'dd.MM.yyyy' )
            elide: Text.ElideMiddle
        }

    }
    TableViewColumn {
        role: 'dismount_date'
        title: 'Демонтирован'
        width: 120
        delegate: Text {
            font.pixelSize: 18
            visible: root.treeData[ styleData.row ] !== undefined ? root.treeData[ styleData.row ].visible : false
            Layout.fillWidth: true
            Layout.fillHeight: true
            verticalAlignment: Text.AlignVCenter
            text: Qt.formatDate( styleData.value, 'dd.MM.yyyy' )
            elide: Text.ElideMiddle
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
    Menu {
        id: contextMenuElement
        property alias itemName: contextMenuElementName.text
        MenuItem {
            id: contextMenuElementName
            enabled: false
        }
        MenuSeparator {}
        MenuItem {
            text: qsTr('Редактировать запись')
//            onTriggered: dialog.initEdit()
        }
        MenuItem {
            text: qsTr('Удалить запись')
//            onTriggered: dialog.initDelete()
        }
    }
    Menu {

        id: contextMenuWired
        property alias itemName: contextMenuWiredName.text
        MenuItem {
            id: contextMenuWiredName
            enabled: false
            text: parent.itemName === undefined ? '' : parent.itemName
        }
        MenuSeparator {}
        MenuItem {
            text: qsTr('Редактировать запись')
//            onTriggered: dialog.initEdit()
        }

    }
    Menu {
        id: contextMenuType
        property alias itemName: contextMenuTypeName.text
        MenuItem {
            id: contextMenuTypeName
            enabled: false
        }
        MenuSeparator {}
        MenuItem {
            text: qsTr('Добавить элемент')
//            onTriggered: dialog.initDelete()
        }
        MenuItem {
            text: qsTr('Привязать элемент')
//            onTriggered: dialog.initDelete()
        }
    }

}

