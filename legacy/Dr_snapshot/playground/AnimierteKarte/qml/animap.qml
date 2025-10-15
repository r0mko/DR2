import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Window 2.2

import Dr.Graphics 1.0
import Dr.UI 1.0
import Dr.MetaGraph 1.0
import Dr.Cartograph 1.0

import Playground 1.0

import "ValueEditors"
ApplicationWindow {
    id: mainWindow
    width: 1920
    height: 1000
    x: 0
    y: 0
    title: "Анимированная карта"
    //    samples: 16
    //    screenId: 1
    visible: true
    color: "#fff"

    MenuBar {
        Menu {
            title: "Файл"
            MenuItem { action: a_openModel }
            MenuItem { action: a_saveAs }
            MenuItem { action: a_save }
            MenuItem { action: a_reloadModel }
        }
    }

    Action {
        id: showVehicles
        shortcut: "h"
        checkable: true

    }

    TransportSimulator {
        id: transport
        property GeoView geoView: null
        property var vehs: []
        Component.onCompleted: initializeFromOverpassXML("/Users/Tox/data_large")

        onVehicleUpdated: {
            if (!geoView)
                return


            if (!vehs[id]) {
                vehs[id] = createVehicle(geoView.getPoint(pos), type, routeId)
            }
            vehs[id].position = geoView.getPoint(pos)
            vehs[id].visible = showVehicles.checked
        }

        function createVehicle(position, type, routeId) {
            var i = m_sceneBuilder.addItem("PathItem", {
                                               scale: 0.1,
                                               position: position,
                                               shapeType: ShapeInstance.Polygonal,
                                               flags: VisualInstance.ItemIgnoresTransformations

                                           })
            var p = creatist.create("PainterPath")
            p.addEllipse(Qt.point(0,0), 15, 15)
            i.path = p
            i.pen.color = '#000000'
            i.pen.width = 1
            i.pen.cosmetic = true
            if (type === 2) // troll
                i.brush.color = "#ffee00"
            else
                i.brush.color = "#ff0000"
            i.brush.style = Qt.SolidPattern


            var t = m_sceneBuilder.addItem("TextItem", {
                                               text: routeId,
                                               horizontalAlignment: Qt.AlignHCenter,
                                               verticalAlignment: Qt.AlignVCenter,
                                               flags: VisualInstance.ItemIgnoresTransformations
                                           }, i)
            return i

        }

        onInitialized: {
            start(60)
        }

    }

    Component.onCompleted: gmodel.init()

    property bool atExistingGuide: false

    SplitView {
        id: root
        anchors.fill: parent
        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true

            SceneModel {
                id: m_sceneBuilder
                scene: m_scene.qGraphicsScene
            }

            DrawArea {
                id: drawarea
                backgroundColor: "#fff"
                anchors.fill: parent
                scene.width: m_scene.sceneRect.width
                scene.height: m_scene.sceneRect.height
                maxScale: 512
                scaleAnimation.duration: 300

                //                scrollArea.onContentXChanged: gmodel.sceneData.contentX = scrollArea.contentX
                //                scrollArea.onContentYChanged: gmodel.sceneData.contentY = scrollArea.contentY
                //                onScaleChanged:               gmodel.sceneData.zoom = scene.scale


                property var dat: []
                property rect sourceRect: Qt.rect(scrollArea.contentX / scene.scale, scrollArea.contentY / scene.scale, width / scene.scale, height / scene.scale)
                grid.visible: false

                MouseArea {
                    id: scene_mouse
                    acceptedButtons: Qt.AllButtons
                    anchors.fill: parent
                    onPressed: m_scene.clearSelection()
                }



                Scene {
                    id: m_scene
                    enableBsp: true
                    viewport: drawarea
                    width: sceneRect.width
                    height: sceneRect.height

                    sceneRect: Qt.rect(0, 0, 16000, 16000)
                    //                                                    renderer.renderTarget: SceneRenderer.Image
                    //                                                    renderer.cacheEnabled: true
                    renderer.renderTarget: SceneRenderer.Image
                    //                    renderer.renderTarget: SceneRenderer.FramebufferObject
                    renderer.z: 1000
                    property real visualScale: 1 / (viewportScale >= 4 ? viewportScale : 2 * Math.sqrt(viewportScale))

                    //                    onViewportScaleChanged: console.log("vscale", viewportScale, visualScale)
                    Loader {
                        id: gview
                        property GeoView view: item
                        active: spview.linked
                        anchors.fill: parent
                        sourceComponent: GeoView {
                            id: g_view
                            //                            visible: false
                            //                    parent: drawarea.scene
                            anchors.fill: parent
                            contentWidth: 16000
                            contentHeight: 16000
                            targetSRID: spview.handle.data.targetSRID
                            //                                                        onViewExtentChanged: console.log("view extent", viewExtent)
                            onViewTransformChanged: transport.geoView = g_view
                        }
                    }


                    MarkerItem {
                        id: hover_handle
                    }

                    MarkerItem {
                        id: drag_handle
                        border.color: "#88ffff"
                    }

                    Repeater {
                        id: i_markers
                        model: points_model
                        delegate:  MarkerItem {
                            border.color: "#5030ff"
                            width: 8
                            height: 8
                            radius: 4
                            x: xpos
                            y: ypos
                            rotation: angle
                            visible: true
                        }
                    }

                }
            }

            TextLayer {
                id: textLayer
                anchors.fill: parent
                font.family: 'Roboto'
                //                transform: Translate {
                //                    // FIXME: that is a problem - we have to fix viewportTransform
                //                    y: -79 //ubuntu menu height
                //                }
                spacing: 20
                depth: 12
                color: '#333'
                viewTransform: m_scene.viewTransform
                viewportRect: m_scene.viewportRect
                outline: true
                outlineColor: "#fff"
                //                onEmerged: fadein.restart()
                //                onVanished: opacity = 0

                //                NumberAnimation on opacity {
                //                    id: fadein
                //                    from: 0
                //                    to: 1
                //                    duration: 500
                //                    easing.type: Easing.OutQuad
                //                    running: false

                //                }
            }




            Action {
                onTriggered: textLayer.visible ^= true
                shortcut: 'T'
            }


            SceneItemStroker {
                id: m_selectionOutline
                anchors.fill: drawarea
                //                penStyle: Qt.DotLine
                //                color: "#088"
                scene: m_scene
            }
            ShaderEffectSource {
                id: m_selectionOutlineSrc

                sourceItem: m_selectionOutline
                anchors.fill: drawarea
                hideSource: true
                opacity: 0.4
                visible: false
            }
            SelectShaderEffect {
                id: outliner
                anchors.fill: drawarea
                source: m_selectionOutlineSrc
                animationSpeed: 83

            }

        }


        ExclusiveGroup {
            id: g_addSection
            Action {
                id: a_addTramSection
                checkable: true
                onTriggered: {
                    if (checked && !network_model.linked) {
                        m_scene.clearSelection()
                        nets_panel.selection.clear()
                        f_Node.parentHandle = undefined
                        f_Node.create("contact_net")
                    }
                    if (checked) {
                        route.currentGraph = tramGraph
                        route.currentSectionType = "TramSection"
                    } else {
                        route.currentGraph = null
                        route.currentSectionType = ""
                    }
                }
            }
            Action {
                id: a_addTrollSection
                checkable: true
                onTriggered: {
                    if (checked && !network_model.linked) {

                        m_scene.clearSelection()
                        nets_panel.selection.clear()
                        f_Node.parentHandle = undefined
                        f_Node.create("contact_net")
                    }
                    if (checked) {
                        route.currentGraph = roadGraph
                        route.currentSectionType = "TrolleySection"
                    } else {
                        route.currentGraph = null
                        route.currentSectionType = ""
                    }
                }
            }

            Action {
                id: a_addSegment
                enabled: nets_panel.selection.count == 1
                checkable: true
            }

        }

        Sidebar {
            id: m_sidebar
            Layout.fillHeight: true
            Layout.minimumWidth: 320
            property real curveSmoothing: 10
            property real curvePrecision: 4
            property real baseCurveOffset: 0
            property bool debugMode: false


            SidebarPanel {
                id: guide_panel
                title: "Линии"

                JSObjectView {
                    id: paths_props
                    Layout.fillWidth: true
                    onImplicitHeightChanged: { guide_panel.contentHeight = implicitHeight + 4 }
                    JSObjectViewRow {
                        text: "Скругление"
                        valueDelegate: TinySlider {
                            minimumValue: 0
                            maximumValue: 150
                            value: m_sidebar.curveSmoothing
                            onValueChanged:  if (pressed)
                                                 m_sidebar.curveSmoothing = value
                        }
                    }
                    JSObjectViewRow {
                        text: "Точность"
                        valueDelegate: TinySlider {
                            minimumValue: 0
                            maximumValue: 10
                            value: m_sidebar.curvePrecision
                            onValueChanged:  if (pressed) m_sidebar.curvePrecision = value

                        }
                    }
                    JSObjectViewRow {
                        text: "Масштабирование"
                        valueDelegate: CheckBox {
                            checked: m_scene.notifyViewTransformUpdates
                            onClicked: m_scene.notifyViewTransformUpdates = checked
                        }
                    }
                    JSObjectViewRow {
                        text: "Показать узлы"
                        valueDelegate: CheckBox {
                            checked: m_sidebar.debugMode
                            onClicked: m_sidebar.debugMode = checked
                        }
                    }
                }
            }

            NetsPanel {
                id: nets_panel
                model: network_model
            }

            //            SegmentsPanel {
            //            currentNetHandle: nets_panel.currentNetHandle
            //                id: segmentsPanel
            //            }


            SidebarPanel {
                id: tramgraph_panel
                title: "Граф трамвайных путей"
                JSObjectView {
                    id: rg_props
                    Layout.fillWidth: true
                    onImplicitHeightChanged: { tramgraph_panel.contentHeight = implicitHeight + 4 }
                    model: [
                        { "Адрес": gr_tram.mrl },
                        { "Кол-во рёбер": tramGraph.count },
                    ]
                    JSObjectViewRow {
                        text: "Отображать"
                        valueDelegate: CheckBox {
                            checked: tramGraph.visible
                            onClicked: tramGraph.visible = checked
                        }
                    }
                    //                    JSObjectViewRow {
                    //                        text: "Debug nodes"
                    //                        valueDelegate: CheckBox {
                    //                            checked: m_sidebar.debugMode
                    //                            onClicked: m_sidebar.debugMode = checked
                    //                        }
                    //                    }
                }
            }
            SidebarPanel {
                id: roadgraph_panel
                title: "Граф дорог"
                JSObjectView {
                    id: rg_props_2
                    Layout.fillWidth: true
                    onImplicitHeightChanged: { roadgraph_panel.contentHeight = implicitHeight + 4 }
                    model: [
                        { "Адрес": gr_roads.mrl },
                        { "Кол-во рёбер": roadGraph.count },
                    ]
                    JSObjectViewRow {
                        text: "Отображать"
                        valueDelegate: CheckBox {
                            checked: roadGraph.visible
                            onClicked: roadGraph.visible = checked
                        }
                    }
                    //                    JSObjectViewRow {
                    //                        text: "Debug nodes"
                    //                        valueDelegate: CheckBox {
                    //                            checked: m_sidebar.debugMode
                    //                            onClicked: m_sidebar.debugMode = checked
                    //                        }
                    //                    }
                }
            }

            BoundariesPanel {
                id: bounds_panel
                model: model_boundaries

            }
        }
    }

    GraphModel{
        id: cabelmodel
        DataNodeFactory {  }
        NodeFactory {  }
        MetaNodeFactory {  }
        CollectionFactory {  }
        TypeBuilder {  }
        BlobFactory {  }
        PointNodeFactory{  }
        RootNodeFactory { }
        //        Component.onCompleted: loadModel('file:///home/awl/cables2.mgf')
        Component.onCompleted: {
            console.log(   'CABLES LOADED',
                        loadModel('file:///Users/Tox/cables2.mgf')
                        )
            //            myPowerCabelsEdgeModel.go()
        }
    }
    EdgeModel {
        id: myPowerCabelsEdgeModel
        model: cabelmodel
        //            tag: 'polygon'
        mrl: "/power_cables"
        property var polys: null
        property var pwToData: {
            5601: 465188,
                    5602: 465195,
                    5605: 465271,
                    5606: 465278
        }
        function go()
        {
            console.log(   'CABLES GO ', count )
            for( var i = 0; i < count; ++i){
                var edgehandle = getHandle(i)
                console.log( 'CABLES GO ', i, edgehandle )
                var data = edgehandle.getData()
                var polygon = data.polygon
                var attributes = data.attributes
                var name = attributes.name_obj

                if( name == 5601 || name == 5602 || name == 5605 || name == 5606) {

                    var phandle = m_sceneBuilder.addItem("PolygonItem", {})
                    phandle.polygon = polygon
                    //                        phandle.pen.color = idfToData[ pwToData[name] ].color
                    phandle.pen.width = 0.07
                    //                        phandle.pen.cosmetic = true
                    phandle.shapeType = 1
                    //                        if( polys === null ) polys = {}
                    //                        polys[name].phandle = phandle

                }
            }
        }

        onModelReset: {
            console.log( 'MODEL RESET')
            //                go()
            //                for( var i = 0; i < count; ++i){
            //                    var edgehandle = getHandle(i)
            //                    var data = edgehandle.getData()
            //                    var polygon = data.polygon
            //                    var attributes = data.attributes
            //                    var name = attributes.name_obj

            //                    if( name == 5601 || name == 5602 || name == 5605 || name == 5606) {

            //                        var phandle = m_sceneBuilder.addItem("PolygonItem", {})
            //                        phandle.polygon = polygon
            //                        phandle.pen.color = idfToData[ pwToData[name] ].color
            //                        phandle.pen.width = 0.07
            //                        phandle.shapeType = 1
            //                        if( polys === null ) polys = {}
            //                        polys[name].phandle = phandle

            //                    }
            //                }
        }

        onItemAdded: {
            //                console.log( 'ITEM ADDED')
            //                var edgehandle = getHandleByUid(uid)
            //                var data = edgehandle.getData()
            //                var polygon = data.polygon
            //                var attributes = data.attributes
            //                var name = attributes.name_obj

            //                if( name == 5601 || name == 5602 || name == 5605 || name == 5606) {

            //                    var phandle = m_sceneBuilder.addItem("PolygonItem", {})
            //                    phandle.polygon = polygon
            //                    phandle.pen.color = idfToData[ pwToData[name] ].color
            //                    phandle.pen.width = 0.07
            //                    phandle.shapeType = 1
            //                    if( polys === null ) polys = {}
            //                    polys[name].phandle = phandle

            //                }
        }

        //        function update(){
        //            for( var i = 0; i < count ; ++i ){
        //                var edgehandle = getHandle(i)
        //                var data = edgehandle.getData()
        //                var attributes = data.attributes
        //                var name = attributes.name_obj
        //                if( name == 5601 || name == 5602 || name == 5605 || name == 5606) {
        //                    console.log( 'update!!!!!!!!!!!!!!!!!!!!!!!!!!!!!' )
        //                }
        //            }
        //        }
    }
    GraphModel {
        id: gmodel
        property url file
        property string fileName: file.toString().length ? file.toString().match(/(\w+)\..+$/)[1] : "untitled"
        property QtObject sceneData: spview.handle.dataHandle
        RootNodeFactory { }
        NodeFactory { id: f_Node }
        DataNodeFactory { id: f_DataNode }
        MetaNodeFactory { id: f_MetaNode }
        CollectionFactory { id: f_Collection }
        TypeBuilder { id: f_Type }
        BlobFactory { id: f_Blob }
        InstanceFactory { id: f_Instance }
        ElementFactory { id: f_Element }
        SqlBridgeNodeFactory { id: f_SqlBridgeNode }
        PointNodeFactory { id: f_PointNode }
        //        onLoaded: console.log("loaded model")


        ModelWatcher {
            id: gr_tram
            //            mrl: "/spview/road_graph"
            mrl: "/spview/tram_graph"

        }
        ModelWatcher {
            id: gr_roads
            mrl: "/spview/road_graph"

        }
        NodeChildrenModel {
            id: network_model
            mrl: "/contact_net"
            onDataChanged: {
                if (topLeft.row < 0) {
                    console.log("nets data change", topLeft.row, bottomRight.row)
                    return
                }
                var h = getHandle(topLeft.row)
                console.log("nets data change", topLeft.row, JSON.stringify(h.data))
            }
        }
        function init() {
            aux.initUI()
            //            if (!loadModel("/Users/romko/Desktop/moscow23.mgf"))
            //            if (!loadModel("/Users/romko/Desktop/routes_d.mgf"))
            //                if (!loadModel("/Users/Tox/routes_d.mgf"))
            if (!loadModel("/Users/Tox/rrr.mgf"))
                if (!loadModel("/Users/romko/Desktop/moscow_catenary.mgf"))
                    if (!loadModel("/Volumes/Romko/Desktop/trams.mgf"))
                        console.log("unable to auto-load model")
            f_Node.create("contact_net")
        }

    }

    AuxModel {
        id: aux
    }

    EdgeModel {
        id: myeNNNdgeModel
        model: geomodel
        tag: 'polygon'
        mrl: "/spview/road_graph"

        function addLabel(h) {
            var edgehandle = h
            var data = edgehandle.getData()
            var polygon = data.polygon
            var attributes = data.attributes
            if( attributes.name_2.length > 1 ) {
                var size = 90
                if( attributes.type_2 === 'primary' ) size = 16
                else if( attributes.type_2 === 'primary_link' ) size = 16
                else if( attributes.type_2 === 'secondary' ) size = 14
                else if( attributes.type_2 === 'secondary_link' ) size = 14
                else if( attributes.type_2 === 'tertiary' ) size = 12
                else if( attributes.type_2 === 'tertiary_link' ) size = 12
                textLayer.appendLabel(polygon, attributes.name_2, size)
            }
        }

        onModelReset: {
            for (var i = 0; i < count; ++i) {
                var h = getHandle(i)
                addLabel(h)
            }
        }
        onItemAdded: {
            //            addLabel(getHandleByUid(uid))
        }
    }



    GraphModel {
        id: geomodel
        RootNodeFactory { }
        NodeFactory {  }
        DataNodeFactory { }
        MetaNodeFactory     { }
        CollectionFactory   { }
        TypeBuilder         { }
        BlobFactory         { }
        InstanceFactory     { }
        ElementFactory      { }
        SqlBridgeNodeFactory { }
        PointNodeFactory    { }
        NodeChildrenModel {
            id: model_ds
            mrl: "spview/datasources"
        }

        NodeChildrenModel {
            id: model_layers
            mrl: "spview/layers"

        }
        NodeChildrenModel {
            id: cache_model
            mrl: "spview/cache"
        }
        ModelWatcher {
            id: spview
            mrl: "spview"
            onLinkedChanged: {
                if(linked && gmodel.sceneData.zoom !== undefined) {
                    drawarea.scrollArea.contentX = gmodel.sceneData.contentX
                    drawarea.scrollArea.contentY = gmodel.sceneData.contentY
                    drawarea.scene.scale = gmodel.sceneData.zoom
                }
            }
        }

        NodeChildrenModel {
            id: styles_model
            mrl: "spview/styles.test"
        }

        function init() {
            if (!loadModel("/Users/romko/Desktop/moscow24.mgf"))
                if (!loadModel("/Users/Romko/Desktop/lablz.mgf"))
                    if (!loadModel("/Users/Romko/moscow24.mgf"))
                        if (!loadModel("/Users/romko/Desktop/trams.mgf"))
                            if (!loadModel("/Volumes/Romko/Desktop/trams.mgf"))
                                console.log("unable to auto-load model")
        }
        Component.onCompleted: init()
    }


    SectionMaker {
        id: i_sections
        model: network_model
    }

    EdgeModel {
        id: model_boundaries
        model: gmodel
        tag: "BoundaryVertex"
        mrl: "/contact_net"
    }

    BoundsMaker { id: i_bounds }

    Action {
        id: a_split
        checkable: true
        enabled: network_model.count > 0
    }

    property int sectionId: 5000
    function createSection(type, name) {
        f_DataNode.parentHandle = network_model.rootHandle
        if (!name) {
            if (type === "TrolleySection")
                name = "TRB%1/%2".arg(Number(sectionId)).arg(Number(sectionId + 50))
            else if (type === "TramSection")
                name = "TRM%1".arg(Number(sectionId))
        }

        var hue = Math.random()
        var negHue = (hue + 0.5) % 1
        f_DataNode.data = { type: type, posColor: UI.fromHSLum(hue, 1, 0.4).toString(), negColor: UI.fromHSLum(negHue, 1, 0.4).toString(), posCableId: sectionId, negCableId: sectionId + 50 }
        sectionId++
        if (sectionId % 100 > 49)
            sectionId += 100 - (sectionId % 100)
        return f_DataNode.create(name)
    }

    RoadGraph {
        id: tramGraph
        model: gmodel
        rootIndex: gr_tram.modelIndex
        onMrlChanged: console.log("tram mrl:", mrl)

        onEdgeHoverLeave: hover_handle.visible = false
        onEdgeHoverEnter: {
            hover_handle.visible = true
            hover_handle.runner = runner
        }
        onEdgeHoverMove:  hover_handle.runner = runner
    }
    RoadGraph {
        id: roadGraph
        model: gmodel
        rootIndex: gr_roads.modelIndex
        //        rootIndex: gr_tram.modelIndex/
        onMrlChanged: console.log("roads mrl:", mrl)
        onEdgeHoverLeave: hover_handle.visible = false
        onEdgeHoverEnter: {
            hover_handle.visible = true
            hover_handle.runner = runner
        }
        onEdgeHoverMove: hover_handle.runner = runner
    }

    ListModel { id: points_model }

    RouteBuilder { id: route }

    Action {
        id: a_moveBoundary
        checkable: true
        tooltip: "Переместить изолятор"
    }

    Action {
        id: a_addConnection
        tooltip: "Добавить кабельный ввод"
        checkable: true
        enabled: nets_panel.selection.count === 1
    }

    Action {
        id: a_sectionSettings
        enabled: nets_panel.selection.count == 1
        property SectionSettingsWindow settingsWindow: SectionSettingsWindow {
            id: sectionSettingsWindow
            handle: nets_panel.currentNetHandle
        }
        onTriggered: {
            if (!sectionSettingsWindow.visible)
                sectionSettingsWindow.show()
            else
                sectionSettingsWindow.raise()
        }
    }

    Action {
        id: a_segmentSettings
        enabled: nets_panel.selection.count == 1
        onTriggered: {
            console.log(">>>>>>>>>> SHOW SEGMENT LIST & SETTINGS DIALOG!!!!")
        }
    }





    StateGroup {
        id: editor
        onStateChanged: console.log("Current editor state: %1".arg(state))
        states: [

            State {
                name: "addway"
                when: a_addTramSection.checked || a_addTrollSection.checked

                PropertyChanges {
                    target: tramGraph
                    interactive: true
                }
                PropertyChanges {
                    target: roadGraph
                    interactive: true
                }

                PropertyChanges {
                    target: route

                    onWayStarted: {
                        currentSectionHandle = a_addTramSection.checked ?
                                    createSection("TramSection", "TRM%1".arg((Number(sectionId)))) :
                                    createSection("TrolleySection", "TRB%1/%2".arg(Number(sectionId)).arg(Number(sectionId + 50)))
                    }

                    onWayFinished: {
                        addWay(currentSectionHandle)
                        console.log("Added way to", currentSectionHandle.mrl)
                        g_addSection.current.checked = false
                        hover_handle.visible = false
                    }
                }
            },

            State {
                name: "addSegment"
                when: a_addSegment.checked

                PropertyChanges {
                    target: tramGraph
                    interactive: nets_panel.currentNetHandle.data.type === "TramSection"
                }
                PropertyChanges {
                    target: roadGraph
                    interactive: nets_panel.currentNetHandle.data.type === "TrolleySection"
                }

                PropertyChanges {
                    target: route

                    onWayStarted: {
                        currentSectionHandle = nets_panel.currentNetHandle
                    }

                    onWayFinished: {
                        addWay(currentSectionHandle)
                        console.log("Added way to", currentSectionHandle.mrl)
                        a_addSegment.checked = false
                        //                        a_addTramSection.trigger()
                        hover_handle.visible = false
                    }
                }
            },
            State {
                name: "extend"
                PropertyChanges {
                    target: route
                    onWayStarted: {
                        hover_handle.visible = true
                        hover_handle.runner = drag_handle.runner
                        currentSectionHandle = nets_panel.currentNetHandle
                    }
                    onWayFinished: {
                        for (var i = 0; i < points_model.count; ++i) {
                            var p = points_model.get(i)
                            var segHandle = currentNet.segmentsModel.getHandleByUid(p.segmentUid)
                            console.log("extend segment %1 at net %2".arg(p.segmentUid).arg(currentSectionHandle.mrl))
                            if (p.isEndpoint) {
                                extendSegmentFromEnd(segHandle, p.startId, p.edgePos, 0, 0)
                            } else if (p.isStartpoint) {
                                extendSegmentFromStart(segHandle, p.startId, p.edgePos, 0, 0)
                            } else {
                                insertBranchSegment(segHandle, p.startId, p.sourcePos, 0, 0)
                            }
                            console.log("extend segment %1 at net %2".arg(p.segmentUid).arg(currentSectionHandle.mrl))
                        }
                        currentSectionHandle = undefined
                        hover_handle.visible = false
                        points_model.clear()
                        currentGraph = null
                        editor.state = ""
                    }
                    onDesiredPosChanged: {
                        var reply = route.currentNet.findAtPoint(pos)
                        var runner = drag_handle.runner
                        //                        console.log("runner%1%2 points model count %3".arg(runner.atStart ? " atStart" : "").arg(runner.atEnd ? " atEnd" : "").arg(points_model.count), reply.sourceRunner.atEnd, reply.sourceRunner.atStart)
                        if (points_model.count == 0 && (runner.atStart || runner.atEnd || reply.sourceRunner.atEnd)) {
                            var stIndex
                            if (reply.sourceRunner.atEnd)
                                stIndex = count - 1
                            else
                                stIndex = count
                            points_model.append({
                                                    xpos: reply.posResultRunner.point.x,
                                                    ypos: reply.posResultRunner.point.y,
                                                    angle: reply.posResultRunner.angle,
                                                    sourcePos: reply.sourceRunner.position,
                                                    edgePos: runner.position,
                                                    startId: stIndex,
                                                    isEndpoint: reply.sourceRunner.atEnd,
                                                    isStartpoint: reply.sourceRunner.atStart,
                                                    segmentUid: reply.segment.uid
                                                })
                        }
                    }
                }
            },

            //            State {
            //                name: "extend"
            //                PropertyChanges {
            //                    target: route
            //                    onWayStarted: {
            //                        hover_handle.visible = true
            //                        hover_handle.runner = drag_handle.runner
            //                        currentSectionHandle = nets_panel.currentNetHandle
            //                        atExistingGuide = true
            //                    }
            //                    onWayFinished: {
            //                        console.log("finished extending", points_model.count, count)
            //                        var extType = []
            //                        var newSections = []
            //                        var extGuidePar = {}

            //                        for (var i = 0; i < points_model.count; ++i) {
            //                            var p = points_model.get(i)
            //                            extType.push(p.type)

            //                            if (p.type == "ExtTail" || p.type == "Junction") {
            //                                extGuidePar.leave = {
            //                                    segment: p.segmentUid,
            //                                    sourcePos: p.sourcePos,
            //                                    extIndex: p.startId,
            //                                    extPos: p.edgePos,
            //                                    type: p.type,
            //                                    runner: currentNet.segment(p.segmentUid).source.setRunner(p.sourcePos)
            //                                }
            //                                if (extGuidePar.entry) {
            //                                    newSections.push(extGuidePar)
            //                                    extGuidePar = {}
            //                                }
            //                            } else if (p.type == "Conv" || p.type == "ExtHead") {
            //                                extGuidePar.entry = {
            //                                    segment: p.segmentUid,
            //                                    sourcePos: p.sourcePos,
            //                                    extIndex: p.startId,
            //                                    extPos: p.edgePos,
            //                                    type: p.type,
            //                                    runner: currentNet.segment(p.segmentUid).source.setRunner(p.sourcePos)
            //                                }
            //                                if (extGuidePar.leave) {
            //                                    newSections.push(extGuidePar)
            //                                    extGuidePar = {}
            //                                }
            //                            }
            //                        }
            ////                        if (newSections.length == 0)
            ////                            newSections.push(extGuidePar)
            ////                        console.log("ext type", extType.join(""), JSON.stringify(newSections))

            ////                        var si = m_sceneBuilder.addItem("GuideStroke", {
            ////                                                            smooth: 0,
            ////                                                            precision: 0
            ////                                                        })
            ////                        si.pen.color = "#ff00ff"
            ////                        si.pen.width = 2
            ////                        si.pen.cosmetic = true
            ////                        si.pen.catStyle = Qt.RoundCap

            ////                        var pg, ng, mg
            ////                        newSections.forEach(function(sect) {
            ////                            if (!sect.leave) {
            ////                                // only entry -> convergence or attach head
            ////                                var g = buildGuide(0, sect.entry.extIndex, 0, sect.entry.extPos)
            ////                                si.source = g.guide
            ////                                console.log("enter guide", g.guide, g.guide.length, JSON.stringify(sect.entry))
            ////                                var segSrc = currentNet.segment(sect.entry.segment)
            ////                                var segHandle = currentNet.segmentsModel.getHandleByUid(sect.entry.segment)
            ////                                pg = segSrc.source.part(0, sect.entry.sourcePos)
            ////                                ng = segSrc.source.part(sect.entry.sourcePos, segSrc.source.length)
            ////                                var leftPartMetadata = { guide: pg, offset: segSrc.offset }
            ////                                var rightPartMetadata = { guide: ng, offset: segSrc.offset }
            ////                                var extMetadata = { edges: g.edges, guide: g.guide, offset: segSrc.offset }
            ////                                var stnode = segHandle.startNode
            ////                                var endnode = segHandle.endNode
            ////                                var oldUid = segSrc.uid
            ////                                segHandle.destroy()
            ////                                f_DataNode.parentHandle = currentSectionHandle
            ////                                if (sect.entry.type == "Conv")
            ////                                    f_DataNode.data = { type: "ConvergenceVertex", x: g.guide.tailPoint.x, y: g.guide.tailPoint.y }
            ////                                var cnode = f_DataNode.create("Conv")
            ////                                f_DataNode.data = { type: "StartVertex", x: g.guide.headPoint.x, y: g.guide.headPoint.y }
            ////                                var snode = f_DataNode.create("Start")

            ////                                var s2c = gmodel.addEdge(stnode, cnode, "Catenary|Guide", leftPartMetadata)
            ////                                var c2e = gmodel.addEdge(cnode, endnode, "Catenary|Guide", rightPartMetadata)
            ////                                var s22c = gmodel.addEdge(snode, cnode, "Catenary|Guide", extMetadata)
            ////                                fixBoundaries(currentSectionHandle, oldUid, [c2e], [s22c, s2c])
            ////                            } else if (!sect.entry) {
            ////                                // leave only -> junction or attach tail
            ////                                g = buildGuide(sect.leave.extIndex, count, sect.leave.extPos)
            ////                                si.source = g.guide
            ////                                console.log("leave guide", g.guide, g.guide.length, JSON.stringify(sect.leave))
            ////                                var segmentHandle = currentNet.segmentsModel.getHandleByUid(sect.leave.segment)
            ////                                stnode = segmentHandle.startNode
            ////                                endnode = segmentHandle.endNode
            ////                                oldUid = segmentHandle.uid
            ////                                if (sect.leave.type === "ExtTail") {
            ////                                    var segData = segmentHandle.getData()
            ////                                    var fullPath = segData.guide.clone()
            ////                                    fullPath.appendGuide(g.guide)
            ////                                    segData.guide = fullPath
            ////                                    endnode.data = { type: "EndVertex", x: fullPath.tailPoint.x, y: fullPath.tailPoint.y }
            ////                                    segmentHandle.destroy()
            ////                                    var edge = gmodel.addEdge(stnode, endnode, "Catenary|Guide", segData)
            ////                                    fixBoundaries(currentSectionHandle, oldUid, [edge], [edge])
            ////                                } else if (sect.leave.type === "Junction") {
            ////                                    segSrc = currentNet.segment(sect.leave.segment)
            ////                                    pg = segSrc.source.part(0, sect.leave.sourcePos)
            ////                                    ng = segSrc.source.part(sect.leave.sourcePos, segSrc.source.length)
            ////                                    segmentHandle.destroy()

            ////                                    f_DataNode.parentHandle = currentSectionHandle
            ////                                    f_DataNode.data = { type: "JunctionVertex", x: pg.tailPoint.x, y: pg.tailPoint.y }
            ////                                    var jnode = f_DataNode.create("Junction")
            ////                                    f_DataNode.data = { type: "EndVertex", x: pg.tailPoint.x, y: pg.tailPoint.y }
            ////                                    var enode = f_DataNode.create("End")

            ////                                    leftPartMetadata = { guide: pg, offset: segSrc.offset }
            ////                                    rightPartMetadata = { guide: ng, offset: segSrc.offset }
            ////                                    extMetadata = { edges: g.edges, guide: g.guide, offset: segSrc.offset }
            ////                                    //        console.log("current handle", currentSectionHandle, "start node", stnode, "new end node", enode, "old end node", endnode, "junction node", jnode)
            ////                                    var s2j = gmodel.addEdge(stnode, jnode, "Catenary|Guide", leftPartMetadata)
            ////                                    var j2e1 = gmodel.addEdge(jnode, endnode, "Catenary|Guide", rightPartMetadata)
            ////                                    var j2e2 = gmodel.addEdge(jnode, enode, "Catenary|Guide", extMetadata)
            ////                                    fixBoundaries(currentSectionHandle, oldUid, [j2e1, j2e2], [s2j])
            ////                                }
            ////                            } else {
            ////                                segmentHandle = currentNet.segmentsModel.getHandleByUid(sect.leave.segment)
            ////                                stnode = segmentHandle.startNode
            ////                                endnode = segmentHandle.endNode
            ////                                oldUid = segmentHandle.uid
            ////                                g = buildGuide(sect.leave.extIndex, sect.entry.extIndex, sect.leave.extPos, sect.entry.extPos)
            ////                                var segSrcLeave = currentNet.segment(sect.leave.segment)
            ////                                var segSrcEntry = currentNet.segment(sect.entry.segment)
            ////                                console.log(segSrcLeave.uid, segSrcEntry.uid, segSrcLeave.source.length, sect.leave.sourcePos, sect.entry.sourcePos)
            ////                                pg = segSrcLeave.source.part(0, sect.leave.sourcePos)
            ////                                if (segSrcLeave.uid === segSrcEntry.uid) {
            ////                                    ng = segSrcEntry.source.part(sect.entry.sourcePos, segSrcEntry.source.length)
            ////                                    mg = segSrcEntry.source.part(sect.leave.sourcePos, sect.entry.sourcePos)
            ////                                    f_DataNode.parentHandle = currentSectionHandle
            ////                                    f_DataNode.data = { type: "JunctionVertex", x: pg.tailPoint.x, y: pg.tailPoint.y }
            ////                                    jnode = f_DataNode.create("Junction")
            ////                                    f_DataNode.data = { type: "ConvergenceVertex", x: g.guide.tailPoint.x, y: g.guide.tailPoint.y }
            ////                                    cnode = f_DataNode.create("Conv")
            ////                                    segmentHandle.destroy()
            ////                                    leftPartMetadata = { guide: pg, offset: segSrcLeave.offset }
            ////                                    var middlePartMetadata = { guide: mg, offset: segSrcLeave.offset }
            ////                                    rightPartMetadata = { guide: ng, offset: segSrcEntry.offset }
            ////                                    extMetadata = { edges: g.edges, guide: g.guide, offset: segSrcEntry.offset }
            ////                                    var s2j = gmodel.addEdge(stnode, jnode, "Catenary|Guide", leftPartMetadata)
            ////                                    var j2c = gmodel.addEdge(jnode, cnode, "Catenary|Guide", middlePartMetadata)
            ////                                    var j2ce = gmodel.addEdge(jnode, cnode, "Catenary|Guide|Extra", extMetadata)
            ////                                    var c2e = gmodel.addEdge(jnode, enode, "Catenary|Guide", rightPartMetadata)
            ////                                    fixBoundaries(currentSectionHandle, oldUid, [c2e], [s2j])

            ////                                } else {
            ////                                    console.warn("DIFFERENT SEGMENTS FOR LEAVE & ENTRY NOT SUPPORTED YET")
            ////                                }

            ////                                si.source = g.guide
            ////                                console.log("inout guide", g.guide, g.guide.length, JSON.stringify(sect))

            ////                            }
            ////                        })
            ////                        clear()
            ////                        edgeUid2modelIdx = []
            ////                        currentEdgeUid = -1
            ////                        candidateEdges = []

            //                        var g = buildGuide()
            //                        if (p.isEndpoint) {
            //                            extendSegmentFromEnd(segHandle, p.startId, p.edgePos, 0, 0)
            //                        } else if (p.isStartpoint) {
            //                            extendSegmentFromStart(segHandle, p.startId, p.edgePos, 0, 0)
            //                        } else {
            //                            insertBranchSegment(segHandle, p.startId, p.sourcePos, 0, 0)
            //                        }
            //                        currentSectionHandle = undefined
            //                        hover_handle.visible = false
            //                        points_model.clear()
            //                        currentGraph = null
            //                        editor.state = ""
            //                    }
            //                    onDesiredPosChanged: {
            //                        var runner = drag_handle.runner
            //                        var reply = route.currentNet.findAtPoint(pos)
            //                        if (atExistingGuide) {
            //                            if(runner.atStart || runner.atEnd || reply.sourceRunner.atEnd) {
            //                                var type
            //                                if (reply.sourceRunner.atEnd) {
            //                                    type = "ExtTail"
            //                                } else {
            //                                    type = "Junction"
            //                                }
            //                                atExistingGuide = false
            //                                points_model.append(
            //                                            {
            //                                                xpos: reply.posResultRunner.point.x,
            //                                                ypos: reply.posResultRunner.point.y,
            //                                                angle: reply.posResultRunner.angle,
            //                                                sourcePos: reply.sourceRunner.position,
            //                                                edgePos: runner.position,
            //                                                startId: count - 1,
            //                                                isEndpoint: reply.sourceRunner.atEnd,
            //                                                isStartpoint: reply.sourceRunner.atStart,
            //                                                segmentUid: reply.segment.uid,
            //                                                type: type
            //                                            })
            //                            }
            //                        } else {
            //                            var dist = route.distance(reply.sourceRunner.point, runner.point)
            //                            var k = dist /** Math.abs(runner.angle - reply.sourceRunner.angle)*/
            //                            console.log(k, Math.abs(Math.min(runner.vectorLengthAhead, runner.vectorLengthBehind) - Math.min(reply.sourceRunner.vectorLengthAhead, reply.sourceRunner.vectorLengthBehind)))
            //                            if (k < 1E-3) {
            //                                console.log("Re-entry?", k,
            //                                            reply.sourceRunner.position - Math.min(reply.sourceRunner.vectorLengthAhead, reply.sourceRunner.vectorLengthBehind),
            //                                            "count", count,
            //                                            runner.atVertex)
            //                                // Re-entry? 2.2198471228713367e-8 23.946172853191747 5.329131402075291 0 1.0970207452774048 true
            //                                type = "Conv"
            //                                atExistingGuide = true
            //                                points_model.append({
            //                                                        xpos: reply.posResultRunner.point.x,
            //                                                        ypos: reply.posResultRunner.point.y,
            //                                                        angle: reply.posResultRunner.angle,
            //                                                        sourcePos: reply.sourceRunner.position - Math.min(reply.sourceRunner.vectorLengthAhead, reply.sourceRunner.vectorLengthBehind),
            //                                                        edgePos: runner.position,
            //                                                        startId: count,
            //                                                        isEndpoint: reply.sourceRunner.atEnd,
            //                                                        isStartpoint: reply.sourceRunner.atStart,
            //                                                        segmentUid: reply.segment.uid,
            //                                                        type: type
            //                                                    })
            //                            }


            //                        }
            //                    }
            //                }
            //            },
            State {
                name: "extendTroll"
                extend: "extend"
                PropertyChanges {
                    target: roadGraph
                    interactive: true

                }
                PropertyChanges {
                    target: route
                    currentGraph: roadGraph
                    currentSectionType: "TrolleySection"
                }

            },
            State {
                name: "extendTram"
                extend: "extend"
                PropertyChanges {
                    target: tramGraph
                    interactive: true
                }
                PropertyChanges {
                    target: route
                    currentSectionType: "TramSection"
                    currentGraph: tramGraph
                }
            },
            State {
                name: "append"
                PropertyChanges {
                    target: route
                    onWayStarted: {
                        hover_handle.visible = true
                        hover_handle.runner = drag_handle.runner
                        currentSectionHandle = nets_panel.currentNetHandle
                    }
                    onWayFinished: {
                        for (var i = 0; i < points_model.count; ++i) {
                            var p = points_model.get(i)
                            var segHandle = currentNet.segmentsModel.getHandleByUid(p.segmentUid)
                            appendSection(segHandle, p.startId, p.edgePos, 0, 0)
                        }
                        currentSectionHandle = undefined
                        hover_handle.visible = false
                        points_model.clear()
                        currentGraph = null
                        editor.state = ""
                    }
                    onDesiredPosChanged: {
                        var reply = route.currentNet.findAtPoint(pos)
                        var runner = drag_handle.runner
                        //                        console.log("runner%1%2 points model count %3".arg(runner.atStart ? " atStart" : "").arg(runner.atEnd ? " atEnd" : "").arg(points_model.count), reply.sourceRunner.atEnd, reply.sourceRunner.atStart)
                        if (points_model.count == 0 && reply.sourceRunner.atEnd) {
                            var stIndex = count - 1
                            points_model.append({
                                                    xpos: reply.posResultRunner.point.x,
                                                    ypos: reply.posResultRunner.point.y,
                                                    angle: reply.posResultRunner.angle,
                                                    sourcePos: reply.sourceRunner.position,
                                                    edgePos: runner.position,
                                                    startId: stIndex,
                                                    isEndpoint: reply.sourceRunner.atEnd,
                                                    isStartpoint: reply.sourceRunner.atStart,
                                                    segmentUid: reply.segment.uid
                                                })
                        }
                    }
                }
            },
            State {
                name: "appendTroll"
                extend: "append"
                PropertyChanges {
                    target: roadGraph
                    interactive: true
                }

                PropertyChanges {
                    target: route
                    currentSectionType: "TrolleySection"
                    currentGraph: roadGraph
                }

            },
            State {
                name: "appendTram"
                extend: "append"
                PropertyChanges {
                    target: tramGraph
                    interactive: true
                }
                PropertyChanges {
                    target: route
                    currentSectionType: "TramSection"
                    currentGraph: tramGraph
                }
            }
        ]
    }


    Action {
        id: a_openModel
        text: "Открыть..."
        shortcut: "Ctrl+O"
        onTriggered: {
            fileselector.open()
        }
    }
    Action {
        id: a_saveAs
        text: "Сохранить как..."
        shortcut: "Ctrl+Shift+S"
        onTriggered: {
            saveFileselector.open()
        }
    }
    Action {
        id: a_saveNetwork
        text: "Экспорт сети..."
        onTriggered: {
            saveFileselector.open()
        }
    }
    Action {
        id: a_reloadModel
        text: "Перезагрузить модель"
        shortcut: "Ctrl+Shift+R"
        enabled: gmodel.file.toString().length > 0
        onTriggered: {
            gmodel.loadModel(gmodel.file)
        }
    }

    Action {
        id: a_save
        text: "Сохранить %1".arg(gmodel.file)
        enabled: gmodel.file.toString().length > 0
        shortcut: "Ctrl+S"
        onTriggered: {
            //            gmodel.sceneData.contentX = drawarea.scrollArea.contentX
            //            gmodel.sceneData.contentY = drawarea.scrollArea.contentY
            //            gmodel.sceneData.zoom = drawarea.scene.scale
            gmodel.exportModel(gmodel.file, gmodel.root)
        }
    }


    FileDialog {
        id: fileselector
        selectExisting: true
        modality: Qt.NonModal
        folder: shortcuts.desktop
        nameFilters: ["MetaGraph model files (*.mgf)"]
        onAccepted: {
            gmodel.file = fileUrl
            gmodel.loadModel(fileUrl)
        }
    }

    FileDialog {
        id: saveFileselector
        folder: shortcuts.desktop
        selectExisting: false
        nameFilters: ["MetaGraph model files (*.mgf)"]
        onAccepted: {
            var st = fileUrl.toString()
            var p = st.toString().lastIndexOf(".mgf")
            if (p < st.length - 4)
                fileUrl = Qt.resolvedUrl(st + ".mgf")
            gmodel.file = fileUrl
            gmodel.exportModel(fileUrl, gmodel.rootIndex)
        }
    }


    Creatist {
        id: creatist
    }


    SLDBMaker {
        id: i_ds
        model: model_ds
    }

    GeoLayerMaker {
        id: i_layers
        model: model_layers
        active: i_ds.ready && i_ds.pendingSize === 0 && spview.linked
    }

    StyleMaker {
        id: i_styles
        index: styles_model.rootIndex
        typeRole: "blob$type"
        valueRole: "blob$metadata"
    }



    Action {
        id: netHider
        shortcut: 'R'
        property int i: 0
        property int counter: 0
        onTriggered: {
            console.log( 'HIDE' )
            //            m_scene.renderer.grabToImage(function(result) {
            //                result.saveToFile("scene_%1.png".arg(i));
            //            })
            //            textLayer.grabToImage(function(result) {
            //                result.saveToFile("text_%1.png".arg(i));
            //            })
            //            gview.item.grabToImage(function(result) {
            //                result.saveToFile("geo_%1.png".arg(i));
            //            })
            //            i++
            if( i == 0 ) {
                i_sections.vis = true
                i_bounds.vis = false
                gview.item.visible =  false
                textLayer.visible = false
                showVehicles.checked = false
                drawarea.grabToImage(function(result) {
                    result.saveToFile("img%1%2.png".arg(i).arg(counter));
                })
                i++
            }else if( i == 1 ) {
                i_sections.vis = false
                i_bounds.vis = true
                gview.item.visible =  false
                textLayer.visible = false
                showVehicles.checked = false
                drawarea.grabToImage(function(result) {
                    result.saveToFile("img%1%2.png".arg(i).arg(counter));
                })
                i++
            }else if( i == 2 ) {
                i_sections.vis = false
                i_bounds.vis = false
                gview.item.visible =  true
                textLayer.visible = false
                showVehicles.checked = false
                drawarea.grabToImage(function(result) {
                    result.saveToFile("img%1%2.png".arg(i).arg(counter));
                })
                i++
            }
            else if( i == 3 ) {
                i_sections.vis = false
                i_bounds.vis = false
                gview.item.visible = false
                textLayer.visible = true
                showVehicles.checked = false
                drawarea.grabToImage(function(result) {
                    result.saveToFile("img%1%2.png".arg(i).arg(counter));
                })
                i++
            }
            else if( i == 4 ) {
                i_sections.vis = false
                i_bounds.vis = false
                gview.item.visible = false
                textLayer.visible = false
                showVehicles.checked = true
                drawarea.grabToImage(function(result) {
                    result.saveToFile("img%1%2.png".arg(i).arg(counter));
                })
                i=0
                counter++
            }

            //            for( var i = 0; i < i_sections.count; ++i ) {
            //                var model = i_sections.objectAt( i ).nodesModel

            //                console.log( 'PORTS', model.connectionPorts, model.connectionPorts.length)
            //                for( var j = 0 ; j < model.connectionPorts.length; ++j) {

            ////                    model.connectionPorts[j].sign.visible = i_bounds.vis
            //////                    h.dataHandle.negSign.visible = i_bounds.vis
            //////                    h.dataHandle.posSign.visible = i_bounds.vis
            //                }
            //            }
        }
    }

}
