import QtQuick 2.3
import Dr 1.0
import Dr.Graphics 1.0

RibGraphicsDelegate {
    id: root

    //    property RibSlider slider: null
    property RibSlider inSlider: null
    property RibSlider outSlider: null
    property Object resources: Object {
        id: d
        Connections {
            target: outSlider

            onAtEndChanged: {
                console.log( 'onAtEndChanged', outSlider.runner.data )
                if (outSlider.atEnd) {
                    console.log("out slider reached end of rib", modelIdx, "node at end:", node2)
                    if (node2 !== route_builder.currentNodeIdx){
                        console.log( 'pushing hop' )
                        route_builder.pushHop(node2, root)
                    }else {
                        route_builder.popHop()
                    }

                }
            }
            onAtStartChanged: {
                console.log( 'onAtStartChanged', outSlider.runner.data )
                if (outSlider.atStart) {
                    console.log("out slider reached start of rib", modelIdx, "node at start:", node1)
                    if (node1 !== route_builder.currentNodeIdx)
                        route_builder.pushHop(node1, root)
                    else {
                        route_builder.popHop()
                    }

                }
            }
        }
    }
    visible: m_ribs.showRibs
    minimumWidth:  8
    pen: highlight ? m_ribs.highlightPen : m_ribs.ribPen

    //                showArrows: m_showArrows.checked
    showArrows: isHovered || drawOutline
    //    outlinePen: m_ribs.outlinePen
    outlineBrush: m_ribs.outlineBrush
    acceptedButtons: Qt.AllButtons
    hoverEnabled: true
    flags: SceneItem.ItemIsSelectable | SceneItem.NoSelectionOutline
    oneWay: oneway
    polygon: polygonData
    guide.onLengthChanged: console.log("guide length", guide.length)
    highlight: isHovered
    arrow.length: 10
    arrow.step: 30
    arrow.width: 3
    arrow.offset: 2
    arrow.pen: m_ribs.arrowPen
    strokePen: m_ribs.highlightPen2

    function updateHighlight() {
        highlightRange(inSlider.position, outSlider.position)
    }

    function posAtNode(nodeId) {
        if (nodeId === node1)
            return 0;
        if (nodeId === node2)
            return guide.length
        return -1
    }

    function setSliders(inPos, outPos) {
        inSlider = comp_ribSlider.createObject(drawarea.scene, { rib: root, position: inPos } )
        outSlider = comp_ribSlider.createObject(drawarea.scene, { rib: root, position: outPos } )
        inSlider.positionChanged.connect(updateHighlight)
        outSlider.positionChanged.connect(updateHighlight)
    }

    function resetSliders() {
        clearRangeHighlight()
        if (inSlider) {
            inSlider.destroy()
        }
        if (outSlider) {
            outSlider.destroy()
        }
    }

    onPressed: {
        if (!(buttons & Qt.LeftButton))
            return
        if (!inSlider && !outSlider) {
            var f = nearestPoint(pos)
            setSliders(f.length,f.length)
            route_builder.candidateRibs = [ root ]
            route_builder.currentSlider = outSlider
            route_builder.currentRib = root
        }
    }
    onMoved: {
        route_builder.findNearestRib(pos)
    }
    onReleased: {
        var i = 0
        route_builder.clearCandidateRibs()
        route_builder.currentRoute.push({ rib: route_builder.currentRib, node: -1 })
//        var wireDelegate = comp_smoothLine.createObject()
        var route = []

        var stSlider = null
        var endSlider = null
        route_builder.currentRoute.forEach(function(e){
            route = route.concat(e.rib.guide.partData(e.rib.inSlider.position, e.rib.outSlider.position))
            if (!stSlider) {
                stSlider = comp_wireSlider.createObject()
                stSlider.ribModelIndex = e.rib.modelIdx
                stSlider.ribPosition = e.rib.inSlider.position
                stSlider.reversed = e.rib.inSlider.position > e.rib.outSlider.position
            }

            e.rib.resetSliders()
            if (e.candidates)
                e.candidates.forEach(function(r) {
                    r.drawOutline = false
                })
        })
        if (stSlider) {
            endSlider = comp_wireSlider.createObject()
            endSlider.ribModelIndex = route_builder.currentRib.modelIdx
            endSlider.ribPosition = route_builder.currentRib.outSlider.position
            endSlider.reversed = route_builder.currentRib.inSlider.position > route_builder.currentRib.outSlider.position
        }
        if (route.length > 0)
            model_wires.addWire(route, 3, Math.random(), 1.0, 0.4, { begin: stSlider, end: endSlider })
        route_builder.currentRoute = []
        route_builder.currentSlider = null
        route_builder.currentRib = null
        route_builder.currentNodeIdx = -1


    }

    function otherNodeId(nodeId) {
        if (nodeId === node1) {
            return node2
        }
        if (nodeId === node2) {
            return node1
        }
        return null
    }

    //            opacity: 0.5

    modelIdx: index
    node1: startNode
    node2: endNode
    scene: m_scene
}
