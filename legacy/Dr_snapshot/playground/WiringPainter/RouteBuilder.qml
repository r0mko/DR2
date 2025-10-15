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
QtObject {
    id: route_builder
    property var stuff:[]
    property int currentNodeIdx: -1
    property var candidateRibs: []
    property var currentRoute: []
    //        onCurrentRibPosChanged: console.log("position on rib %1 = %2 of %3 (%4 \%)".arg(currentRib.modelIdx).arg(currentRibPos).arg(currentRib.length).arg(Number(100 * currentRibPos/currentRib.length).toFixed(2)))
//    property RibDelegate currentRib: null
//    property RibDelegate previousRib: null
//    property RibSlider currentSlider: null
    
    function popHop() {
        var l = currentRoute.pop()
        if (!l)
            return
        if (currentRib)
            currentRib.resetSliders()
        clearCandidateRibs()
        currentRib = l.rib
        candidateRibs = l.candidates
        currentRib.outSlider.handle.visible = true
        currentSlider = currentRib.outSlider
        previousRib = l.previous
        currentNodeIdx = l.currentNode
    }
    
    function pushHop(nodeIdx, originRib) {
        currentRoute.push({rib: originRib, previous: previousRib, candidates: candidateRibs, currentNode: currentNodeIdx})
        
        originRib.outSlider.handle.visible = false
        previousRib = originRib
        currentNodeIdx = nodeIdx
        var nd = m_graphbuilder.nodes.nodeAt(nodeIdx)
        clearCandidateRibs()
        nd.outgoing.forEach(function (i){
            var r = m_ribs.objectAt(i)
            r.drawOutline = true
            candidateRibs.push(r)
        })
        nd.incoming.forEach(function (i){
            var r = m_ribs.objectAt(i)
            r.drawOutline = true
            if (candidateRibs.indexOf(r) === -1) {
                candidateRibs.push(r)
            }
        })
        currentRib = null
        findNearestRib(originRib.outSlider.position)
    }
    
    
    function clearCandidateRibs() {
        candidateRibs.forEach(function(r){
            r.drawOutline = false
        })
        candidateRibs = []
    }
    
    function findNearestRib(desiredPos) {
        var nextRib
        var dist = 1e38
        var pos
        var len
        candidateRibs.forEach(function(r) {
            var pt = r.nearestPoint(desiredPos)
            var d = distance(desiredPos, pt.position)
            if (d < dist) {
                nextRib = r
                pos = pt.position
                len = pt.length
                dist = d
            }
        })

        if (nextRib) {
            if (currentRib !== nextRib) {
                if (currentRib) {
                    currentRib.resetSliders()
                }
                if (nextRib === previousRib) {
                    //                        console.log("move back", previousRib.length, previousRib.outSlider.runner.position, len)
                    if (previousRib.outSlider.position == len)
                        return
                    popHop()
                    return
                }
                nextRib.setSliders(nextRib.posAtNode(currentNodeIdx), len)
                nextRib.inSlider.handle.visible = false
                currentSlider = nextRib.outSlider
                currentRib = nextRib
            } else {
                currentSlider.position = len
            }
        }
    }
    
    function distance(p1, p2) {
        return Qt.vector2d(p2.x - p1.x, p2.y - p1.y).length()
    }
}
