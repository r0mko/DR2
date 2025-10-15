import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.2
import QtQuick.Dialogs 1.2
import Dr.Cartograph 1.0
import Dr.MetaGraph 1.0
import QtQml.Models 2.2
import Dr.Graphics 1.0
import Dr.UI 1.0

import Dr.QRPC 1.0

Rectangle {
    id: root

    property alias sa: drawarea.scrollArea
    property alias ls: drawarea.scale
    property alias da: drawarea
    property alias engine: engine
    property alias marker: marker


    property var idfToData: {
        465188: { color: '#fcd82a', visible: false },
        465195: { color: '#dd2932', visible: false },
        465202: { color: '#f7eb39', visible: false },
        465209: { color: '#1d35e4', visible: false },
        465271: { color: '#feba41', visible: false },
        465278: { color: '#2f83be', visible: false }
    }

    property var guides: []
    property alias testobj: testobject


    property var ways: null
    property alias myways: ways_model

    RPCEngine {
        id: engine
        nodeName: '<MGT>'
        networkMessenger {
            id: netman
            listening: true
        }
    }
//    RemotePeer {
//        host: "10.0.0.62"
//        port: 42001
//    }

    Rectangle {
        anchors.fill: parent
        DrawArea {
            id: drawarea
            anchors.fill: parent
            backgroundColor: "#fff"
            grid.visible: false
            scaleAnimation.duration: 500
            scene.width: spview.handle.data.contentWidth
            scene.height: spview.handle.data.contentHeight

            MouseArea {
                id: scene_mouse
                acceptedButtons: Qt.AllButtons
                anchors.fill: parent
                onClicked: console.log( 'mouse click', mouse.x, mouse.y )
            }
            Scene {
                id: m_scene
                enableBsp: true
                viewport: drawarea
                width: sceneRect.width
                height: sceneRect.height
                sceneRect: Qt.rect(0, 0, 16000, 16000)
                renderer.renderTarget: SceneRenderer.FramebufferObject
                renderer.z: 1000
                Loader {
                    id: gview
                    property GeoView view: item
                    active: spview.linked
                    anchors.fill: parent
                    sourceComponent: GeoView {
                        id: g_view
                        anchors.fill: parent
                        contentWidth: 16000
                        contentHeight: 16000
                        targetSRID: spview.handle.data.targetSRID
                    }
                }
            }

            Item {
                id: testobject
                property int idx: 0
                property real rot: 0
                property var currentGuide: root.guides[idx] === undefined ? null : root.guides[idx]
                onCurrentGuideChanged: console.log( 'currentGuideChanged' )
                onIdxChanged: console.log( 'idx changed', idx )
                x: currentGuide === null ? 0 : currentGuide.findNearestPos(Qt.point(poster.x, poster.y)).point.x
                y: currentGuide === null ? 0 : currentGuide.findNearestPos(Qt.point(poster.x, poster.y)).point.y
                onXChanged: {
                    pathitem.x = x
                    pathitem.y = y
                    pathitem.rotation = rot
                }
                onYChanged: {
                    pathitem.x = x
                    pathitem.y = y
                    pathitem.rotation = rot
                }
                onRotChanged: pathitem.rotation = rot

                //            border.width: 0.05
                //            border.color: '#40444444'
                width: 0.25
                height: 0.25
                transform: [
                    Translate {
                        x: -root.testobj.width * 0.5
                        y: -root.testobj.height * 0.5
                    },
                    Rotation {
                        angle: testobject.rot
                    }

                ]
                Rectangle {
                    anchors.centerIn: parent
                    width: 0.23
                    height: 0.50
                    color: '#ccccdd'
                }
                property var pathitem
                Component.onCompleted: {
                    var path = creatist.create('PainterPath')
                    path.moveTo(   -1, -1 )
                    path.lineTo( -0.5, -1 )
                    path.lineTo( -0.5,  1 )
                    path.lineTo(   -1,  1 )
                    path.moveTo(    1, -1 )
                    path.lineTo(  0.5, -1 )
                    path.lineTo(  0.5,  1 )
                    path.lineTo(    1,  1 )

                    pathitem = m_sceneBuilder.addItem("PathItem")
                    pathitem.path = path
                    pathitem.pen.color = '#000000'
                    pathitem.pen.width = 0.2
                    pathitem.scale = 0.25
                    pathitem.z = 90
                    //                pathitem.pen.cosmetic = true
                }

            }

            Rectangle {
                id: marker
                property alias text: sometext.text
                visible: false
                width: 50 / drawarea.scale
                height: 50 / drawarea.scale
                opacity: drawarea.scale > 0.8 ? 0.8 : drawarea.scale
                color: 'blue'
                onXChanged: {
                    var r = root.guides[1].findNearestPos(Qt.point(x, y))
                    testobject.x =  r.point.x
                    testobject.y =  r.point.y
                    testobject.rot = r.angle
                }
                onYChanged: {
                    var r = root.guides[1].findNearestPos(Qt.point(x, y))
                    testobject.x =  r.point.x
                    testobject.y =  r.point.y
                    testobject.rot = r.angle
                }
                transform: [
                    Translate{
                        x: -marker.width*0.5
                        y: -marker.height*0.5
                    }
                ]

                MouseArea {
                    anchors.fill: parent
                    drag.target: parent
                    drag.threshold: 0
                    onClicked: markerInstantiator.model.remove(index)
                }
                Text{
                    id: sometext
                    anchors.fill: parent
                    color: 'black'
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    scale: 1.0 / drawarea.scale
                }
            }


        }

        TextLayer {
            id: textLayer
            anchors.fill: parent
            font.family: 'Roboto'
            transform: Translate {
                // FIXME: that is a problem - we have to fix viewportTransform
                x: -r2.width - 30
                y: -79
            }
            spacing: 20
            depth: 12
            color: '#333'
            viewTransform: m_scene.viewTransform
            viewportRect: m_scene.viewportRect
        }
    }



    EdgeModel {
        model: main_model
        tag: 'polygon'
        mrl: 'spview/road_graph'
        onItemAdded: {
            console.log( 'EDGE ADDED' )
            var edgehandle = getHandleByUid(uid)
            var data = edgehandle.getData()
            var polygon = data.polygon
            var attributes = data.attributes
            if( attributes.name_2.length > 1 ) {
                var size = 90
                if( attributes.type_2 === 'primary' ) size = 20
                else if( attributes.type_2 === 'primary_link' ) size = 18
                else if( attributes.type_2 === 'secondary' ) size = 16
                else if( attributes.type_2 === 'secondary_link' ) size = 14
                else if( attributes.type_2 === 'tertiary' ) size = 12
                textLayer.appendLabel(polygon, attributes.name_2, size)
                console.log('APPEND LABEL', attributes.name_2)

            }
        }
        onRootIndexChanged: console.log("root", rootIndex)
        onMrlChanged: console.log("mrl", mrl)
        onModelReset: {
            console.log('MODEL RESET', count)
            for( var i = 0; i < count; ++i ){
                var edgehandle = getHandle(i)
                var data = edgehandle.getData()
                var polygon = data.polygon
                var attributes = data.attributes
                if( attributes.name_2.length > 1 ) {
                    var size = 90
                    if( attributes.type_2 === 'primary' ) size = 20
                    else if( attributes.type_2 === 'primary_link' ) size = 18
                    else if( attributes.type_2 === 'secondary' ) size = 16
                    else if( attributes.type_2 === 'secondary_link' ) size = 14
                    else if( attributes.type_2 === 'tertiary' ) size = 12
                    textLayer.appendLabel(polygon, attributes.name_2, size)
                    console.log('APPEND LABEL', attributes.name_2)

                }
            }
        }
        Component.onCompleted: {

            console.log('EDGEMODEL COMPLETED', count)
        }
    }


    GraphModel {
        id: main_model
        property url file
        property string fileName: file.toString().length ? file.toString().match(/(\w+)\..+$/)[1] : "untitled"
        property bool changed
        onDataChanged: changed = true
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
            mrl: "spview/styles.test"
        }

        Component.onCompleted: loadModel('file:///Users/Tox/moscow24.mgf')
    }


    NodeChildrenModel {
        id: ways_model
        mrl: "/ways"
        model: main_model

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
        function setVisible( idf ) {
            for (var i = 0; i < count; ++i) {
                var h = ways_model.getHandle(i)
                var col = root.idfToData[idf].color

                h.agent.blob$metadataHandle.color = col
                h.agent.blob$metadataHandle.visible = h.metadata.idf == idf

                updateWay(i)
            }
        }

        function buildWay(index) {
            var h = ways_model.getHandle(index)

            var idf = h.metadata.idf

            var i = m_sceneBuilder.addItem("GuideStroke", {
                                               smooth: 15,
                                               offset: 0,
                                               precision: 1,
                                               flags: VisualInstance.ItemIsSelectable | VisualInstance.NoSelectionOutline
                                           } )
            i.pen.width = 3
            i.pen.color = h.metadata.color
            i.pen.cosmetic = true
            i.pen.strokeStyle = h.metadata.visible ? 1 : 0

            var guide = creatist.deserialize("Guide", h.data)
            var test = gview.view.morf(guide.toPolygon())
            guide.setPolygon(test)


            root.guides.push(guide)
            root.guidesChanged()
            i.source  = guide
            i.source.setPathData(h.nodeId)
            idx2inst[index] = i
            if (h.metadata.label && h.metadata.visible) {
                var t = m_sceneBuilder.addItem("GuideText")
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
            i.pen.strokeStyle = h.metadata.visible ? 1 : 0
            var guide = creatist.deserialize("Guide", h.data)
            var test = gview.view.morf(guide.toPolygon())
            guide.setPolygon(test)
            guide.setPathData(h.nodeId)

            i.source  = guide
            i.source.setPathData(h.nodeId)


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

        onRowsInserted: buildWay(first)
        onDataChanged: updateWay(topLeft.row)
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
                if(ready)
                    i_ds.pendingSize--
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
            visible: handle.data.visible
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
        onObjectAdded: if (parent) parent.insertChild(object, index)
        onObjectRemoved: object.deleted = true
        onValueChanged: {
            var handle = main_model.getHandle(index)
            var l
            if( handle.type != 'Style' )
                l = i_layers.objectAt(handle.parent.inNodes()[0].parent.rowIndex)
            else
                l = i_layers.objectAt(handle.inNodes()[0].parent.rowIndex)
            l.update()
        }
    }

    SceneModel {
        id: m_sceneBuilder
        scene: m_scene.qGraphicsScene
    }

    AuxModel {
        id: aux
    }

    Creatist {
        id: creatist
    }



}

