import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Window 2.2

import Dr.Graphics 1.0
import Dr.UI 1.0
import Dr.MetaGraph 1.0
import Dr.Cartograph 1.0

AAWindow {
    id: mainWindow
    width: 1920
    height: 1000
    x: 0
    y: 0
    title: "Анимированная карта"
    samples: 16
    screenId: 0
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

    Component.onCompleted: {
        showMaximized()
        main_model.init()
    }

    //    Component.onCompleted: showFullScreen()
    SplitView {
        id: root
        anchors.fill: parent
        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
            DrawArea {
                //        PaintBoard {
                id: drawarea
                //            backgroundColor: spview.handle.data.scene.background
                backgroundColor: "transparent"
                grid.visible: false
                anchors.fill: parent
                scaleAnimation.duration: 500
                scene.width: spview.handle.data.contentWidth
                scene.height: spview.handle.data.contentHeight


                Loader {
                    id: gview
                    property GeoView view: item
                    active: spview.linked
                    anchors.fill: parent
                    sourceComponent: GeoView {
                        id: g_view
    //                    parent: drawarea.scene
                        anchors.fill: parent
                        contentWidth: 16000
                        contentHeight: 16000
                        targetSRID: spview.handle.data.targetSRID
                    }
                }

                MouseArea {
                    id: scene_mouse
                    acceptedButtons: Qt.AllButtons
                    anchors.fill: parent
                }

                Scene {
                    id: m_scene
                    enableBsp: true
                    viewport: drawarea
                    width: sceneRect.width
                    height: sceneRect.height
                    sceneRect: Qt.rect(0, 0, 16000, 16000)
                    //                    renderer.renderTarget: SceneRenderer.Image
                    renderer.renderTarget: SceneRenderer.FramebufferObject
                    renderer.z: 1000

                    MarkerItem {
                        id: hover_handle
                    }

                    MarkerItem {
                        id: drag_handle
                        border.color: "#88ffff"
                    }
                }

                Scene {
                    id: m_scene2
//                    enableBsp: true
                    viewport: drawarea
                    width: sceneRect.width
                    height: sceneRect.height
                    sceneRect: Qt.rect(0, 0, 16000, 16000)
                    //                    renderer.renderTarget: SceneRenderer.Image
                    renderer.renderTarget: SceneRenderer.FramebufferObject
                    renderer.z: 1000

                }

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
        Sidebar {
            id: sidebar
            Layout.fillHeight: true
            Layout.minimumWidth: 200
            Layout.preferredWidth: 320
            DataSourcesPanel {
                id: panel_ds
                title: "Источники данных"
                datasourcesModel: model_ds
                datasources: i_ds
            }
            LayersPanel {
                title: "Слои"
                mainModel: main_model
                layersModel: model_layers
                layers: i_layers
                active: i_ds.ready && (i_layers.ready || model_layers.count == 0)
                //                views: i_mapviews
            }




            SidebarPanel {
                id: panel_stuff

                title: "Stuff"
                state: "collapsed"
                ColumnLayout {
                    id: m_sidebar
                    anchors.fill: parent
                    anchors.margins: 8

                    CheckBox {
                        id: cb_bindScale
                        text: "Fixed scale"
                        property real lastScale: 1
                        onCheckedChanged: {
                            if (!checked)
                                lastScale = m_scene.viewportScale
                        }
                    }

                    CheckBox {
                        id: cb_showVertices
                        text: "Display nodes"
                        checked: false
                    }

                    Label { text: "Line width: %1".arg(sl_lineWidth.value) }

                    Slider {
                        id: sl_lineWidth
                        minimumValue: 0
                        maximumValue: 10
                        stepSize: 0.5
                        value: 3
                    }



                    Rectangle {
                        Layout.fillWidth: true
                        height: m_toolPanel.height
                        clip: true
                        Behavior on height {
                            NumberAnimation { easing.type: Easing.OutCubic; duration: 200 }
                        }

                        Loader {
                            id: m_toolPanel
                        }
                    }
                    TextField {
                        id: itemType
                        text: "DummyItem"
                        onAccepted: {
                            focus = false
                        }
                    }

                    property var handle

                    ColorDialog {
                        id: cdial
                        onCurrentColorChanged: {
                            m_sidebar.handle.color = currentColor
                        }
                    }
                    Button {
                        id: addItemButton
                        text: "Add item %1".arg(itemType.text)
                        onClicked: {
                            m_sidebar.handle = m_sceneBuilder.addItem(itemType.text,{ flags: 3, x: 350, y: 800 })
                            m_sidebar.handle.color = "#FFDD00"
                            cdial.color = m_sidebar.handle.color
                            cdial.open()
                        }
                    }

                }
            }
            SidebarPanel {
                id: panel_polygon
                title: "Build polygon"
                contentHeight: 400
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    //                    Layout.fillWidth: true
                    Label { text: "Curve precision: %1".arg(sl_precision.value) }

                    Slider {
                        id: sl_precision
                        minimumValue: 1
                        maximumValue: 60
                        //                stepSize:
                        value: 10
                    }

                    Label { text: "Radius: %1".arg(radiusSlider.value) }

                    Slider {
                        id: radiusSlider
                        minimumValue: 0
                        maximumValue: 150
                        value: 40
                        stepSize: 1
                    }

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
                                var col = Qt.hsla(Math.random(), 1, 0.7)
                                phandle.polygon = polfactory.polygon
                                //                                phandle.polygon = smoother.output
                                phandle.pen.color = col
                                phandle.pen.cosmetic = true
                                phandle.pen.width = 3
                                phandle.shapeType = 1
                                phandle.hoverEnabled = true
                                phandle.onMousePress = onClick
                                phandle.onHoverEnter = onHoverEnter
                                phandle.onHoverLeave = onHoverLeave
                                phandle.bind(VisualInstance.ItemPositionChange, itemPosChange)
                                polmodel.clear()
                                polfactory.buildPolygon()
                                smoother.currentAxisItem.update()
                                smoother.currentItem.update()
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

                    PolygonSmoother {
                        id: smoother
                        input: polfactory.polygon
                        //                onInputChanged: console.log("input updated")
                        property var currentItem
                        property color pcolor: "#888"
                        property var currentAxisItem
                        onCurrentAxisItemChanged: currentAxisItem.pen.color = pcolor
                        onAxisChanged: if (currentAxisItem) {
                                           currentAxisItem.polygon = axis
                                       }
                        radius: radiusSlider.value
                        offset: 0
                        precision: sl_precision.value / 10
                        onOutputChanged: {
                            if (!currentItem) {
                                currentItem = m_sceneBuilder.addItem("PolygonItem")
                                currentItem.pen.cosmetic = true
                                currentItem.shapeType = 1
                                currentItem.pen.color = "#ff0030"
                                currentItem.pen.width = 4
                            }
                            currentItem.polygon = output
                        }

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
                                currentItem.pen.width = 2

                                currentItem.shapeType = 1
                                currentItem.pen.color = "#fff"
                                currentItem.pen.strokeStyle = Qt.DashLine


                            }
                            currentItem.polygon = polygon
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
                                    if (!smoother.currentAxisItem) {
                                        smoother.currentAxisItem = m_sceneBuilder.addItem("PolygonItem")
                                        smoother.currentAxisItem.pen.cosmetic = true
                                        smoother.currentAxisItem.shapeType = 1
                                    }
                                    smoother.currentAxisItem.polygon = smoother.axis
                                }
                            }
                        }
                    ]
                }
            }
        }
    }



    //    TableView {
    //        id: t_route
    //        Layout.fillWidth: true
    //        Layout.minimumHeight: 240
    //        model: route
    //        TableViewColumn {
    //            title: "Edge"
    //            role: "edge"
    //            width: 60
    //            delegate: Label {
    //                color: styleData.textColor
    //                text: styleData.value
    //            }
    //        }
    //        TableViewColumn {
    //            title: "Begin"
    //            role: "startPos"
    //            width: 50
    //            delegate: Label {
    //                color: styleData.textColor
    //                text: Number(styleData.value).toFixed(2)
    //            }
    //        }
    //        TableViewColumn {
    //            title: "End"
    //            role: "endPos"
    //            width: 50
    //            delegate: Label {
    //                color: styleData.textColor
    //                text: Number(styleData.value).toFixed(2)
    //            }
    //        }
    //    }



    GraphModel {
        id: gmodel
        property url file
        property string fileName: file.toString().length ? file.toString().match(/(\w+)\..+$/)[1] : "untitled"
        RootNodeFactory { }
        RootNodeFactory { id: f_RootNode }
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
        NodeChildrenModel {
            id: gr_nodes
            mrl: "/spview/road_graph"
            onLinkedChanged: console.log("graph contains", count, "nodes")

        }
    }

    EdgeModel {
        id: edgeModel
        model: gmodel
        tag: 'polygon'
        onItemAdded: {
            var eh = getHandle(index)
            var p = creatist.create("GuidePath")
            var ed = eh.getData()
            p.setPolygon(ed.polygon)
            p.setHeadData(index)
            p.setTailData(index)
            p.setPathData(ed.attributes)
            var i = m_sceneBuilder.addItem("GuidePathStroke",
                                           {
                                               smooth: 0,
                                               precision: 0,
                                               source: p,
                                               flags: VisualInstance.ItemIsSelectable | VisualInstance.NoSelectionOutline,
                                               //                                               cacheMode: 1
                                           })
            i.pen.width = 0
            ed.instance = i
            eh.setData(ed)
            i.pen.color = "#80ffff"
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

        property var edge2model: []
        property int currentEdge
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


        function appendSegment(edgeIndex, runner, entry)
        {
            var i = edgeModel.edgeData(edgeIndex).instance
            if (entry === undefined)
                i.strokeStart = runner.position
            else
                i.strokeStart = entry * i.source.length
            append({
                       edge: edgeIndex,
                       startPos: i.strokeStart,
                       endPos: runner.position,
                       startX: runner.point.x,
                       startY: runner.point.y,
                       endX: runner.point.x,
                       endY: runner.point.y,
                       len: i.source.length
                   })
            edge2model[edgeIndex] = count - 1
            i.drawResult = true
            i.strokeEnd = runner.position
            currentEdge = edgeIndex
            console.log("added segment %1 (edge #%2), entry pos: %3, current pos: %4".arg(count - 1).arg(edgeIndex).arg(i.strokeStart).arg(i.strokeEnd), runner.position)
        }

        function updateEndPos(runner) {
            if (edge2model[currentEdge] === undefined) {
                console.log("edge", edgeIndex, "unknown")
                return
            }
            var endpos
            if (runner.atEnd)
                endpos = edgeModel.getHandle(currentEdge).getData().instance.source.length
            set(edge2model[currentEdge], {
                    endPos: runner.position,
                    endX: runner.point.x,
                    endY: runner.point.y
                })
            var i = edgeModel.edgeData(currentEdge).instance
            i.drawResult = true
            i.strokeEnd = runner.position
        }

        function findCandidateEdges(node, runner) {
            candidateEdges = []
            node.outEdges("polygon").forEach(function(e){
                var d = e.getData()
                if (d.instance.source.headData() == currentEdge)
                    return
                candidateEdges.push( { edge: d.instance.source.headData(), pos: 0 })
            })
            node.inEdges("polygon").forEach(function(e){
                var d = e.getData()
                if (d.instance.source.headData() == currentEdge)
                    return
                candidateEdges.push( { edge: d.instance.source.headData(), pos: 1 })
            })
            console.log("candidates", JSON.stringify(candidateEdges), "Current:", JSON.stringify({edge: currentEdge, pos: runner.position/edgeModel.getHandle(currentEdge).getData().instance.source.length }))
        }

        function distance(p1, p2) {
            return Qt.vector2d(p2.x - p1.x, p2.y - p1.y).length()
        }


        function switchCurrentEdge(edgeIndex, runner, entry) {
            var inst = edgeModel.getHandle(currentEdge).getData().instance
            inst.strokeEnd = inst.strokeStart
            remove(edge2model[currentEdge])
            edge2model[currentEdge] = undefined
            if (edge2model[edgeIndex]) {
                console.log("move to already visited edge", edge2model[edgeIndex], count, get(count-1).startPos, get(count-1).endPos)
                currentEdge = edgeIndex
                updateEndPos(runner)
            } else
                appendSegment(edgeIndex, runner, entry)
        }


        function findCandidate(desiredPos) {
            var nextEdge = -1
            var dist = 1e38
            var runner
            var entry
            candidateEdges.forEach(function(i) {
                var e = edgeModel.edgeData(i.edge)
                var r = e.instance.source.findNearestPos(desiredPos)
                var d = distance(desiredPos, r.point)
                if (d < dist) {
                    nextEdge = i.edge
                    runner = r
                    dist = d
                    entry = i.pos
                }
            })
            if (nextEdge < 0) {
                var e = edgeModel.edgeData(currentEdge)
                var r = e.instance.source.findNearestPos(desiredPos)
                console.log("no available candidate edge for position", desiredPos, get(count - 1).startPos)
                return { edge: currentEdge, runner: r, entry: get(count - 1).startPos }
            }
            if (edge2model[nextEdge] != undefined && edge2model[nextEdge] - 1 > count) {
                console.warn("move back more than one step", edge2model[nextEdge] - 1 , count)
            }
            return { edge: nextEdge, runner: runner, entry: entry }
        }

        function onMousePress(event) {
            var r = this.source.findNearestPos(event.pos)
            candidateEdges.push({ edge: this.source.headData(), pos: r.position / this.result.length })
            appendSegment(this.source.headData(), r)
            drag_handle.visible = true
            drag_handle.runner = r
            drag_handle.updateHandle()
        }

        function onMouseMove(event) {
            var c = findCandidate(event.pos)
            drag_handle.runner = c.runner
            drag_handle.updateHandle()
            if (c.edge == currentEdge) {
                updateEndPos(c.runner)
            } else {
                switchCurrentEdge(c.edge, c.runner, c.entry)
            }

            var edgeId = c.edge
            var eh = edgeModel.getHandle(edgeId)

            if (c.runner.atEnd) {
                findCandidateEdges(eh.endNode, c.runner)
            } else if (c.runner.atStart) {
                findCandidateEdges(eh.startNode, c.runner)
            } else
                return

            c = findCandidate(event.pos)
            if (edge2model[c.edge] !== undefined) {
                console.log("edge %1 already is in the route model".arg(c.edge))
                switchCurrentEdge(c.edge, c.runner, c.entry)
            } else {
                console.log("adding hop %1".arg(c.edge))
                appendSegment(c.edge, c.runner, c.entry)
            }

        }

        function onMouseRelease(event) {
            drag_handle.visible = false
            buildPath()
        }

        function buildPath() {
            var path = creatist.create("GuidePath")
            for (var i = 0; i < count; ++i) {
                var s = get(i)

                console.log("adding segment", s.edge, "\tentry", s.startPos / s.len, "\texit", s.endPos / s.len)
                var seg = edgeModel.getHandle(s.edge)
                var startNodeHandle = seg.startNode
                var endNodeHandle = seg.endNode
                var item = seg.getData().instance
                item.drawResult = false
                item.strokeEnd = -1
                item.strokeStart = 0
                if (i != 0 && i != count - 1) {
                    if (s.startPos > s.endPos)
                        path.appendPath(item.source.reversed())
                    else
                        path.appendPath(item.source)
                } else {
                    var partPath = item.source.part(s.startPos, s.endPos)
                    path.appendPath(partPath)

                }

            }
            clear()
            edge2model = []
            currentEdge = -1
            candidateEdges = []
            var iseg = m_sceneBuilder.addItem("GuidePathStroke",
                                              {
                                                  smooth: 3,
                                                  offset: 1,
                                                  fast: true,
                                                  precision: 10,
                                                  source: path,
                                                  flags: VisualInstance.ItemIsSelectable
                                                  //                                               cacheMode: 1
                                              })
            iseg.pen.color = Qt.hsla(Math.random(), 0.9, 0.5)
            iseg.pen.width = 2
            iseg.pen.cosmetic = true
            iseg.drawResult = true

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

    SceneModel {
        id: m_sceneBuilder
        scene: m_scene.qGraphicsScene
    }

    ListModel {
        id: polmodel
    }


    GraphModel {
        id: main_model
        property url file
        property string fileName: file.toString().length ? file.toString().match(/(\w+)\..+$/)[1] : "untitled"
        property bool changed
        onDataChanged: changed = true
        //        onModelRootChanged: console.log(modelRoot)
        DataNodeFactory { id: fDataNode }
        NodeFactory { id: fNode }
        MetaNodeFactory { id: fMetaNode }
        CollectionFactory { id: fCollection }
        TypeBuilder { id: builder }
        BlobFactory { id: fBlob }
        PointNodeFactory{ id: fPoint }
        RootNodeFactory { }

        NodeChildrenModel {
            id: model_ds
            mrl: "spview/datasources"
            onLinkedChanged: console.log("ds linked", linked)
        }

        NodeChildrenModel {
            id: model_layers
            mrl: "spview/layers"
        }

        ModelWatcher {
            id: spview
            mrl: "spview"
            onLinkedChanged: console.log("Spview linked", linked)
        }

        NodeChildrenModel {
            id: cache_model
            mrl: "spview/cache"
        }

        NodeChildrenModel {
            id: styles_model
            //            mrl: "spview/styles"
            mrl: "spview/styles.test"
        }

        function init() {
            loadModel("qml/spv_base.mgf")
            fNode.parentHandle = getHandle("/spview")
            fNode.create("datasources")
            fNode.create("layers")
            fNode.create("cache")

        }
    }

    Creatist {
        id: creatist
    }
    AuxModel {
        id: aux
    }

    ModelWeaver {
        id: i_ds
        function getDB(handle) {
            if (!handle)
                return null
            return objectAt(handle.rowIndex)
        }
        model: model_ds
        property int pendingSize    ///< Количество создаваемых в данный момет делегатов
        onObjectAdded: pendingSize++

        SpatiaLiteDB {
            database: handle.data.url
            onReadyChanged: {
                var dh = handle.dataHandle
                console.log(dh)
                if(isOpen !== dh.isOpen)
                    dh.isOpen = isOpen
                if(ready) {
                    i_ds.pendingSize--
                    console.log("ds arrived", dh)
                }
                else
                    i_ds.pendingSize++
            }

        }
    }

    ModelWeaver {
        id: i_layers
        model: model_layers
        active: i_ds.ready && i_ds.pendingSize === 0 && spview.linked
        delegate: GeoLayer {
            id: g_layer
            dataSource: i_ds.getDB(handle.outNodes("db")[0])
            table: handle.data.tableName
            parent: gview.item
            anchors.fill: parent
            property alias iGeomBuffer: i_geoms
            property NodeChildrenModel geomBuffers: childrenModel

            ModelWeaver {
                id: i_geoms
                active: i_layers.ready && g_layer.isValid
                model: g_layer.geomBuffers
                GeomBuffer {
                    id: g_buf
                    layer: g_layer
                    property var node: handle
                    property var attrs: handle.data.attrlist ? handle.data.attrlist : ""
                    clause: handle.data.where
                    Component.onCompleted: loadData(attrs)
                    onReadyChanged: applyStyle()

                    function applyStyle() {
                        if(ready) {
                            var styleList = node.outNodes("style")
                            if (styleList.length === 0)
                                return
                            var s = styleList[0]
                            style = i_styles.instance(s.modelIndex)
                        }
                    }

                    function loadData(attrs) {
                        if (attrs.length > 0)
                            var attrlist = attrs.split(" ")
                        else
                            attrlist = []
                        var cache = node.outNodes("cache")
                        if (cache.length === 0) {
                            fetchGeometry(attrlist)
                        } else {
                            console.log("loading cache for", clause, cache[0])
                            loadCache(cache[0].data)
                        }
                    }
                }
            }
        }
    }

    Weaver {
        id: i_styles
        index: styles_model.rootIndex
        typeRole: "blob$type"
        valueRole: "blob$metadata"
        onObjectAdded: {
//            console.log("Added style", object, "parent", parent, index)
            if (parent)
                parent.insertChild(object, index)
        }
        onObjectRemoved: {
            console.log("Removed style", object, "parent", parent, index)
            object.deleted = true

        }
        onValueChanged: {
            var handle = main_model.getHandle(index)
            if( handle.type != 'Style' ){
                var l = i_layers.objectAt(handle.parent.inNodes()[0].parent.rowIndex)
            } else {
                var l = i_layers.objectAt(handle.inNodes()[0].parent.rowIndex)
            }
            l.update()
            console.log("value changed", handle, instance, l)
        }
    }

}

