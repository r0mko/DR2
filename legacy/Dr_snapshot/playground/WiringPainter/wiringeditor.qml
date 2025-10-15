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
import Playground 1.0

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

    //    TransportSimulator {
    //        Component.onCompleted: initializeFromOverpassXML("/Users/romko/Desktop/data")
    //        onVehiclePositionUpdated: console.log("Vehicle %1 pos %2".arg(id).arg(pos))
    //        onInitialized: {
    //            start(60)
    //        }
    //    }

    MenuBar {
        Menu {
            title: "Файл"
            MenuItem { action: a_openModel }
            MenuItem { action: a_saveAs }
            MenuItem { action: a_save }
            MenuItem { action: a_reloadModel }
        }
    }
    Rectangle {
        id: l_1
        color: "#222"
        anchors.fill: parent

        SceneModel {
            id: m_sceneBuilder
            scene: m_scene.qGraphicsScene
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
            grid.visible: true

            MouseArea {
                id: scene_mouse
                acceptedButtons: Qt.AllButtons
                anchors.fill: parent
                onPressed: m_scene.clearSelection()
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
                //                                renderer.renderTarget: SceneRenderer.Image
                //                renderer.renderTarget: SceneRenderer.FramebufferObject
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
                Repeater {
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

                MarkerItem {
                    id: hover_handle
                    //                    property var posPath
                    //                    property var negPath
                    //                    Item {
                    //                        id: container
                    //                        width: 1
                    //                        height: 1
                    //                        anchors.centerIn: hover_handle
                    //                        rotation: -hover_handle.rotation
                    //                        Rectangle {
                    //                            id: negVec
                    //                            width: 10
                    //                            height: 10
                    //                            color: "#80ffff"
                    //                            border.width: 2
                    //                            border.color: "#0080ff"
                    //                            transform: Translate {
                    //                                x: -3
                    //                                y: -3
                    //                            }
                    //                        }
                    //                        Rectangle {
                    //                            id: posVec
                    //                            width: 10
                    //                            height: 10
                    //                            color: "#ffff80"
                    //                            border.width: 2
                    //                            border.color: "#800000"

                    //                            transform: Translate {
                    //                                x: -3
                    //                                y: -3
                    //                            }
                    //                        }
                    //                        Rectangle {
                    //                            id: negPt
                    //                            width: 6
                    //                            height: 6
                    //                            color: "#0080ff"
                    //                            transform: Translate {
                    //                                x: -3
                    //                                y: -3
                    //                            }
                    //                        }
                    //                        Rectangle {
                    //                            id: posPt
                    //                            width: 6
                    //                            height: 6
                    //                            color: "#ff8080"
                    //                            transform: Translate {
                    //                                x: -3
                    //                                y: -3
                    //                            }
                    //                        }

                    //                    }

                    //                    Component.onCompleted: {
                    //                        posPath = m_sceneBuilder.addItem("GuideStroke", {z: 2})
                    //                        negPath = m_sceneBuilder.addItem("GuideStroke", {z: 2})
                    //                        posPath.pen.color = "#ffab40"
                    //                        posPath.pen.cosmetic = true
                    //                        posPath.pen.width = 2
                    //                        negPath.pen.color = "#40abff"
                    //                        negPath.pen.cosmetic = true
                    //                        negPath.pen.width = 2
                    //                    }

                    //                    onVisibleChanged: {
                    //                        negPath.visible = visible
                    //                        posPath.visible = visible
                    //                    }

                    //                    onRunnerChanged: {
                    //                        var smoothData = runner.debugRounding(m_sidebar.curveSmoothing)

                    //                        negPath.source = smoothData.negPath
                    //                        posPath.source = smoothData.posPath
                    //                        negPath.update()
                    //                        posPath.update()
                    //                        negVec.x = smoothData.negVec.x  *  20
                    //                        negVec.y = smoothData.negVec.y  *  20
                    //                        posVec.x = smoothData.posVec.x  *  20
                    //                        posVec.y = smoothData.posVec.y  *  20
                    //                        negPt.x = smoothData.negPt.x    *  20
                    //                        negPt.y = smoothData.negPt.y    *  20
                    //                        posPt.x = smoothData.posPt.x    *  20
                    //                        posPt.y = smoothData.posPt.y    *  20

                    //                        //                        smoothDelta.x = smoothData.negPt
                    //                    }
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
        width: 320
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
            property bool debugMode: false
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
                        minimumValue: -25
                        maximumValue: 25
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
                JSObjectViewRow {
                    text: "Debug nodes"
                    valueDelegate: CheckBox {
                        checked: m_sidebar.debugMode
                        onClicked: m_sidebar.debugMode = checked
                    }
                }

            }


            property var handle
            onHandleChanged: console.log("handle item", handle)

            Creatist {
                id: creatist
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
                            text: "Take last"
                            onClicked: {
                                smoother.currentItem.source.takeLastVectorNode()
                                smoother.currentItem.update()
                            }
                        }
                        Button {
                            text: "Take first"
                            onClicked: {
                                smoother.currentItem.source.takeFirstVectorNode()
                                smoother.currentItem.update()
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

            ColumnLayout {
                id: way_edit

                Layout.fillWidth: true
                //                Layout.minimumHeight: 300
                //                visible: false
                CaptionLabel {
                    text: "Участки"
                    Layout.fillWidth: true
                    level: 3
                }

                TableView {
                    id: ways_table
                    Layout.fillWidth: true
                    Layout.minimumHeight: 200
                    selectionMode: SelectionMode.MultiSelection
                    model: ways_model
                    TableViewColumn {
                        role: "nodeId"
                    }
                    TableViewColumn {
                        role: "blob$metadata"
                        delegate: Rectangle {
                            anchors.fill: parent
                            ColorDialog {
                                id: colorpicker
                                color: styleData.value.color
                                //                                onAccepted:
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: colorpicker.open()

                            }
                        }
                    }
                }
                RowLayout {
                    Layout.fillWidth: true
                    Button {
                        id: b_merge
                        enabled: ways_table.selection.count > 1
                        text: "Merge"
                        onClicked: {
                            var receiver
                            ways_table.selection.forEach( function(rowIndex) {
                                if (!receiver) {
                                    receiver = i_ways.getInstance(ways_model.getHandle(rowIndex).nodeId)
                                } else {
                                    var h = ways_model.getHandle(rowIndex)
                                    receiver.merge(i_ways.getInstance(h.nodeId))
                                    h.destroy()
                                }
                            })
                        }
                    }

                    Button {
                        id: b_del
                        text: "Remove"
                        enabled: false
                    }

                    Button {
                        id: b_extend
                        text: "Extend"
                        enabled: false
                        checkable: true
                    }
                }

                CaptionLabel {
                    id: l_waylabel
                    text: "Сегменты"
                    Layout.fillWidth: true
                    level: 3
                }
                TableView {
                    id: segs_table
                    Layout.fillWidth: true
                    Layout.minimumHeight: 200
                    model: null
                    selectionMode: SelectionMode.MultiSelection
                    onSelectionChanged: {
                        console.log(selection)
                        var ph
                        selection.forEach( function(rowIndex) {
                            if (!ph) {
                                ph = segs_table.model.getHandle(rowIndex);
                                return;
                            }
                            var ch = segs_table.model.getHandle(rowIndex)
                            gmodel.addEdge(ph, ch, "GuideLink|JointVertex")
                            ph = ch
                        })
                    }

                    property var currentItem
                    //                    onCurrentRowChanged: {
                    //                        var h = model.getHandle(currentRow)
                    //                        var guide = creatist.deserialize("Guide", h.data)
                    //                        if (!currentItem) {
                    //                            currentItem = m_sceneBuilder.addItem("GuideStroke",
                    //                                                                 {
                    //                                                                     smooth: 0,
                    //                                                                     offset: 0,
                    //                                                                     precision: 0,
                    //                                                                     fast: false
                    //                                                                     //                                                                 type: GuidePAth
                    //                                                                 })

                    //                            currentItem.pen.cosmetic = true
                    //                            currentItem.pen.color = "#ddaaff"
                    //                            currentItem.pen.width = 4
                    //                            currentItem.pen.capStyle = Qt.RoundCap

                    //                            currentItem.pen.cosmetic = true
                    //                            //                            currentItem.pen.strokeStyle = Qt.DotLine
                    //                            currentItem.drawSource = false
                    //                            //                            currentItem.setDebugMode(true)
                    //                        }
                    //                        currentItem.source = guide
                    //                        //                        guide.outline(10 * m_scene.scale, Qt.FlatCap)
                    //                        currentItem.strokeStart = h.metadata.startPos
                    //                        currentItem.strokeEnd = h.metadata.endPos
                    //                        currentItem.update()



                    //                        console.log(h)

                    //                    }

                    TableViewColumn {
                        role: "nodeId"
                        delegate: Label {
                            text: styleData.value
                            color: styleData.textColor
                        }
                    }
                    TableViewColumn {
                        title: "EntryPos"
                        role: "blob$metadata"
                        delegate: Label {
                            text: ~~styleData.value.startPos
                            color: styleData.textColor
                        }
                    }
                    TableViewColumn {
                        title: "ExitPos"
                        role: "blob$metadata"
                        delegate: Label {
                            text: ~~styleData.value.endPos
                            color: styleData.textColor
                        }
                    }
                    TableViewColumn {
                        title: "Offset"
                        role: "blob$metadata"
                        delegate: Label {
                            text: styleData.value ? styleData.value.offset : ""
                            color: styleData.textColor
                        }
                    }
                }

                RowLayout {
                    Button {
                        id: b_addway
                        text: "Add"
                        checkable: true
                    }
                    Button {
                        id: b_split
                        text: "Split"
                        checkable: true
                        enabled: false
                    }

                    Button {
                        id: b_connect
                        text: "Connect"
                        enabled: false
                    }
                }
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
                property real scaling: m_scene.viewportScale
                property bool debugMode: m_sidebar.debugMode


                onPolygonChanged: {
                    if (!m_sceneBuilder.scene)
                        return
                    if (!currentItem) {
                        currentItem = m_sceneBuilder.addItem("GuideStroke",
                                                             {
                                                                 smooth: smooth,
                                                                 offset: offset,
                                                                 precision: precision,
                                                                 fast: fast
                                                             })
                        currentItem.pen.cosmetic = true
                        currentItem.pen.color = UI.fromHSLum(Math.random(), 1.0, 0.5)
                        currentItem.pen.width = 3
                        currentItem.pen.strokeStyle = Qt.SolidLine
                        currentItem.pen.cosmetic = true
                        currentItem.sourcePen.width = 1
                        currentItem.sourcePen.color = "#00ff80"
                        currentItem.sourcePen.cosmetic = true
                        currentItem.drawSource = true
                        currentItem.setDebugMode(debugMode)
                    }
                    var src = creatist.create("Guide")
                    src.setPolygon(polygon)
                    currentItem.source = src
                    currentItem.updatePath(true)

                }

                onSmoothChanged: updateCurrentItem()
                onOffsetChanged: updateCurrentItem()
                onPrecisionChanged: updateCurrentItem()
                onFastChanged: updateCurrentItem()
                onScalingChanged: updateCurrentItem()
                onDebugModeChanged: updateCurrentItem()


                function updateCurrentItem() {
                    currentItem.smooth = smooth
                    currentItem.precision = precision
                    currentItem.offset = offset
                    currentItem.fast = fast
                    currentItem.scalingFactor = scaling
                    currentItem.setDebugMode(debugMode)
                    //                    var r = currentItem.source.smoothed(smooth, precision)

//                    currentItem.source.setPolygon(polygon)
                    currentItem.updatePath(true)
                    //                    currentDebugItem.path = r.wavefront_debug(offset)
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

            ColumnLayout {
                id: buttons_0
                Layout.fillHeight: true
                Layout.fillWidth: true
            }


            FileDialog {
                id: importDialog
                //            id: fileselector
                //                modality: Qt.NonModal
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

        Component.onCompleted: loadModel("/Users/romko/Desktop/trams2.mgf")
        //        Component.onCompleted: loadModel("/Users/romko/Desktop/msk-egko.mgf")
        //        Component.onCompleted: loadModel("/Users/romko/Desktop/trams_net.mgf")
        //                Component.onCompleted: loadModel("/Volumes/Romko/Dropbox/msk_roads.mgf")
        //                        Component.onCompleted: loadModel("/Volumes/Romko/Desktop/segs2.mgf")
        //                                Component.onCompleted: loadModel("/Volumes/Romko/Desktop/rails.mgf")

        NodeChildrenModel {
            id: ways_model
            mrl: "/ways"
            property alias smooth: m_sidebar.curveSmoothing
            property alias precision: m_sidebar.curvePrecision
            property alias offset: m_sidebar.baseCurveOffset
            property alias fast: m_sidebar.fastMode

            function createNewWay(wayName, color, idf) {
                f_Blob.parentHandle = rootHandle
                f_Blob.type = "GuideRoute"
                var itemcolor =  Qt.hsla(Math.random(), 1, 0.8)
                var metadata = {
                    name: wayName,
                    wires: 1,
                    idf: idf,
                    color: color.toString()
                }
                f_Blob.metadata = metadata
                return f_Blob.create(wayName)
            }

            function getHandleByInstance(wayName) {
                return findChild(wayName.name)
            }

            function getSegmentHandle(wayId, segUid) {
                return findChild(wayId).findChild("seg#%1".arg(segUid))
            }

            function createSegment(parentHandle, uid, guide, entryPos, exitPos, offset) {
                f_Blob.parentHandle = parentHandle
                f_Blob.type = "GuideNetSegment"
                f_Blob.metadata = {
                    edgeUid: uid,
                    startPos: entryPos,
                    endPos: exitPos,
                    offset: offset
                }
                f_Blob.data = creatist.serialize(guide)
                console.log("created segment %1 for way %2".arg(uid).arg(parentHandle.nodeId))
                return f_Blob.create("seg#%1".arg(uid))
            }

        }
    }



    EdgeModel {
        id: roadGraph
        model: gmodel
        tag: 'polygon'
        property var edgeUid2item: []

        property bool interactive: b_addway.checked
        onInteractiveChanged: {
            edgeUid2item.forEach(function(e) {
                e.hoverEnabled = interactive

                if (interactive) {
                    e.sourcePen.color = "#68a"
                    e.acceptedMouseButtons = Qt.LeftButton
                    e.onMousePress = route.onMousePress
                    e.onMouseMove = route.onMouseMove
                    e.onMouseRelease = route.onMouseRelease
                } else {
                    e.sourcePen.color = "#555"
                    e.acceptedMouseButtons = Qt.NoButton
                    e.onMousePress = undefined
                    e.onMouseMove = undefined
                    e.onMouseRelease = undefined
                }
                e.update()

            } )
        }


        onItemAdded: {
            var eh = getHandleByUid(uid)
            var p = creatist.create("Guide")
            var ed = eh.getData()
            p.setPolygon(ed.polygon)
            p.setHeadData(uid)
            p.setTailData(uid)
            p.setPathData(ed.attributes)
            var i = m_sceneBuilder.addItem("GuideStroke", {
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
            i.sourcePen.color = "#555"
            i.sourcePen.cosmetic = true
            i.sourcePen.strokeStyle = Qt.SolidLine
            i.drawSource = true
            i.drawResult = false

            i.onHoverEnter = onHoverEnter
            i.onHoverLeave = onHoverLeave
            i.onHoverMove = onHoverMove

            //            i.onMousePress = route.onMousePress
            //            i.onMouseMove = route.onMouseMove
            //            i.onMouseRelease = route.onMouseRelease

            i.hoverEnabled = interactive
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


        function appendSegment(edgeUid, runner, entry)
        {
            var i = roadGraph.edgeUid2item[edgeUid]
            if (entry === undefined)
                i.strokeStart = runner.position
            else
                i.strokeStart = entry * i.source.length
            append({
                       uid: edgeUid,
                       startPos: i.strokeStart,
                       endPos: runner.position,
                       offset: 1
                   })
            edgeUid2modelIdx[edgeUid] = count - 1
            i.drawResult = true
            i.strokeEnd = runner.position
            currentEdgeUid = edgeUid
            //            console.log("added segment %1 (edge #%2), entry pos: %3, current pos: %4".arg(count - 1).arg(edgeUid).arg(i.strokeStart).arg(i.strokeEnd), runner.position)
        }

        function updateEndPos(runner) {
            if (edgeUid2modelIdx[currentEdgeUid] === undefined) {
                console.log("edge", edgeIndex, "unknown")
                return
            }
            var endpos
            if (runner.atEnd) endpos = roadGraph.edgeUid2item[currentEdgeUid].source.length
            set(edgeUid2modelIdx[currentEdgeUid], {
                    endPos: runner.position
                })
            var i = roadGraph.edgeUid2item[currentEdgeUid]
            i.drawResult = true
            i.strokeEnd = runner.position
        }

        function findCandidateEdges(node, runner) {
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
            //            console.log("candidates", JSON.stringify(candidateEdges), "Current:", JSON.stringify({uid: currentEdgeUid, pos: runner.position/edgeModel.edgeUid2item[currentEdgeUid].source.length }))
        }

        function distance(p1, p2) {
            return Qt.vector2d(p2.x - p1.x, p2.y - p1.y).length()
        }


        function switchCurrentEdge(edgeUid, runner, entry) {
            var inst = roadGraph.edgeUid2item[currentEdgeUid]
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
                var r = roadGraph.edgeUid2item[i.uid].source.findNearestPos(desiredPos)
                var d = distance(desiredPos, r.point)
                if (d < dist) {
                    nextEdge = i.uid
                    runner = r
                    dist = d
                    entry = i.pos
                }
            })
            if (nextEdge < 0) {
                var r = roadGraph.edgeUid2item[currentEdgeUid].source.findNearestPos(desiredPos)
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
            var eh = roadGraph.getHandleByUid(edgeUid)

            if (c.runner.atEnd) {
                findCandidateEdges(eh.endNode, c.runner)
            } else if (c.runner.atStart) {
                findCandidateEdges(eh.startNode, c.runner)
            } else
                return

            c = findCandidate(event.pos)
            if (edgeUid2modelIdx[c.uid] !== undefined) {
                //                console.log("edge %1 already is in the route model".arg(c.uid))
                switchCurrentEdge(c.uid, c.runner, c.entry)
            } else {
                //                console.log("adding hop %1".arg(c.uid))
                appendSegment(c.uid, c.runner, c.entry)
            }
        }

        function onMouseRelease(event) {
            drag_handle.visible = false
            buildPath()
        }

        function buildPath() {
            var ph = ways_model.rootHandle
            if (!ways_model.linked) {
                f_Node.parentHandle = ways_model.rootHandle
                ph = f_Node.create("ways")
            }
            f_Blob.parentHandle = ph
            f_Blob.type = "GuideRoute"
            var itemcolor =  Qt.hsla(Math.random(), 1, 0.8)
            f_Blob.metadata = {
                name: "untitled",
                radius: 15,
                color: itemcolor.toString()
            }
            var wayContainer = f_Blob.create("way%1".arg(ph.childCount))
            f_Blob.parentHandle = wayContainer
            var prev
            for (var i = 0; i < count; ++i) {
                var s = get(i)


                f_Blob.type = "GuideNetSegment"
                var seg = roadGraph.getHandleByUid(s.uid)
                var startNodeHandle = seg.startNode
                var endNodeHandle = seg.endNode


                var item = roadGraph.edgeUid2item[s.uid]
                f_Blob.metadata = {
                    edgeUid: s.uid,
                    startPos: s.startPos,
                    endPos: s.endPos,
                    offset: 0,
                    wires: 1
                }

                var edgeCount = endNodeHandle.outEdges().length
                console.log("at end of %1 out count: %2".arg(s.uid).arg(edgeCount))

                var guide = item.source
                f_Blob.data = creatist.serialize(guide)
                var segnode = f_Blob.create("seg#%1".arg(s.uid))

                if( i === 0 )
                    gmodel.addEdge(wayContainer, segnode, 'GuideLink|StartVertex')
                if (prev)
                    gmodel.addEdge(prev, segnode, "GuideLink|JointVertex")
                if( i === count - 1 )
                    gmodel.addEdge(segnode, wayContainer, 'GuideLink|EndVertex')
                prev = segnode

                //                gmodel.addEdge(segnode, startNodeHandle, "startnode")
                //                gmodel.addEdge(segnode, endNodeHandle, "endNode")

                item.drawResult = false
                item.strokeEnd = -1
                item.strokeStart = 0
            }
            //            var agent = wayContainer.agent
            clear()
            edgeUid2modelIdx = []
            currentEdgeUid = -1
            candidateEdges = []
        }

    }

    Instantiator {
        id: i_ways
        model: ways_model
        delegate: GuideNet {

            id: g_net
            scene: m_scene
            name: agent.nodeId
            flags: VisualInstance.ItemIsSelectable
            property bool debugmode: m_sidebar.debugMode
            property QtObject agent: object
            property alias segments: segmentsModel
            property GuideNet currentDetach: null
            property var extendingSegment: null
            Repeater {
                z: 1200
                id: handlemaker
                model: null
                parent: m_scene
                delegate: HandleItem {
                    property var runner
                    property var runner2
                    property var v: modelData
                    property var guide: v.isStart ? v.to : v.from
                    drag.target: dummy
                    border.color: agent.blob$metadata.color
                    height: 24
                    //                    mouseArea.onMouseXChanged: console.log(mouseArea.mouseX)
                    drag.onActiveChanged: {
                        if (!drag.active) {
                            dummy.x = 0
                            dummy.y = 0
                        }

                    }
                    //                    property var ps: mapToItem(m_scene, dummy.x, dummy.y)
                    //                    property var ps: Qt.point(mouseArea.mouseX.x, mouseArea.mouseY)
                    property var ps: mapToItem(m_scene, mouseArea.mouseX, mouseArea.mouseY)
                    onPsChanged: {

                        if (drag.active) {
                            console.log("seeking for nearest points at segments %1 and %2".arg(v.from.name).arg(v.to.name), "dummy pos", ps)
                            runner2 = v.from.path.findNearestPos(ps)
                            console.log("path pos:", runner2.position, "\nold dest entry:", v.to.entryPos, "\nold src exit:", v.from.exitPos)
                            var srcSeg = v.from
                            var dstSeg = v.to
                            v.from.net.updateSegment(srcSeg.uid, srcSeg.entryPos, runner2.position, srcSeg.offset)
                            v.to.net.updateSegment(dstSeg.uid, runner2.position, dstSeg.exitPos, dstSeg.offset)

                            v.from.net.update()
                            v.to.net.update()


                            //                            rotation = runner2.angle
                            //                            x = runner2.point.x// modelData.x
                            //                            y = runner2.point.y// modelData.y
                        }
                    }

                    onRunnerChanged: {
                        if (runner) {
                            visible = true
                            rotation = runner.angle
                            x = runner.point.x// modelData.x
                            y = runner.point.y// modelData.y
                        } else {
                            visible = false
                        }
                    }
                    Component.onCompleted: runner = v.isStart ? v.to.result.headRunner() : v.from.result.tailRunner()
                    Item {
                        id: dummy
                        parent: m_scene
                        width: 1
                        height: 1
                    }

                }
            }

            //            property var utilityItems: []

            onSegmentChanged: {
                var h = segmentsModel.findChild(name)
                var d = h.metadata
                //                console.log("segment:", JSON.stringify(d), name, h)
                //                console.log(g_net.name, "updating segment %1 metadata: startPos %2 endPos %3 offset %4".arg(h.nodeId).arg(exitPos).arg(entryPos).arg(offset))
                d.startPos = entryPos
                d.endPos = exitPos
                d.offset = offset
                h.metadata = d
            }


            onDebugmodeChanged: setDebugNodes(debugmode)
            acceptedButtons: Qt.AllButtons
            pen.color: agent.blob$metadata.color
            pen.width: 3
            pen.cosmetic: true
            smooth: m_sidebar.curveSmoothing
            precision: m_sidebar.curvePrecision

            hoverEnabled: true

            Component.onCompleted: {
                b_addway.checked = false
                selected = true
            }


            Binding on children {
                when: g_net.agent != null
                value: agent.children
            }
            StateGroup {
                id: gstates
                states: [
                    State {
                        name: "extend"
                        when: b_extend.checked

                        PropertyChanges {
                            target: roadGraph
                            interactive: true
                        }

                        PropertyChanges {
                            target: g_net

                            onHoverEnter: {
                                hover_handle.visible = true
                                var r = findNearestPoint(hover.pos)
                                var s = segmentAt(hover.pos)
                                hover_handle.runner = r
                                hover_handle.updateHandle("seg#%1".arg(s.uid))
                            }

                            onHoverMove: {
                                var r = findNearestPoint(hover.pos)
                                var s = segmentAt(hover.pos)
                                hover_handle.runner = r
                                hover_handle.updateHandle("seg#%1".arg(s.uid))
                            }

                            onHoverLeave: {
                                hover_handle.visible = false
                            }

                            onMousePress: {
                                mouse.accepted = false
                                hover_handle.visible = true
                                var r = findNearestPoint(mouse.pos)
                                var s = g_net.segmentAt(mouse.pos)
                                console.log("click at", s.uid, s.entryPos, s.exitPos)
                                extendingSegment = s
                            }
                            onMouseRelease: {
                                console.log("move release")
                                for (var i = 0; i < route.count; ++i) {
                                    var e = route.get(i)
                                    //                                    console.log(JSON.stringify(e))
                                }
                                //                                g_net.ungrabMouse()
                                route.clear()
                                mouse.accepted = true
                            }

                        }

                        PropertyChanges {
                            target: route

                            onRowsInserted: {
                                var i = get(first)
                                console.log("inserted seg #", first, i.uid, roadGraph.getHandleByUid(i.uid))
                                //                                if (!g_net.hasSegment(i.uid))
                                //                                    g_net.appendSegment(i.uid, )
                                if (g_net.extendingSegment) {
                                    g_net.extendingSegment.name = "s#%1(%2)".arg(i.uid).arg(g_net.extendingSegment.uid)
                                }
                            }
                            onDataChanged: {
                                var i = get(topLeft.row)
                                g_net.updateSegment(i.uid, i.startPos, i.exitPos, i.offset)
                                console.log("route changed", i.uid, topLeft.row, JSON.stringify(get(topLeft.row)))
                            }
                            onRowsAboutToBeRemoved: {
                                var i = get(first)
                                g_net.removeSegment(i.uid)
                            }

                        }
                    },

                    State {
                        name: "split"
                        extend: "selected"
                        when: b_split.checked


                        PropertyChanges {
                            target: g_net
                            hoverEnabled: true
                            onHoverEnter: {
                                hover_handle.runner = g_net.findNearestPoint(hover.pos)
                                hover_handle.visible = true
                                hover_handle.updateHandle()
                            }
                            onHoverMove: {
                                hover_handle.runner = g_net.findNearestPoint(hover.pos)
                                hover_handle.updateHandle()
                            }
                            onHoverLeave: hover_handle.visible = false
                            onMousePress: {
                                g_net.splitAtPoint(mouse.pos)
                            }

                            onSplitComplete: {
                                console.log("Split complete, position: %1".arg(splitRunner.position))
                                var peerTo = ways_model.getSegmentHandle(currentDetach.name, newDestination.uid)
                                var segFrom = ways_model.getSegmentHandle(g_net.name, newSource.uid)

                                addBoundary(newSource, newDestination, false)
                                gmodel.addEdge(segFrom, peerTo, "GuideLink|BoundaryVertex")
                                currentDetach.addBoundary(newSource, newDestination, true)
                                g_net.update()
                                currentDetach.update()
                                currentDetach = null
                                hover_handle.visible = false
                            }
                            onMouseRelease: {
                                hover_handle.visible = false
                                b_split.checked = false
                                gstates.state = ""
                            }

                            onSegmentDetached: {
                                //                                console.log("Segment detached from", g_net.name)
                                var wname = "%1_part2".arg(g_net.name)
                                var itemcolor =  Qt.hsla(Math.random(), 1, 0.8)
                                var way = ways_model.createNewWay(wname, itemcolor, -1)

                                currentDetach = i_ways.getInstance(wname)
                                currentDetach.pushSegment(segment)
                                ways_model.createSegment(way, segment.uid, segment.path, segment.entryPos, segment.exitPos, segment.offset)
                            }
                            onSegmentRemoved: {
                                console.log("Segment removed from", g_net.name, "adding it to", currentDetach.name)
                                console.log("Segment edge 4%1, entryPos: %2, exitPos: %3".arg(segment.uid).arg(segment.entryPos).arg(segment.exitPos))
                                currentDetach.pushSegment(segment)
                                ways_model.createSegment(currentDetach.agent.handle, segment.uid, segment.path, segment.entryPos, segment.exitPos, segment.offset)
                            }

                            onLinkRemoved: {
                                //                                console.log("Link removed from", g_net, uidFrom, uidTo, type, "currentDetach", currentDetach)
                                var self_segFrom = segmentsModel.getHandleByUid(uidFrom)
                                var self_segTo = segmentsModel.getHandleByUid(uidTo)
                                console.log("Removing seg",self_segTo)
                                self_segTo.destroy()

                                var peer_segFrom = ways_model.getSegmentHandle(currentDetach.name, uidFrom)
                                var peer_segTo = ways_model.getSegmentHandle(currentDetach.name, uidTo)

                                gmodel.addEdge(peer_segFrom, peer_segTo, "GuideLink|%1".arg(type))
                                //                                linksModel
                                if (type == "JointVertex") {
                                    currentDetach.addJoint(uidFrom, uidTo)
                                }

                            }

                        }
                    },
                    //                    State {
                    //                        name: "boundary"
                    //                        when: b_connect.checked
                    //                        PropertyChanges {
                    //                            target: m_scene
                    ////                            onSelectedItemsChanged:
                    //                        }
                    //                    },
                    State {
                        name: "selected"
                        when: g_net.selected
                        PropertyChanges {
                            target: b_split
                            enabled: true
                        }
                        PropertyChanges {
                            target: b_del

                            enabled: true
                            onClicked: {
                                var h = ways_model.getHandleByInstance(g_net)
                                h.destroy()
                            }
                        }
                        PropertyChanges {
                            target: l_waylabel
                            text: g_net.name
                        }
                        PropertyChanges {
                            target: segs_table
                            model: segmentsModel


                        }
                        //                        PropertyChanges {
                        //                            target: g_net
                        //                            onMousePress: {
                        //                                var r = g_net.findNearestPoint(mouse.pos)
                        //                                var seg= g_net.segmentAt(mouse.pos)

                        //                                console.log("mouse press", mouse.pos, r, seg)
                        //                                mouse.accepted = false
                        //                            }
                        //                        }
                        PropertyChanges {
                            target: handlemaker
                            model: g_net.bounds
                        }

                        PropertyChanges {
                            target: b_extend
                            enabled: true

                        }
                    }

                ]
            }


            NodeChildrenModel {
                id: segmentsModel
                rootIndex: agent.modelIndex
                onNodeAdded: addSegment(handle)
                onLinkedChanged: console.log("segments linked", count, agent)

                onRowsAboutToBeRemoved: {
                    var row = first
                    var h = getHandle(row)
                    console.log("row remove:", row, h)
                    g_net.removeSegment(h.metadata.edgeUid)
                }

                onModelReset: rebuildSegments()

                function getHandleByUid(uid) {
                    return findChild("seg#%1".arg(uid))
                }

                function addSegment(handle) {

                    console.log("adding segment", JSON.stringify(handle.metadata))
                    var guide = creatist.deserialize("Guide", handle.data)
                    g_net.appendSegment( handle.metadata.edgeUid, handle.nodeId, guide, handle.metadata.startPos, handle.metadata.endPos, 0)
                }

                function rebuildSegments() {
                    g_net.clearSegments()
                    for (var i = 0; i < count; ++i) {
                        var handle = getHandle(i)
                        addSegment(handle)

                    }
                }
            }


            EdgeModel {
                id: linksModel
                rootIndex: agent.modelIndex
                model: gmodel
                tag: "GuideLink"
                direction: EdgeModel.Any
                onModelReset: rebuildLinks()

                function rebuildLinks() {
                    g_net.clearLinks()
                    console.log("rebuilding links", g_net.name)
                    for (var i = 0; i < count; ++i) {
                        var eh = getHandle(i)
                        addLink(eh)
                    }

                }

                onEdgeRemoved: {
                    var eh = getHandleByUid(uid)
                    console.log("removed edge", eh.tags, eh.startNode.mrl, eh.endNode.metadata.mrl)
                    g_net.removeLink(eh.startNode.metadata.edgeUid, eh.endNode.metadata.edgeUid)
                }

                onItemAdded: {
                    var eh = getHandleByUid(uid)
                    addLink(eh)
                }

                function addLink(eh) {
                    var from = eh.startNode
                    var to = eh.endNode
                    //                    console.log("Edge from %1 to %2".arg(from.mrl).arg(to.mrl), from.parent, to.parent, from.parent == to.parent)


                    if (eh.hasTag('StartVertex')) {
                        g_net.addStart(eh.endNode.metadata.edgeUid)
                        //                    } else if (eh.hasTag('EndVertex')) {
                        //                        g_net.addEnd(eh.startNode.metadata.edgeUid)
                    } else if (eh.hasTag('JointVertex')) {
                        console.log("joint", eh.startNode, eh.endNode)
                        g_net.addJoint(eh.startNode.metadata.edgeUid, eh.endNode.metadata.edgeUid)
                    } else if (eh.hasTag('BoundaryVertex')) {
                        if (from.parent.nodeId != agent.nodeId) {
                            console.log("got foreign boundary at", from.parent.nodeId)
                            var fromNet = i_ways.objectAt(from.parent.rowIndex)
                            g_net.addBoundary(fromNet.segment(from.metadata.edgeUid), g_net.segment(to.metadata.edgeUid), true)
                        } else {
                            var toNet = i_ways.objectAt(to.parent.rowIndex)
                            g_net.addBoundary(g_net.segment(from.metadata.edgeUid), g_net.segment(to.metadata.edgeUid), false)
                        }


                    } else {
                        console.log("Unhandled edge type", eh.tags)
                    }
                }
            }

            //            Repeater {
            //                parent: buttons_0
            //                model: childNodes
            //                Button {
            //                    text: "remove %1".arg(handle.metadata.edgeUid)
            //                    onClicked: childNodes.remove(index)
            //                }
            //            }
        }

        function getInstance(netName) {
            var h = ways_model.findChild(netName)
            if (!h)
                return
            return objectAt(h.rowIndex)
        }


        function createWay(name, segments, links) {
            var ph = ways_model.rootHandle
            f_Blob.parentHandle = ph
            f_Blob.type = "GuideRoute"
            var itemcolor =  Qt.hsla(Math.random(), 1, 0.8)
            f_Blob.metadata = {
                name: name,
                radius: 15,
                color: itemcolor.toString()
            }

            var wayContainer = f_Blob.create("way_%1".arg(name))
            f_Blob.parentHandle = wayContainer
            segments.forEach(function(s) {
                //{"endPos":86.66914305522819,"startPos":86.66914305522819,"uid":670,"offset":2}
                //                var seg = roadGraph.getHandleByUid(s.uid)
                //                var startNodeHandle = seg.startNode
                //                var endNodeHandle = seg.endNode
                f_Blob.type = "GuideNetSegment"
                f_Blob.metadata = {
                    edgeUid: s.uid,
                    startPos: s.startPos,
                    endPos: s.endPos,
                    offset: s.offset,
                    wires: 1,
                    color: itemcolor.toString(),
                }
                //                var guide = item.source
                f_Blob.data = creatist.serialize(s.guide)
                var segnode = f_Blob.create("seg#%1".arg(s.uid))
            })
            links.forEach(function(link) {
                var linkType = link.type.replace("Type", "Vertex")
                var srcNode = "seg#%1".arg(link.outUid)
                var dstNode = "seg#%1".arg(link.inUid)
                //                console.log("from: %1 to: %2 type: %3".arg(link.outUid).arg(link.inUid).arg(link.type))
                var stnode
                var endnode

                if (linkType == "BoundaryVertex") {
                    console.log(link.inSegment.net.segments, link.inSegment.name, dstNode)
                    stnode = link.inSegment.net.segments.findChild(link.inSegment.name)
                    endnode = wayContainer.findChild(dstNode)
                } else {
                    if (link.outUid == -1)
                        stnode = wayContainer
                    else
                        stnode = wayContainer.findChild(srcNode)

                    if (link.inUid == -1)
                        endnode = wayContainer
                    else
                        endnode = wayContainer.findChild(dstNode)

                }

                console.log("adding edge type %1 from".arg(linkType), stnode.nodeId, "to", endnode.nodeId)
                gmodel.addEdge(stnode, endnode, "GuideLink|%1".arg(linkType))
                //                gmodel.addEdge()



                //                if( i === 0 )
                //                    gmodel.addEdge(wayContainer, segnode, 'GuideLink|StartVertex')
                //                if (prev)
                //                    gmodel.addEdge(prev, segnode, "GuideLink|JointVertex")
                //                if( i === count - 1 )
                //                    gmodel.addEdge(segnode, wayContainer, 'GuideLink|EndVertex')
            })
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

