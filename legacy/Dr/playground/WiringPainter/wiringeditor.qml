import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import Dr.Graphics 1.0
import Dr.UI 1.0
import QtGraphicalEffects 1.0
import Dr.Cartograph 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.2
import Dr.MetaGraph 1.0
import QtQuick.Window 2.2

ApplicationWindow {
    id: mainWindow
    width: 1920
    height: 1000
    x: 0
    y: 0
    title: "Qt Wiring Editor"
    //    samples: 16
    //    screenId: 1
    visible: true

    MenuBar {
        Menu {
            title: "Файл"
            MenuItem { action: a_openModel }
            MenuItem { action: a_saveAs }
            MenuItem { action: a_save }
            MenuItem { action: a_reloadModel }
        }
    }

    Component.onCompleted: console.log()
    Rectangle {
        id: l_1
        color: "#222"
        anchors.fill: parent

        SceneModel {
            id: m_sceneBuilder
            scene: m_scene.qGraphicsScene
            Component.onCompleted: {
                roleBindings.polygon = "edgeData"
            }
        }

        DrawArea {
            id: drawarea
            backgroundColor: "transparent"
            anchors.fill: parent
            scene.width: m_scene.sceneRect.width
            scene.height: m_scene.sceneRect.height
            maxScale: 512
            property var dat: []
            property rect sourceRect: Qt.rect(scrollArea.contentX / scene.scale, scrollArea.contentY / scene.scale, width / scene.scale, height / scene.scale)
            grid.visible: false

            MouseArea {
                id: scene_mouse
                acceptedButtons: Qt.AllButtons
                anchors.fill: parent
            }
            Repeater{
                model: polmodel
                delegate: HandleItem {
                    //                    id: handle
                    onXChanged: polmodel.setProperty(index, "xp", x)
                    onYChanged: polmodel.setProperty(index, "yp", y)
                    Component.onCompleted: {
                        x = xp// modelData.x
                        y = yp// modelData.y
                    }
                }
            }



            //            Scene {
            //                id: m_scene2
            //                viewport: m_scene.viewport
            //                sceneRect: m_scene.sceneRect
            //                width: m_scene.width
            //                height: m_scene.height
            //            }

            Scene {
                id: m_scene
                enableBsp: true
                viewport: drawarea
                width: sceneRect.width
                height: sceneRect.height
                sceneRect: Qt.rect(0, 0, 16000, 16000)
                //                renderer.renderTarget: SceneRenderer.Image
                renderer.renderTarget: SceneRenderer.FramebufferObject
                renderer.z: 1000

                //                Repeater {
                //                    model: gr_nodes
                //                    delegate: Rectangle {
                //                        id: rec
                //                        x: handle.point.x
                //                        y: handle.point.y
                //                        width: 10
                //                        height: 10
                //                        radius: 5
                //                        border.width: 1
                //                        border.color: "#ff0000"
                //                        color: "transparent"
                //                        transform: Translate {
                //                            x: -rec.width / 2
                //                            y: -rec.height / 2
                //                        }
                //                        scale: 1 / drawarea.scene.scale
                //                    }

                //                }

                MarkerItem {
                    id: hover_handle
                    property var posPath
                    property var negPath
                    Item {
                        id: container
                        width: 1
                        height: 1
                        anchors.centerIn: hover_handle
                        rotation: -hover_handle.rotation
                        Rectangle {
                            id: negVec
                            width: 10
                            height: 10
                            color: "#80ffff"
                            border.width: 2
                            border.color: "#0080ff"
                            transform: Translate {
                                x: -3
                                y: -3
                            }
                        }
                        Rectangle {
                            id: posVec
                            width: 10
                            height: 10
                            color: "#ffff80"
                            border.width: 2
                            border.color: "#800000"

                            transform: Translate {
                                x: -3
                                y: -3
                            }
                        }
                        Rectangle {
                            id: negPt
                            width: 6
                            height: 6
                            color: "#0080ff"
                            transform: Translate {
                                x: -3
                                y: -3
                            }
                        }
                        Rectangle {
                            id: posPt
                            width: 6
                            height: 6
                            color: "#ff8080"
                            transform: Translate {
                                x: -3
                                y: -3
                            }
                        }

                    }

                    Component.onCompleted: {
                        posPath = m_sceneBuilder.addItem("GuidePathStroke", {z: 2})
                        negPath = m_sceneBuilder.addItem("GuidePathStroke", {z: 2})
                        posPath.pen.color = "#ffab40"
                        posPath.pen.cosmetic = true
                        posPath.pen.width = 2
                        negPath.pen.color = "#40abff"
                        negPath.pen.cosmetic = true
                        negPath.pen.width = 2
                    }

                    onVisibleChanged: {
                        negPath.visible = visible
                        posPath.visible = visible
                    }

                    onRunnerChanged: {
                        var smoothData = runner.debugRounding(m_sidebar.curveSmoothing)

                        negPath.source = smoothData.negPath
                        posPath.source = smoothData.posPath
                        negPath.update()
                        posPath.update()
                        negVec.x = smoothData.negVec.x  *  20
                        negVec.y = smoothData.negVec.y  *  20
                        posVec.x = smoothData.posVec.x  *  20
                        posVec.y = smoothData.posVec.y  *  20
                        negPt.x = smoothData.negPt.x    *  20
                        negPt.y = smoothData.negPt.y    *  20
                        posPt.x = smoothData.posPt.x    *  20
                        posPt.y = smoothData.posPt.y    *  20

//                        smoothDelta.x = smoothData.negPt
                    }
                }

                MarkerItem {
                    id: drag_handle
                    border.color: "#88ffff"
                }

                property var segs: []

                //                                    ShaderEffectSource {
                //                                        id: src_raster
                //                                        anchors.fill: m_scene.renderer
                //                                        sourceItem: m_scene.renderer
                //                                        scale: m_scene.renderer.scale
                //                                        transformOrigin: Item.TopLeft
                //                                        visible: false
                //                                        z: 1000
                //                                    }
                //                                    Glow{
                //                                        id: raster_glow
                //                                        source: src_raster
                //                                        anchors.fill: source
                //                                        transformOrigin: Item.TopLeft
                //                                        scale: source.scale
                //                                        samples: 16
                //                                        radius: 1
                //                                        spread: 1
                //                                        color: "#000000"
                //                                        z: 1000
                //                                    }
            }



        }
        SceneItemStroker {
            id: m_selectionOutline
            anchors.fill: drawarea
            scene: m_scene
        }
        ShaderEffectSource {
            id: m_selectionOutlineSrc
            sourceItem: m_selectionOutline
            anchors.fill: drawarea
            hideSource: true
        }

        SelectShaderEffect {
            anchors.fill: drawarea
            source: m_selectionOutlineSrc
        }
    }

    Item {
        id: l_2
        width: 240
        z: 100
        anchors.right: parent.right
        height: parent.height

        Rectangle {
            anchors.fill: parent
            opacity: 0.9
        }

        ColumnLayout {
            id: m_sidebar
            property real curveSmoothing: 30
            property real curvePrecision: 1
            property real baseCurveOffset: 0
            property bool fastMode: false
            anchors.fill: parent
            anchors.margins: 8
            CaptionLabel {
                level: 3
                Layout.fillWidth: true
                text: "Guide path properties"
            }

            JSObjectView {
                id: paths_props
                Layout.fillWidth: true
                JSObjectViewRow {
                    text: "Smoothing"
                    valueDelegate: TinySlider {
                        minimumValue: 0
                        maximumValue: 150
                        value: m_sidebar.curveSmoothing
                        onValueChanged:  if (pressed)
                                             m_sidebar.curveSmoothing = value

                    }
                }
                JSObjectViewRow {
                    text: "Precision"
                    valueDelegate: TinySlider {
                        minimumValue: 0
                        maximumValue: 8
                        value: m_sidebar.curvePrecision
                        onValueChanged:  if (pressed) m_sidebar.curvePrecision = value

                    }
                }
                JSObjectViewRow {
                    text: "Base offset"
                    valueDelegate: TinySlider {
                        minimumValue: 0
                        maximumValue: 20
                        stepSize: 0.5
                        value: m_sidebar.baseCurveOffset
                        onValueChanged:  if (pressed) m_sidebar.baseCurveOffset = value

                    }
                }
                JSObjectViewRow {
                    text: "Fast mode"
                    valueDelegate: CheckBox {
                        checked: m_sidebar.fastMode
                        onClicked: m_sidebar.fastMode = checked
                        text: "Don't detect cusps"
                    }
                }
                JSObjectViewRow {
                    text: "Follow scale"
                    valueDelegate: CheckBox {
                        checked: m_scene.notifyViewTransformUpdates
                        onClicked: m_scene.notifyViewTransformUpdates = checked
                    }
                }

            }


            property var handle
            onHandleChanged: console.log("handle item", handle)

            Creatist {
                id: creatist
            }

            //            Button {
            //                text: 'pathtester2'
            //                onClicked: {
            //                    var p = creatist.handle('PainterPath')
            //                    console.log( 'GP', p )
            //                    p.deserialize(icons.handle.data)
            //                    p.addText( 20, 20, 'Roboto','ને બદલે હિંદુ અને આર્યાવ્રતને બદલે' )
            //                    var i = m_sceneBuilder.addItem( "PathItem", {
            //                                                       flags: VisualInstance.ItemIsSelectable |
            //                                                              VisualInstance.ItemIsMovable |
            //                                                              VisualInstance.ItemSendsGeometryChanges,
            //                                                       path: p
            //                                                   })
            //                    i.pen.color = '#4f8'
            //                    i.pen.width = 0.1
            //                    i.brush.style = Qt.SolidPattern
            //                    i.brush.color = '#f48'
            //                    //                    i.path = p
            //                }
            //            }
            //            Button {
            //                text: 'pathtest'
            //                onClicked: {
            //                    //                    for( var j = 0; j < 1300; ++j ){
            //                    //                        var i = m_sceneBuilder.addItem( "PathItem", { flags: 3, x: Math.random()*1600, y: Math.random()*1600, rotation: Math.random()*360, scale: Math.random()*2 } )
            //                    var i = m_sceneBuilder.addItem( "PathItem", { flags: VisualInstance.ItemIsSelectable |
            //                                                                         VisualInstance.ItemIsMovable |
            //                                                                         VisualInstance.ItemSendsGeometryChanges
            //                                                   } )
            //                    console.log( i )
            //                    var idx = ~~( Math.random()*200 )
            //                    //                        i.setBlob( icons.getChild( idx ).data )
            //                    i.setBlob( icons.handle.data )
            //                    i.pen.color = '#4f8'
            //                    i.pen.width = 0.5
            //                    i.brush.style = Qt.SolidPattern
            //                    i.brush.color = '#9f8'
            //                    //                    }
            //                }
            //                GraphModel {
            //                    id: graph
            //                    NodeFactory { }
            //                    BlobFactory { }
            //                    RootNodeFactory { }
            //                    DataNodeFactory { }
            //                    Component.onCompleted: init()
            //                    function init() {
            //                        clear()
            //                        var ui = importFile("/Users/Tox/Desktop/icons.mgf", null)
            //                    }
            //                    ModelWatcher {
            //                        id: icons
            //                        //                        mrl: "/font2pic/icons/iconPaths"
            //                        mrl: "/font2pic/icons/iconPaths/uni25FC"
            //                    }
            //                }
            //            }

            GroupBox {
                title: "Way"
                Layout.fillWidth: true
                TableView {
                    Layout.fillWidth: true
                    model: ways_model

                    TableViewColumn {
                        role: "handle"

                        delegate: RowLayout {
                            id: _row
                            property QtObject agent: ways_model.get(styleData.row)
                            TextField {
                                text: styleData.value.metadata.color
                                onAccepted: {
                                    var md = _row.agent.blob$metadata
                                    md.color = text
                                    _row.agent.blob$metadata = md
                                    focus = false
                                }
                            }
                            Label {
                                text: _row.agent.nodeId
                            }
                        }
                    }
                    //                    TableViewColumn {
                    //                        role: "yp"
                    //                    }
                }

            }


            GroupBox {
                title: "Build polygon"
                Layout.fillWidth: true
                ColumnLayout {
                    Layout.fillWidth: true
                    RowLayout {
                        Layout.fillWidth: true
                        Switch {
                            id: drawpol
                        }
                        Button {
                            text: "Add"
                            onClicked: {
                                var phandle = m_sceneBuilder.addItem("PolygonItem", { flags:
                                                                         VisualInstance.ItemIsSelectable |
                                                                         VisualInstance.ItemIsMovable |
                                                                         VisualInstance.ItemSendsGeometryChanges
                                                                     })
                                //                                phandle.polygon = smoother.output
                                phandle.pen.color = smoother.currentItem.pen.color
                                phandle.pen.cosmetic = true
                                phandle.pen.width = 2
                                phandle.shapeType = 1
                                phandle.hoverEnabled = true
                                phandle.onMousePress = onClick
                                phandle.onHoverEnter = onHoverEnter
                                phandle.onHoverLeave = onHoverLeave
                                phandle.bind(VisualInstance.ItemPositionChange, itemPosChange)
                                polmodel.clear()
                                polfactory.buildPolygon()

                                //                                drawpol.checked = false
                            }
                            function onClick(event) {
                                console.log("polygon pressed", this)
                            }
                            property var oldColor
                            onOldColorChanged: console.log("old color", oldColor)
                            function onHoverEnter(event) {
                                //                                console.log("hover enter", this)
                                oldColor = this.pen.color
                                this.pen.color = "#FFFFFF"
                                this.update()
                            }
                            function onHoverLeave(event) {
                                //                                console.log("hover leave", this)
                                //                                oldColor = this.pen.color
                                this.pen.color = oldColor
                                this.update()
                            }
                            function itemPosChange(value) {
                                value.x = Math.round(value.x / 8) * 8
                                value.y = Math.round(value.y / 8) * 8
                                return value.point
                            }
                        }
                    }
                    TableView {
                        Layout.fillWidth: true
                        model: polmodel
                        TableViewColumn {
                            role: "xp"
                        }
                        TableViewColumn {
                            role: "yp"
                        }
                    }
                }
            }

            TableView {
                id: t_route
                Layout.fillWidth: true
                Layout.minimumHeight: 240
                model: route
                TableViewColumn {
                    title: "Edge"
                    role: "edge"
                    width: 60
                    delegate: Label {
                        color: styleData.textColor
                        text: styleData.value
                    }
                }
                TableViewColumn {
                    title: "Begin"
                    role: "startPos"
                    width: 50
                    delegate: Label {
                        color: styleData.textColor
                        text: Number(styleData.value).toFixed(2)
                    }
                }
                TableViewColumn {
                    title: "End"
                    role: "endPos"
                    width: 50
                    delegate: Label {
                        color: styleData.textColor
                        text: Number(styleData.value).toFixed(2)
                    }
                }

            }

            states: [
                State {
                    name: "createpoly"
                    when: drawpol.checked
                    PropertyChanges {
                        target: scene_mouse
                        onClicked: {
                            polmodel.append({ xp: mouse.x, yp: mouse.y })
                            polfactory.buildPolygon()
                        }
                    }
                }
            ]
            ListModel {
                id: polmodel
            }

            QtObject {
                id: smoother
                //                onInputChanged: console.log("input updated")
                property var currentItem
                property var currentDebugItem
                property alias smooth: m_sidebar.curveSmoothing
                property alias precision: m_sidebar.curvePrecision
                property alias offset: m_sidebar.baseCurveOffset
                property alias fast: m_sidebar.fastMode
                property var polygon: polfactory.polygon

//                onPolygonChanged: { // simplified
//                    if (!m_sceneBuilder.scene)
//                        return
//                    if (!currentItem) {
//                        currentItem = m_sceneBuilder.addItem("GuidePathStroke",
//                                                             {
//                                                                 smooth: 0,
//                                                                 offset: 0,
//                                                                 precision: 0
//                                                             })
//                        currentItem.pen.cosmetic = true
//                        currentItem.pen.color = Qt.hsla(Math.random(), 0.8, 0.8)
//                        currentItem.pen.width = 3
//                        currentItem.pen.cosmetic = true

//                        currentItem.sourcePen.width = 1
//                        currentItem.sourcePen.color = "#00ff80"
//                        currentItem.sourcePen.cosmetic = true
//                        currentItem.drawSource = true

//                        currentItem.setDebugMode(true)

//                    }
//                    var g = currentItem.source
//                    g.setPolygon(polygon)
//                    currentItem.source = g.simplified(precision * 10, smooth)
//                }
                onPolygonChanged: {
                    if (!m_sceneBuilder.scene)
                        return
                    if (!currentItem) {
                        currentItem = m_sceneBuilder.addItem("GuidePathStroke",
                                                             {
                                                                 smooth: smooth,
                                                                 offset: offset,
                                                                 precision: precision,
                                                                 fast: fast
//                                                                 type: GuidePAth
                                                             })


                        currentItem.pen.cosmetic = true
                        currentItem.pen.color = Qt.hsla(Math.random(), 0.8, 0.8)
                        currentItem.pen.width = 3
                        currentItem.pen.cosmetic = true

                        currentItem.sourcePen.width = 1
                        currentItem.sourcePen.color = "#00ff80"
                        currentItem.sourcePen.cosmetic = true
                        currentItem.drawSource = true

                        currentItem.setDebugMode(true)

                    }
                    if (!currentDebugItem) {
                        currentDebugItem = m_sceneBuilder.addItem("PathItem")
                        currentDebugItem.pen.color = currentItem.pen.color
                        currentDebugItem.pen.width = 0
                    }

                    var g = currentItem.source
                    g.setPolygon(polygon)
                    currentDebugItem.path = g.wavefront_debug(offset)
                    currentItem.source = g
                }

                onSmoothChanged: updateCurrentItem()
                onOffsetChanged: updateCurrentItem()
                onPrecisionChanged: updateCurrentItem()
                onFastChanged: updateCurrentItem()

                function updateCurrentItem() {
                    currentItem.smooth = smooth
                    currentItem.precision = precision
                    currentItem.offset = offset
                    currentItem.fast = fast
                    var g = currentItem.source
                    currentDebugItem.path = g.wavefront_debug(offset)
                }

//                function updateCurrentItem() { // simplified
//                    var g = currentItem.source
//                    g.setPolygon(polfactory.polygon)
//                    currentItem.source = g.simplified(precision * 10, smooth)
//                }


            }

            PolygonFactory {
                id: polfactory
                model: polmodel
                xRole: "xp"
                yRole: "yp"
                property var currentItem
                onPolygonChanged:  {
                    if (!currentItem) {
                        currentItem = m_sceneBuilder.addItem("PolygonItem", { polygon: polygon })
                        currentItem.pen.cosmetic = true
                        currentItem.pen.width = 1
                        currentItem.shapeType = 1
                        currentItem.pen.color = "#80ffffff"
                        currentItem.pen.strokeStyle = Qt.DashLine


                    }
                    currentItem.polygon = polygon
                }
            }


            Button {
                text: "Add polygon"
                property var phandle
                onClicked: {
                    polfactory.buildPolygon()
                    var col = Qt.hsla(Math.random(), 1, 0.7)
                    var px = Math.random() * m_scene.width
                    var py = Math.random() * m_scene.height
                    phandle = m_sceneBuilder.addItem("PolygonItem", {
                                                         flags: 3,
                                                         x: px,
                                                         y: py,
                                                         rotation: Math.random() * 360,
                                                         scale: Math.random() * 2,
                                                         onMousePress: function(event) {
                                                             console.log("pressed", this, JSON.stringify(event))
                                                         }
                                                     })
                    console.log(phandle)
                    phandle.polygon = polfactory.polygon
                    phandle.pen.color = col.toString()
                    phandle.pen.cosmetic = true
                }
            }

            Button {
                text: 'load Model'
                onClicked: importDialog.run(openModel)
                function openModel(fileUrl) {
                    gmodel.fileName = fileUrl
                    gmodel.loadModel(fileUrl)
                }
            }
            FileDialog {
                id: importDialog
                //            id: fileselector
                modality: Qt.NonModal
                property var argument
                property var callback
                selectExisting: true
                nameFilters: ["MetaGraph model files (*.mgf)"]

                function runMultiple(callbackFn, arg) {
                    selectMultiple = true
                    callback = callbackFn
                    argument = arg
                    open()
                }
                function run(callbackFn, arg) {
                    callback = callbackFn
                    argument = arg
                    open()
                }
                onAccepted: {
                    if (selectMultiple) {
                        callback(fileUrls, argument)
                        selectMultiple = false
                    } else {
                        callback(fileUrl, argument)
                    }
                }
            }
            Item {
                Layout.fillHeight: true
            }
        }
    }

    ListModel {
        id: model_wires
        function addWire(route, wireOffset, hue, sat, lum, tools) {
            if (!wireOffset)
                wireOffset = 1
            if (!sat)
                sat = 1.0
            if (!lum)
                lum = 0.5
            if (!tools)
                tools = {  begin: 0, end: 0 }
            append({ nodes: route, hue: hue, sat: sat, lum: lum, wireOffset: wireOffset, tools: tools })
        }

    }



    GraphModel {
        id: gmodel
        property url file
        property string fileName: file.toString().length ? file.toString().match(/(\w+)\..+$/)[1] : "untitled"
        RootNodeFactory { id: f_RootNode }
        NodeFactory { id: f_Node }
        DataNodeFactory { id: f_DataNode }
        MetaNodeFactory { id: f_MetaNode }
        BlobFactory { id: f_Blob }
        PointNodeFactory { id: f_PointNode }
        //        onLoaded: console.log("loaded model")
        NodeChildrenModel {
            id: gr_nodes
            mrl: "/spview/road_graph"
            onLinkedChanged: console.log("graph contains", count, "nodes")

        }

        NodeChildrenModel {
            id: ways_model
            mrl: "/ways"
            property alias smooth: m_sidebar.curveSmoothing
            property alias precision: m_sidebar.curvePrecision
            property alias offset: m_sidebar.baseCurveOffset
            property alias fast: m_sidebar.fastMode
            onSmoothChanged: updateWaysCommonProps()
            onOffsetChanged: updateWaysCommonProps()
            onPrecisionChanged: updateWaysCommonProps()
            onFastChanged: updateWaysCommonProps()
            property var idx2inst: []
            onLinkedChanged: createWays()
            function createWays() {
                if (!linked) {
                    idx2inst.forEach(function(e) { e.destroy() } )
                    idx2inst = []
                    return
                }

                for (var i = 0; i < count; ++i) {
                    buildWay(i)
                }
            }

            function buildWay(index) {
                var h = ways_model.getHandle(index)
                var i = m_sceneBuilder.addItem("GuidePathStroke", {
                                                   smooth: smooth,
                                                   offset: offset,
                                                   precision: precision,
                                                   fast: fast
                                                   //                                                   flags: VisualInstance.ItemIgnoresTransformations
                                               })
                i.pen.width = 2
                i.pen.color = h.metadata.color
                i.pen.cosmetic = true

//                i.sourcePen.width = 1
//                i.sourcePen.color = "#00ff80"
//                i.sourcePen.cosmetic = true
//                i.drawSource = true

                i.setDebugMode(true)
                var guide = creatist.deserialize("GuidePath", h.data)
                i.source  = guide
                idx2inst[index] = i
            }
            function updateWaysCommonProps() {
                for (var i = 0; i < count; ++i) {
                    var h = ways_model.getHandle(i)
                    var inst = idx2inst[i]
                    //                    console.log(inst)
                    inst.smooth = smooth
                    inst.precision = precision
                    inst.offset = offset
                    inst.fast = fast
                }

            }

            function updateWay(index) {
                if (!idx2inst[index])
                    buildWay(index)
                var h = ways_model.getHandle(index)
                var i = idx2inst[index]
                i.pen.color = h.metadata.color
                var guide = creatist.deserialize("GuidePath", h.data)
                i.source  = guide
                //                console.log("updated way", h, i, i.source, h.type, guide, h.size, h.type)

            }

            onRowsInserted: {
                buildWay(first)
                //                console.log("inserted way", getHandle(first).childCount)
            }
            onDataChanged: {
                var h = gmodel.getHandle(topLeft)
                //                console.log("data change at row", topLeft.row, getHandle(topLeft.row))
                updateWay(topLeft.row)
            }
        }
    }

    EdgeModel {
        id: links_model
        tag: "GuideLink"
        model: gmodel
        property var instances: []
        //        onItemAdded: {
        //            var e = getHandle(index)
        //            var stnode = e.startNode/*
        //            var p = creatist.create("GuidePath")
        //            var polygon = edgeModel.getHandle(stnode.metadata.edgeIndex).getData().polygon
        //            p.setPolygon(polygon)
        //            var guide = p.part(stnode.metadata.startPos, stnode.metadata.endPos)*/
        ////            var guide = p.part(stnode.metadata.startPos, stnode.metadata.endPos)
        //            var guide = creatist.deserialize("GuidePath", stnode.data)
        //            var i = m_sceneBuilder.addItem("GuidePathStroke", {
        //                                               smooth: 0,
        //                                               precision: 0
        //                                           })
        //            var r = guide.setRunner(0)

        //            console.log("stnode", JSON.stringify(stnode.metadata))
        //            i.pen.color = stnode.metadata.color
        //            i.pen.width = 2
        //            i.pen.cosmetic = true
        //            i.source = guide



        ////            f_Blob.metadata = {
        ////                edgeIndex: s.edge,
        ////                startPos: s.startPos,
        ////                endPos: s.endPos,
        ////                offset: 0,
        ////                wires: 1,
        ////                color: item.pen.color,
        ////                radius: 1,
        ////                precision: 2
        ////            }
        //            console.log("Added segment link", getHandle(index))
        //        }
    }


    EdgeModel {
        id: edgeModel
        model: gmodel
        tag: 'polygon'
        property var edgeUid2item: []

        onItemAdded: {
            var eh = getHandleByUid(uid)
            var p = creatist.create("GuidePath")
            var ed = eh.getData()
            p.setPolygon(ed.polygon)
            p.setHeadData(uid)
            p.setTailData(uid)
            p.setPathData(ed.attributes)
            var i = m_sceneBuilder.addItem("GuidePathStroke",
                                           {
                                               smooth: 0,
                                               precision: 0,
                                               source: p,
                                               flags: VisualInstance.ItemIsSelectable | VisualInstance.NoSelectionOutline
                                               //                                               cacheMode: 1
                                           })
            edgeUid2item[uid] = i
            i.pen.color = "#80ddff"
            i.pen.width = 4
            i.pen.cosmetic = true
            i.sourcePen.color = "#888"
            i.sourcePen.cosmetic = true
            i.sourcePen.strokeStyle = Qt.SolidLine
            i.drawSource = true
            i.drawResult = false

            i.onHoverEnter = onHoverEnter
            i.onHoverLeave = onHoverLeave
            i.onHoverMove = onHoverMove

            i.onMousePress = route.onMousePress
            i.onMouseMove = route.onMouseMove
            i.onMouseRelease = route.onMouseRelease

            i.hoverEnabled = true
            i.updatePath()
        }

        function onHoverEnter(event) {
            //            console.log("hover enter", r.pos, r.point)
            hover_handle.runner = this.result.findNearestPos(event.pos)
            hover_handle.visible = true
            hover_handle.updateHandle(this.result.pathData().name)
        }
        function onHoverMove(event) {
            //            console.log("hover on", JSON.stringify(this.result.pathData()))
            hover_handle.runner = this.result.findNearestPos(event.pos)
            hover_handle.updateHandle(this.result.pathData().name)
        }

        function onHoverLeave(event) {
            hover_handle.visible = false
        }

    }

    ListModel {
        id: route

        property var edgeUid2modelIdx: []
        property int currentEdgeUid
        property var candidateEdges: []

        property var helpers:[]
        //        onRowsAboutToBeRemoved: {
        //            var removed = helpers.splice(first, last-first + 1)
        //            console.log("removed %1 rows".arg(last-first + 1), removed)
        //            removed.forEach(function (e) {
        //                console.log("Kill", e)
        //                e.destroy()
        //            })
        //        }
        //        onRowsInserted: {
        //            var row = get(first)
        //            var eh = edgeModel.getHandle(row.edge)
        //            var p = creatist.create("GuidePath")
        //            var ed = eh.getData()
        //            p.setPolygon(ed.polygon)
        //            var i = m_sceneBuilder.addItem("GuidePathStroke",
        //                                           {
        //                                               smooth: 0,
        //                                               precision: 2,
        //                                               source: p,
        //                                               flags: VisualInstance.ItemIsSelectable
        //                                               //                                               cacheMode: 1
        //                                           })
        //            i.pen.color = Qt.hsla(Math.random(), 0.5, 0.5)
        //            i.pen.width = 2
        //            i.pen.cosmetic = true
        //            i.sourceStart = row.startPos
        //            i.sourceEnd = row.endPos
        //            i.offset = 3
        //            i.drawSource = false
        //            i.drawResult = true
        //            helpers.push(i)
        //            console.log("added hop", i.offset, row.startPos, row.endPos)

        //        }
        //        onDataChanged: {
        //            var i = helpers[bottomRight.row]
        //            var d = route.get(topLeft.row)
        //            i.sourceStart = d.startPos
        //            i.sourceEnd = d.endPos
        //            //            console.log(topLeft.row, topLeft.column, topLeft.model, topLeft.internalId, topLeft.flags)
        //        }


        function appendSegment(edgeUid, runner, entry)
        {
            var i = edgeModel.edgeUid2item[edgeUid]
            if (entry === undefined)
                i.strokeStart = runner.position
            else
                i.strokeStart = entry * i.source.length
            append({
                       uid: edgeUid,
                       startPos: i.strokeStart,
                       endPos: runner.position,
                       startX: runner.point.x,
                       startY: runner.point.y,
                       endX: runner.point.x,
                       endY: runner.point.y,
                       len: i.source.length
                   })
            edgeUid2modelIdx[edgeUid] = count - 1
            i.drawResult = true
            i.strokeEnd = runner.position
            currentEdgeUid = edgeUid
            console.log("added segment %1 (edge #%2), entry pos: %3, current pos: %4".arg(count - 1).arg(edgeUid).arg(i.strokeStart).arg(i.strokeEnd), runner.position)
        }

        function updateEndPos(runner) {
            if (edgeUid2modelIdx[currentEdgeUid] === undefined) {
                console.log("edge", edgeIndex, "unknown")
                return
            }
            var endpos
            if (runner.atEnd)
                endpos = edgeModel.edgeUid2item[currentEdgeUid].source.length
            set(edgeUid2modelIdx[currentEdgeUid], {
                    endPos: runner.position,
                    endX: runner.point.x,
                    endY: runner.point.y
                })
            var i = edgeModel.edgeUid2item[currentEdgeUid]
            i.drawResult = true
            i.strokeEnd = runner.position
        }

        function findCandidateEdges(node, runner) {
            //            candidateEdges.forEach(function(e){
            //                var d = edgeModel.edgeData(e)
            //                d.instance.sourcePen.color = "#A80"
            ////                d.instance.drawResult = false
            //            })
            candidateEdges = []
            node.outEdges("polygon").forEach(function(e){
                if (e.uid == currentEdgeUid)
                    return
                candidateEdges.push( { uid: e.uid, pos: 0 })
            })
            node.inEdges("polygon").forEach(function(e){
                if (e.uid == currentEdgeUid)
                    return
                candidateEdges.push( { uid: e.uid, pos: 1 })
            })
            console.log("candidates", JSON.stringify(candidateEdges), "Current:", JSON.stringify({uid: currentEdgeUid, pos: runner.position/edgeModel.edgeUid2item[currentEdgeUid].source.length }))
        }

        function distance(p1, p2) {
            return Qt.vector2d(p2.x - p1.x, p2.y - p1.y).length()
        }


        function switchCurrentEdge(edgeUid, runner, entry) {
            var inst = edgeModel.edgeUid2item[currentEdgeUid]
            inst.strokeEnd = inst.strokeStart

            remove(edgeUid2modelIdx[currentEdgeUid])
            edgeUid2modelIdx[currentEdgeUid] = undefined
            if (edgeUid2modelIdx[edgeUid]) {
                console.log("move to already visited edge", edgeUid2modelIdx[edgeUid], count, get(count-1).startPos, get(count-1).endPos)
                currentEdgeUid = edgeUid
                updateEndPos(runner)
            } else
                appendSegment(edgeUid, runner, entry)
        }

        function findCandidate(desiredPos) {
            var nextEdge = -1
            var dist = 1e38
            var runner
            var entry
            candidateEdges.forEach(function(i) {
                //                var e = edgeModel.edgeData(i.edge)
                var r = edgeModel.edgeUid2item[i.uid].source.findNearestPos(desiredPos)
                var d = distance(desiredPos, r.point)
                if (d < dist) {
                    nextEdge = i.uid
                    runner = r
                    dist = d
                    entry = i.pos
                }
            })
            if (nextEdge < 0) {
                var r = edgeModel.edgeUid2item[currentEdgeUid].source.findNearestPos(desiredPos)
                console.log("no available candidate edge for position", desiredPos, get(count - 1).startPos)
                return { uid: currentEdgeUid, runner: r, entry: get(count - 1).startPos }
            }
            if (edgeUid2modelIdx[nextEdge] != undefined && edgeUid2modelIdx[nextEdge] - 1 > count) {
                console.warn("move back more than one step", edgeUid2modelIdx[nextEdge] - 1 , count)
            }

            return { uid: nextEdge, runner: runner, entry: entry }
        }

        function onMousePress(event) {
            var r = this.source.findNearestPos(event.pos)
            candidateEdges.push({ uid: this.source.headData(), pos: r.position / this.result.length })
            appendSegment(this.source.headData(), r)
            drag_handle.visible = true
            drag_handle.runner = r
            drag_handle.updateHandle()
            //            event.accepted = false
        }
        function onMouseMove(event) {
            //            updateHandlePosition(event.pos)
            var c = findCandidate(event.pos)
            drag_handle.runner = c.runner
            drag_handle.updateHandle()
            if (c.uid == currentEdgeUid) {
                updateEndPos(c.runner)
            } else {
                switchCurrentEdge(c.uid, c.runner, c.entry)
            }

            var edgeUid = c.uid
            var eh = edgeModel.getHandleByUid(edgeUid)

            if (c.runner.atEnd) {
                findCandidateEdges(eh.endNode, c.runner)
            } else if (c.runner.atStart) {
                findCandidateEdges(eh.startNode, c.runner)
            } else
                return

            c = findCandidate(event.pos)
            if (edgeUid2modelIdx[c.uid] !== undefined) {
                console.log("edge %1 already is in the route model".arg(c.uid))
                switchCurrentEdge(c.uid, c.runner, c.entry)
            } else {
                console.log("adding hop %1".arg(c.uid))
                appendSegment(c.uid, c.runner, c.entry)
            }

            //            if (c.edge !== currentEdge) {
            //                console.log("travel to next edge", c.edge, c.runner.position, c.entry)
            //                if (edge2model[c.edge] !== undefined) {
            //                } else {
            //                    console.log("setting runner at edge #%1 at pos %2, entry pos %3".arg(c.edge).arg(c.runner.position).arg(c.entry))
            //                    //                    c.runner.position
            //                }
            //            }
        }

        function onMouseRelease(event) {
            drag_handle.visible = false
            buildPath()

        }

        function buildPath() {
            var path = creatist.create("GuidePath")

            var ph = ways_model.rootHandle
            if (!ways_model.linked) {
                f_Node.parentHandle = ways_model.rootHandle
                ph = f_Node.create("ways")
            }
            f_Blob.parentHandle = ph
            f_Blob.type = "GuideRoute"
            var itemcolor =  Qt.hsla(Math.random(), 0.9, 0.5)
            f_Blob.metadata = { name: "untitled", color: itemcolor.toString() }
            var wayContainer = f_Blob.create("way%1".arg(ph.childCount))
            f_Blob.parentHandle = wayContainer
            var prev
            //            var path

            for (var i = 0; i < count; ++i) {
                var s = get(i)
                f_Blob.type = "GuideRouteElement"
                var seg = edgeModel.getHandleByUid(s.uid)
                var startNodeHandle = seg.startNode
                var endNodeHandle = seg.endNode
                var item = edgeModel.edgeUid2item[s.uid]
                //                console.log("adding segment", s.uid, "entry", s.startPos, "exit", s.endPos, "item", item)
                if (i != 0 && i != count - 1) {
                    if (s.startPos > s.endPos)
                        path.appendPath(item.source.reversed())
                    else
                        path.appendPath(item.source)
                } else {
                    var partPath = item.source.part(s.startPos, s.endPos)
                    path.appendPath(partPath)
                }
                f_Blob.metadata = {
                    edgeUid: s.uid,
                    startPos: s.startPos,
                    endPos: s.endPos,
                    offset: 0,
                    wires: 1,
                    color: itemcolor.toString(),
                    radius: 1,
                    precision: 2
                }

                var guide = item.result
                f_Blob.data = creatist.serialize(guide)
                var segnode = f_Blob.create("seg#%1".arg(s.uid))
                if (prev)
                    gmodel.addEdge(prev, segnode, "GuideLink")
                prev = segnode
                gmodel.addEdge(segnode, startNodeHandle, "startnode")
                gmodel.addEdge(segnode, endNodeHandle, "endNode")


                item.drawResult = false
                item.strokeEnd = -1
                item.strokeStart = 0
            }
            var agent = wayContainer.agent
            agent.blob$data = creatist.serialize(path)
            //            var i = m_sceneBuilder.addItem("GuidePathStroke", {
            //                                               smooth: 0,
            //                                               offset: 1,
            //                                               precision: 8
            //                                           } )
            //            i.pen.width = 2
            //            i.pen.color = "#0ff"
            //            i.pen.strokeStyle = Qt.DashLine
            //            i.pen.cosmetic = true
            //            i.source = path

            clear()
            edgeUid2modelIdx = []
            currentEdgeUid = -1
            candidateEdges = []
        }

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
        onTriggered: gmodel.exportModel(gmodel.file, gmodel.root)
    }


    FileDialog {
        id: fileselector
        selectExisting: true
        modality: Qt.NonModal
        folder: shortcuts.desktop
        nameFilters: ["MetaGraph model files (*.mgf)"]
        onAccepted: {
            gmodel.file = fileUrl
            //            console.log("loading model", fileUrl.toString())
            gmodel.loadModel(fileUrl)
            //            console.log("model loaded")
        }
    }

    FileDialog {
        id: saveFileselector
        folder: shortcuts.desktop
        //        modality: Qt.NonModal
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

}

