import QtQuick 2.5
import Dr.MetaGraph 1.0
import Dr.Graphics 1.0


EdgeModel {
    id: roadGraph
    tag: 'polygon'
    property var edgeUid2item: []
    property bool interactive
    signal edgeHoverEnter(var runner, var edgeData)
    signal edgeHoverMove(var runner, var edgeData)
    signal edgeHoverLeave
    property bool visible: true
//    onModelAboutToBeReset:

    onVisibleChanged: {
        edgeUid2item.forEach(function(e) {
            e.visible = visible
        })
    }

    onModelAboutToBeReset: {
//        console.log("Road graph resetting", count)
        edgeUid2item.forEach(function(item) { item.destroy() })
        edgeUid2item = []
    }

    onModelReset: {
//        console.log("Road graph was reset", count)
        for (var i = 0; i < count; ++i) {
            var h = getHandle(i)
            addInstance(h)
        }
    }

    onInteractiveChanged: {
        edgeUid2item.forEach(function(e) {
            e.hoverEnabled = interactive
            if (interactive) {
                e.sourcePen.color = "#8af"
                e.acceptedMouseButtons = Qt.LeftButton
                e.onMousePress = onMousePress
                e.onMouseMove = onMouseMove
                e.onMouseRelease = onMouseRelease
            } else {
                e.sourcePen.color = "#ddd"
                e.acceptedMouseButtons = Qt.NoButton
                e.onMousePress = undefined
                e.onMouseMove = undefined
                e.onMouseRelease = undefined
            }
            e.update()
        })
    }

    onItemAdded: {
        var eh = getHandleByUid(uid)
        addInstance(eh)
    }

    function addInstance(eh) {
        var p = creatist.create("Guide")
        var ed = eh.getData()
        p.setPolygon(ed.polygon)
        p.data = { attributes: ed.attributes, uid: eh.uid }
//        p.setPathData ({ attributes: ed.attributes, uid: eh.uid })
        var i = m_sceneBuilder.addItem("GuideStroke", {
                                           smooth: 0,
                                           precision: 0,
                                           source: p,
                                           flags: VisualInstance.ItemIsSelectable | VisualInstance.NoSelectionOutline
                                           //                                               cacheMode: 1
                                       })
        edgeUid2item[eh.uid] = i
        i.pen.color = "#ffaa40"
        i.pen.width = 4
        i.pen.cosmetic = true
        i.sourcePen.color = "#ddd"
        i.sourcePen.cosmetic = true
        i.sourcePen.strokeStyle = Qt.SolidLine
        i.drawSource = true
        i.drawResult = false

        i.onHoverEnter = onHoverEnter
        i.onHoverLeave = onHoverLeave
        i.onHoverMove = onHoverMove
        i.visible = roadGraph.visible

        i.hoverEnabled = interactive
        i.updatePath()
    }

    function onHoverEnter(event) {
        //            console.log("hover enter", r.pos, r.point)
        var runner = this.result.findNearestPos(event.pos)
        edgeHoverEnter(runner,this.result.data)

    }
    function onHoverMove(event) {
        var runner = this.result.findNearestPos(event.pos)
        edgeHoverMove(runner,this.result.data)
    }

    function onHoverLeave(event) { edgeHoverLeave() }

    function onMousePress(event) {
        var r = this.source.findNearestPos(event.pos)
        route.beginSegment(r, this.source, this.source.data.uid)
    }

    function onMouseMove(event) {
        route.continueSegment(event.pos)
    }

    function onMouseRelease() {
        route.finishSegment()
    }
}
