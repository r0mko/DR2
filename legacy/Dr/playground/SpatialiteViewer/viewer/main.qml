import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import Dr.Cartograph 1.0
import Dr.MetaGraph 1.0
import QtQml.Models 2.2
import Dr.Graphics 1.0
import Dr.UI 1.0
import Dr.T2C 1.1
//import Dr.Paint 1.0

AAWindow {
    id: appWin
    title: qsTr("%1 - GISView".arg(main_model.fileName))
    color: "#fff"

    samples: 4
    Component.onCompleted: {
        showMaximized()
        main_model.init()
    }

    property var panels_model: [
        { title: "Источники данных", source: "qrc:/DataSourcesView.qml", checked: true },
        { title: "Слои", source: "qrc:/LayersView.qml", checked: true },
        { title: "Стили", source: "qrc:/StylesView.qml", checked: true }
    ]
    MenuBar {
        Menu {
            title: "Файл"
            MenuItem { action: a_openModel }
            MenuItem { action: a_saveAs }
            MenuItem { action: a_save }
            MenuItem { action: a_reloadModel }

        }
        Menu {
            id: styles_menu
            title: "Параметры"
            MenuItem { action: a_openStyleEditor }
            MenuItem { action: a_createStylesFromLayer }
            MenuItem {
                action: a_openSettings
                visible: true
                text: "Настройки"
            }

        }
        Menu {
            id: panels_menu
            title: "Панели"
            Instantiator {
                id: inst
                model: panels_model
                MenuItem {
                    checkable: true
                    checked: modelData.checked
                    text: modelData.title
                    onToggled: inst.model[index].checked = checked
                }
                onObjectAdded: panels_menu.insertItem(index, object)
                onObjectRemoved: panels_menu.removeItem(object)
            }
        }
        Menu {
            title: 'Debug'
            MenuItem {
                text: 'ControlStar'
                onTriggered: controlStar.show()
            }
        }
    }

    SplitView {
        id: root
        anchors.fill: parent
        property point globalPos: Qt.point(appWin.x, appWin.y)
        //        onGlobalPosChanged: console.log("globalPos", globalPos)
        DrawArea {
//        PaintBoard {
            id: drawarea
            //            backgroundColor: spview.handle.data.scene.background
            backgroundColor: "#fff"
            grid.visible: false
            //                anchors.fill: parent
            scaleAnimation.duration: 500
            Layout.fillHeight: true
            Layout.fillWidth: true
            scene.width: spview.handle.data.contentWidth
            scene.height: spview.handle.data.contentHeight




            Scene {
                id: m_scene
//                visible: false
                enableBsp: true
                viewport: drawarea
                width: sceneRect.width
                height: sceneRect.height
                sceneRect: Qt.rect(0, 0, 16000, 16000)
//                renderer.renderTarget: SceneRenderer.Image
                renderer.renderTarget: SceneRenderer.FramebufferObject
                renderer.z: 1000
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
            }


            MarkerItem {
                id: hover_handle
            }

            MarkerItem {
                id: drag_handle
                border.color: "#88ffff"
            }

        }

        Sidebar {
            id: sidebar
            Layout.fillHeight: true
            Layout.minimumWidth: 200
            Layout.preferredWidth: 320
            //            Layout.maximumWidth:  320

            DataSourcesPanel {
                id: panel_ds
                title: "Источники данных"
                datasourcesModel: model_ds
                datasources: i_ds
            }

            LayersPanel {
                id: panel_layers
                title: "Слои"
                mainModel: main_model
                layersModel: model_layers
                layers: i_layers
                active: i_ds.ready && (i_layers.ready || model_layers.count == 0)
                //                views: i_mapviews
            }

            SidebarPanel {
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    Button {
                        text: "Clear"
                        onClicked: edgeModel.clear()
                    }

                }
            }

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
        enabled: main_model.file.toString().length > 0
        onTriggered: {
            main_model.loadModel(main_model.file)
        }
    }

    Action {
        id: a_save
        text: "Сохранить %1".arg(main_model.file)
        enabled: main_model.file.toString().length > 0
        shortcut: "Ctrl+S"
        onTriggered: main_model.exportModel(main_model.file, main_model.root)
    }

    Action {
        id: a_openStyleEditor
        text: "Редактор стилей"
        enabled: !style_editor_loader.active
        onTriggered: style_editor_loader.active = true
    }
    Action {
        id: a_createStylesFromLayer
        text: "Создать стили из слоя..."
        enabled: !style_editor_loader.active
        onTriggered: load_styles_loader.active = true
    }
    Action {
        id: a_openSettings
//        enabled: spview.linked
        onTriggered: dia_conf.show()
    }

    FileDialog {
        id: fileselector
        selectExisting: true
        //        modality: Qt.NonModal
        folder: shortcuts.desktop
        nameFilters: ["MetaGraph model files (*.mgf)"]
        onAccepted: {
            main_model.file = fileUrl
            //            console.log("loading model", fileUrl.toString())
            main_model.loadModel(fileUrl)
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
            main_model.file = fileUrl
            main_model.exportModel(fileUrl, main_model.rootIndex)
        }
    }

    SystemPalette { id: palette }

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
        }

        NodeChildrenModel {
            id: cache_model
            mrl: "spview/cache"
        }

        NodeChildrenModel {
            id: styles_model
//            mrl: "spview/styles"
            mrl: "spview/styles.test"
            //            onRowsInserted: console.log("inserted style", first)
            //            onCountChanged: console.log("style count", count)
            //            onModelReset: console.log("styles was reset", count)
        }
        NodeChildrenModel {
            id: ways_model
            mrl: "/ways"
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
            function setcolor( idf, power){
                for (var i = 0; i < count; ++i) {
                    var h = ways_model.getHandle(i)
                    if( h.metadata.idf == idf ) {
                        console.log( 'IDF', idf, power )
                        var col = 'black'
                        if( idf == 465188 ) col = '#fcd82a'
                        if( idf == 465195 ) col = '#dd2932'
                        if( idf == 465202 ) col = '#f7eb39'
                        if( idf == 465209 ) col = '#1d35e4'
                        if( idf == 465271 ) col = '#feba41'
                        if( idf == 465278 ) col = '#2f83be'

                        h.agent.blob$metadataHandle.color = power ? col : '#bbb'
                        updateWay(i)
                    }
                }
            }

            function buildWay(index) {
                var h = ways_model.getHandle(index)
                var idf = h.metadata.idf
                manager.subscribe(idf)
                var i = m_sceneBuilder.addItem("GuidePathStroke", {
                                                   smooth: 15,
                                                   offset: 0,
                                                   precision: 1,
                                                   flags: VisualInstance.ItemIsSelectable | VisualInstance.NoSelectionOutline
                                               } )
                i.pen.width = 3
                i.pen.color = h.metadata.color
                i.pen.cosmetic = true

//                i.onHoverEnter = onHoverEnter
//                i.onHoverLeave = onHoverLeave
//                i.onHoverMove = onHoverMove
//                i.hoverEnabled = true
                var guide = creatist.deserialize("GuidePath", h.data)
                i.source  = guide
                i.source.setPathData(h.nodeId)
                idx2inst[index] = i
                if (h.metadata.label) {
                    var t = m_sceneBuilder.addItem("GuidePathText")
                    t.path = guide
                    t.orientation = Qt.Horizontal
                    t.position = guide.length / 2
                    t.text = h.metadata.label
                }
            }
            function updateWay(index) {
                if (!idx2inst[index])
                    buildWay(index)
                var h = ways_model.getHandle(index)
                var i = idx2inst[index]
                i.pen.color = h.metadata.color
                var guide = creatist.deserialize("GuidePath", h.data)
                guide.setPathData(h.nodeId)
                console.log( 'setting names', h.nodeId)
                i.source  = guide
                i.source.setPathData(h.nodeId)
                console.log("updated way", h, i, i.source, h.type, guide, h.size, h.type)

            }
            function onHoverEnter(event) {
                hover_handle.runner = this.result.findNearestPos(event.pos)
                hover_handle.visible = true
                hover_handle.updateHandle(this.source.pathData())
            }

            function onHoverMove(event) {
                hover_handle.runner = this.result.findNearestPos(event.pos)
                hover_handle.updateHandle(this.source.pathData())
            }

            function onHoverLeave(event) {
                hover_handle.visible = false
            }

            onRowsInserted: {
                buildWay(first)
//                console.log("inserted way", getHandle(first).childCount)
            }
            onDataChanged: {
                var h = getHandle(topLeft)
//                console.log("data change at row", topLeft.row, getHandle(topLeft.row))
                updateWay(topLeft.row)
            }
        }

        function init() {
            loadModel("viewer/spv_base.mgf")
            fNode.parentHandle = getHandle("/spview")
            fNode.create("datasources")
            fNode.create("layers")
            fNode.create("cache")
            fNode.create("ways")
        }
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
            id: i_database
            database: handle.data.url
            onReadyChanged: {
                var dh = handle.dataHandle
                console.log(dh)
                if(isOpen !== dh.isOpen)
                    dh.isOpen = isOpen
                if(ready)
                    i_ds.pendingSize--
                else
                    i_ds.pendingSize++
            }
        }

//        SpatialQuery {
//            query: "select geom from adresses where nam_str1 like '%Борьбы%' and number1=13 and lit_n1='Б'"
//            database: i_database
//            onCountChanged: {

//            }

////            onComplete:
//        }
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
            console.log("Added style", object, "parent", parent, index)
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
    Loader {
        id: style_editor_loader
        active: false

        sourceComponent: StyleEditor {
            Component.onCompleted: show()
            stylesModel: styles_model
            geoView: gview.item
            onClosing: style_editor_loader.active = false

        }
    }


    Loader {
        id: load_styles_loader
        active: false

        sourceComponent: Window {
            width: 400
            height: 400
            onClosing: load_styles_loader.active = false
            Component.onCompleted: show()
            color: "#eee"
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 8
                NodeChildrenModel {
                    id: buffers_model
                    rootIndex: model_layers.get(select_layer.currentIndex).handle.modelIndex
                }
                RowLayout {
                    Label {
                        text: "Слой"
                    }
                    ComboBox {
                        id: select_layer
                        model: model_layers
                        textRole: "nodeId"
                    }
                }

                TableView {
                    id: table_geoms
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    frameVisible: true
                    selectionMode: SelectionMode.ExtendedSelection
                    model: buffers_model
                    rowDelegate: Rectangle {
                        height: 25
                        color: styleData.selected ? "#8000aaff" : "transparent"
                    }

                    TableViewColumn {
                        title: "Геометрия"
                        role: "nodeId"
                        delegate: Label {
                            text: JSON.stringify(styleData.value)
                            color: styleData.textColor

                        }
                    }
                }

                RowLayout {
                    Label {
                        text: "Geometry kind: %1".arg(buffers_model.rootHandle.data.geometryKind)
                    }

                    Button {
                        id: btn_ok
                        text: "Добавить"
                        enabled: buffers_model.count > 0
                        onClicked: {
//                            fBlob.parentHandle = styles_model.rootHandle
                            /**
                            switch (buffers_model.rootHandle.data.geometryKind) {
                            case 1:
                                return;
                            case 2:
                                fBlob.type = "CosmeticStroke"
                                fBlob.metadata = { width: 1 }
                                break;
                            case 3:
                                fBlob.type = "SolidColorFill"
                                fBlob.metadata = { }
                                break;
                            }
                            /**/

                            for (var i = 0; i < buffers_model.count; ++i) {
                                var h = buffers_model.getHandle(i)
                                if (h.nodeId.length === 0) continue


                                var s
                                var geom = i_layers.objectAt(h.parent.rowIndex).iGeomBuffer.objectAt(h.rowIndex)
                                if (!styles_model.hasChild(h.nodeId)) { // what if we don't have landusages:parks yet?
                                                                        console.log("Adding style for", geom.clause, h.nodeId, geom.styleNodes)


                                    fBlob.parentHandle = styles_model.rootHandle
                                    fBlob.type = 'Style'
                                    fBlob.metadata = { opacity: 1.0 }//0.6 + Math.random() * 0.15 }
                                    var rootStyleNode = fBlob.create( h.nodeId )

                                    fBlob.parentHandle = rootStyleNode
                                    switch (buffers_model.rootHandle.data.geometryKind) {
                                    case 2:
                                        fBlob.type = "CosmeticStroke"
                                        fBlob.metadata = { width: 1 }
                                        break;
                                    case 3:
                                        fBlob.type = "SolidColorFill"
                                        fBlob.metadata = {  }
                                        break;
                                    }


                                    var col = Qt.hsla(Math.random(), 0.5, 0.7, 0.5)
                                    var md = fBlob.metadata
                                    md.color = col.toString()
                                    fBlob.metadata = md
                                    console.log(JSON.stringify(md))

                                    fBlob.create(fBlob.type)

                                    var randomStroke = true
                                    if( fBlob.type == "SolidColorFill" && Math.random() > 0.5 && randomStroke ) {
                                        fBlob.parentHandle = rootStyleNode
                                        fBlob.type = "CosmeticStroke"
                                        fBlob.metadata = {
                                            width: 1,
                                            color: Qt.hsla(Math.random(), 0.5, 0.7, 0.5).toString()
                                        }
                                        fBlob.create(fBlob.type)
                                    }

                                }
                                else { // what if we have landusages:parks already?
//                                    s = styles_model.getHandle(h.nodeId)
                                    rootStyleNode = styles_model.getHandle(h.nodeId)
                                }

//                                main_model.addEdge(h, s, "style")
                                main_model.addEdge(h, rootStyleNode, "style")
                                geom.applyStyle()
                                //                                geom.clearStyles()
                                //                                geom.addStyle(s.metadata.type, s)

                            }

                        }
                    }
                }


            }

        }
    }
    SettingsDialog {
        id: dia_conf
    }

    AuxModel {
        id: aux
    }

    EdgeModel {
        id: edgeModel
        model: main_model
        tag: 'polygon'
        property var edgeUid2item: []
        onModelAboutToBeReset: {
            edgeUid2item.forEach(function (e) { e.destroy(); })
        }

        onItemAdded: {
//            return
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
                                               flags: VisualInstance.ItemIsSelectable | VisualInstance.NoSelectionOutline /*| VisualInstance.ItemHasNoContents*/,
                                               //                                               cacheMode: 1
                                           })
            edgeUid2item[uid] = i
            i.pen.width = 0
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

        }

        function onMouseRelease(event) {
            drag_handle.visible = false
            buildPath()

        }

        function buildPath() {
            var path = creatist.create("GuidePath")

            var ph = ways_model.rootHandle
            if (!ways_model.linked) {
                fNode.parentHandle = ways_model.rootHandle
                ph = fNode.create("ways")
            }
            fBlob.parentHandle = ph
            fBlob.type = "GuideRoute"
            var itemcolor =  Qt.hsla(Math.random(), 0.7, 0.3)
            fBlob.metadata = { name: "untitled", color: itemcolor.toString() }
            var wayContainer = fBlob.create("way%1".arg(ph.childCount))
            fBlob.parentHandle = wayContainer
            var prev
//            var path

            for (var i = 0; i < count; ++i) {
                var s = get(i)
                fBlob.type = "GuideRouteElement"
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
                fBlob.metadata = {
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
                fBlob.data = creatist.serialize(guide)
                var segnode = fBlob.create("seg#%1".arg(s.uid))
                if (prev)
                    main_model.addEdge(prev, segnode, "GuideLink")
                prev = segnode

                main_model.addEdge(segnode, startNodeHandle, "startnode")
                main_model.addEdge(segnode, endNodeHandle, "endNode")

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
    SceneModel {
        id: m_sceneBuilder
        scene: m_scene.qGraphicsScene
    }


    Creatist {
        id: creatist
    }
    T2CManager {
        id: manager
        host: '192.168.1.78'
        port: 47001
//        Component.onCompleted: start()
        onGotMessage: {
            console.log( message )
            var regexp = /DAT 0 (\d+) B32 (0x\S+) (\S+) (\d+\.\d+)/
            var match  = regexp.exec(message)
            if( match != null ) {
                var v = Number(match[2]) & 15
                var powered = v == 6 || v == 7 || v == 8 || v == 11 || v == 12
                var idf = match[1]
                var status = match[3]
                var time = match[4]
                console.log( message, ' | ', idf, powered, status, time );
                ways_model.setcolor(idf, powered)
            }


        }
    }
    ControlStar {
        id: controlStar
        visible: false
    }
}
