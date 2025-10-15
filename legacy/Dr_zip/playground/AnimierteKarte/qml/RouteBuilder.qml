import QtQuick 2.5
import Dr.MetaGraph 1.0
import Dr.Graphics 1.0


ListModel {
    id: route
    property var edgeUid2modelIdx: []
    property int currentEdgeUid
    property var candidateEdges: []
    property var helpers:[]
    property var currentSectionHandle
    property string currentSectionType
    property GuideView currentNet: currentSectionHandle ? i_sections.instance(currentSectionHandle) : null
    property RoadGraph currentGraph

    onCurrentSectionTypeChanged: console.log(currentSectionType)
    signal wayFinished()
    signal wayStarted(var runner, int edgeUid)
    signal desiredPosChanged(point pos)

    function appendSegment(edgeUid, runner, entry) {
        var i = currentGraph.edgeUid2item[edgeUid]
        if (entry === undefined)
            i.strokeStart = runner.position
        else
            i.strokeStart = entry * i.source.length
        append({
                   uid: edgeUid,
                   startPos: i.strokeStart,
                   endPos: runner.position,
                   offset: 3
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
        if (runner.atEnd) endpos = currentGraph.edgeUid2item[currentEdgeUid].source.length
        set(edgeUid2modelIdx[currentEdgeUid], {
                endPos: runner.position
            })
        var i = currentGraph.edgeUid2item[currentEdgeUid]
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
//        console.log("candidates", JSON.stringify(candidateEdges), "Current:", JSON.stringify({uid: currentEdgeUid, pos: runner.position/roadGraph.edgeUid2item[currentEdgeUid].source.length }))
    }

    function distance(p1, p2) {
        return Qt.vector2d(p2.x - p1.x, p2.y - p1.y).length()
    }


    function switchCurrentEdge(edgeUid, runner, entry) {
        var inst = currentGraph.edgeUid2item[currentEdgeUid]
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
            var r = currentGraph.edgeUid2item[i.uid].source.findNearestPos(desiredPos)
            var d = distance(desiredPos, r.point)
            if (d < dist) {
                nextEdge = i.uid
                runner = r
                dist = d
                entry = i.pos
            }
        })
        if (nextEdge < 0) {
            var r = currentGraph.edgeUid2item[currentEdgeUid].source.findNearestPos(desiredPos)
            console.log("no available candidate edge for position", desiredPos, get(count - 1).startPos)
            return { uid: currentEdgeUid, runner: r, entry: get(count - 1).startPos }
        }
        if (edgeUid2modelIdx[nextEdge] != undefined && edgeUid2modelIdx[nextEdge] - 1 > count) {
            console.warn("move back more than one step", edgeUid2modelIdx[nextEdge] - 1 , count)
        }
        return { uid: nextEdge, runner: runner, entry: entry }
    }


    function beginSegment(runner, guide, uid) {
        candidateEdges.push({ uid: uid, pos: runner.position / guide.length })
        drag_handle.runner = runner
        drag_handle.visible = true
        appendSegment(uid, runner)
        wayStarted(runner, uid)

    }

    function continueSegment(desiredPos) {
        var c = findCandidate(desiredPos)
        drag_handle.runner = c.runner
        drag_handle.updateHandle()
        desiredPosChanged(desiredPos)
        if (c.uid == currentEdgeUid) {
            updateEndPos(c.runner)
        } else {
            switchCurrentEdge(c.uid, c.runner, c.entry)
        }
        var edgeUid = c.uid
        var eh = currentGraph.getHandleByUid(edgeUid)
        if (c.runner.atEnd) {
            findCandidateEdges(eh.endNode, c.runner)
        } else if (c.runner.atStart) {
            findCandidateEdges(eh.startNode, c.runner)
        } else
            return

        c = findCandidate(desiredPos)
        if (edgeUid2modelIdx[c.uid] !== undefined) {
            switchCurrentEdge(c.uid, c.runner, c.entry)
        } else {
            appendSegment(c.uid, c.runner, c.entry)
        }
    }

    function finishSegment() {
        drag_handle.visible = false
        wayFinished()
    }

    function buildGuide(startIndex, endIndex, startPos, endPos) {
        var path = creatist.create("Guide")
        var edges = []
        for (var i = 0; i < count; ++i) {
            var s = get(i)
            var item = currentGraph.edgeUid2item[s.uid]
            item.drawResult = false
            item.strokeEnd = -1
            item.strokeStart = 0
            if (i >= startIndex && i < endIndex) {
                var seg = currentGraph.getHandleByUid(s.uid)
                var segmentPath = creatist.create("Guide")
                segmentPath.setPolygon(seg.getData().polygon)
                var stpos
                if (i === startIndex && startPos !== undefined) {
                    stpos = startPos
                    console.log("substitute startpos", s.startPos, "to", startPos)
                } else
                    stpos = s.startPos
                var epos
                if (i === endIndex - 1 && endPos !== undefined)
                    epos = endPos
                else
                    epos = s.endPos
                var guide = segmentPath.part(stpos, epos)
                path.appendGuide(guide)
                var edge = {
                    uid: s.uid,
                    startPos: s.startPos,
                    endPos: s.endPos,
                }

                edges.push(edge)
            }
        }
        return { guide: path, edges: edges }
    }

    function extendSegmentFromEnd(segmentHandle, junctionCount, junctionPos, convergenceCount, convergencePos) {
        var stnode = segmentHandle.startNode
        var endnode = segmentHandle.endNode
        var segData = segmentHandle.getData()
        var extension = buildGuide(junctionCount, count, junctionPos)
        console.log("segmentHandle", segmentHandle, stnode, endnode, "old len", segData.guide.length, "added len", extension.guide.length, junctionPos, junctionCount)
        var fullPath = segData.guide.clone()

        fullPath.appendGuide(extension.guide)
        segData.guide = fullPath
        endnode.data = { type: "EndVertex", x: fullPath.tailPoint.x, y: fullPath.tailPoint.y }
//        segData.edges.concat(extension.edges)
        var oldUid = segmentHandle.uid
        segmentHandle.destroy()
        var edge = gmodel.addEdge(stnode, endnode, "Catenary|Guide", segData)
        fixBoundaries(currentSectionHandle, oldUid, [edge], [edge])
        //        console.log("added edge", edge)
        clear()
        edgeUid2modelIdx = []
        currentEdgeUid = -1
        candidateEdges = []
    }

    function appendSection(segmentHandle, junctionCount, junctionPos, convergenceCount, convergencePos) {
        var stnode = segmentHandle.startNode
        var endnode = segmentHandle.endNode
        var segData = segmentHandle.getData()
        var extension = buildGuide(junctionCount, count, junctionPos)
        console.log("append new section, seg len", extension.guide.length)
        var nextSec = createSection(currentSectionType)

        var destSegUid = addGuide(nextSec, extension.guide, segData.offset)
        gmodel.addEdge(currentSectionHandle, nextSec, "BoundaryVertex", {
                           from: segmentHandle.uid,
                           to: destSegUid
                       })
        clear()
        edgeUid2modelIdx = []
        currentEdgeUid = -1
        candidateEdges = []
    }


    function addGuide(sectionHandle, guide, offset, startNodeHandle, endNodeHandle) {
        if (offset === undefined)
            offset = 3
        var metadata = {
            edges: [],
            guide: guide,
            offset: offset
        }
        if (!startNodeHandle) {
            f_DataNode.parentHandle = sectionHandle
            f_DataNode.data = { type: "StartVertex", x: guide.headPoint.x, y: guide.headPoint.y  }
            startNodeHandle = f_DataNode.create("Start")
        } else {
            var oldData = startNodeHandle.data
            //            console.log("startNode explicit", JSON.stringify(oldData), startNodeHandle)
            oldData.x = guide.headPoint.x
            oldData.y = guide.headPoint.y
            startNodeHandle.data = oldData
        }
        if (!endNodeHandle) {
            f_DataNode.parentHandle = sectionHandle
            f_DataNode.data = { type: "EndVertex", x: guide.tailPoint.x, y: guide.tailPoint.y }
            endNodeHandle = f_DataNode.create("End")
        } else {
            oldData = endNodeHandle.data
            //            console.log("endNode explicit", JSON.stringify(oldData), endNodeHandle)
            oldData.x = guide.tailPoint.x
            oldData.y = guide.tailPoint.y
            endNodeHandle.data = oldData
        }
        return gmodel.addEdge(startNodeHandle, endNodeHandle, "Catenary|Guide", metadata)
    }

    function extendSegmentFromStart(segmentHandle, junctionCount, junctionPos, convergenceCount, convergencePos) {

    }

    function insertBranchSegment(segmentHandle, junctionCount, junctionPos, convergenceCount, convergencePos) {
        console.log("Seg handle", segmentHandle)
        var stnode = segmentHandle.startNode
        var endnode = segmentHandle.endNode
        var segData = segmentHandle.getData()
        var parts = segData.guide.splitted(junctionPos)
        var extGuide = buildGuide(junctionCount, count)
        console.log(parts, JSON.stringify(segData))
        console.log("Building branch from", junctionCount, "to", count, extGuide.guide.length)
        var leftPartMetadata = { edges: [], guide: parts[0], offset: segData.offset }
//        leftPartMetadata.guide = parts[0]
        var rightPartMetadata = { edges: [], guide: parts[1], offset: segData.offset }
//        rightPartMetadata.guide = parts[1]
        var extMetadata = { edges: extGuide.edges, guide: extGuide.guide, offset: segData.offset }
//        extMetadata.guide = extGuide.guide
        var oldUid = segmentHandle.uid
        segmentHandle.destroy()



        f_DataNode.parentHandle = currentSectionHandle
        f_DataNode.data = { type: "JunctionVertex", x: parts[0].tailPoint.x, y: parts[0].tailPoint.y }
        var jnode = f_DataNode.create("Junction")
        f_DataNode.data = { type: "EndVertex", x: extGuide.guide.tailPoint.x, y: extGuide.guide.tailPoint.y }
        var enode = f_DataNode.create("End")
//        console.log("current handle", currentSectionHandle, "start node", stnode, "new end node", enode, "old end node", endnode, "junction node", jnode)
        var s2j = gmodel.addEdge(stnode, jnode, "Catenary|Guide", leftPartMetadata)
        var j2e1 = gmodel.addEdge(jnode, endnode, "Catenary|Guide", rightPartMetadata)
        var j2e2 = gmodel.addEdge(jnode, enode, "Catenary|Guide", extMetadata)
        fixBoundaries(currentSectionHandle, oldUid, [j2e1, j2e2], [s2j])
        clear()
        edgeUid2modelIdx = []
        currentEdgeUid = -1
        candidateEdges = []

        console.log("s -> j %1, j -> e1 %2, j -> e2 %3".arg(s2j).arg(j2e1).arg(j2e2))
        console.log(stnode.edges(), endnode.edges(), jnode.edges(), enode.edges())
    }


    function addWay(sectionHandle) {
        var p = buildGuide(0, count)
        var metadata = {
            edges: p.edges,
            guide: p.guide,
            offset: 3
        }
        f_DataNode.parentHandle = sectionHandle

        f_DataNode.data = { type: "StartVertex", x: p.guide.headPoint.x, y: p.guide.headPoint.y  }
        var startVtx = f_DataNode.create("Start")
        f_DataNode.data = { type: "EndVertex", x: p.guide.tailPoint.x, y: p.guide.tailPoint.y }
        var endVtx = f_DataNode.create("End")

        var edge = gmodel.addEdge(startVtx, endVtx, "Catenary|Guide", metadata)
        console.log("added edge", edge, JSON.stringify(metadata), currentSectionType)

        clear()
        edgeUid2modelIdx = []
        currentEdgeUid = -1
        candidateEdges = []
    }

    function fixInBoundary(sectionHandle, removedUid, addedUid) {
        sectionHandle.inEdges("BoundaryVertex").forEach(function(ibh) {
            var ibd = ibh.getData()
            if (ibd.to == removedUid) {
                ibd.to = addedUid
                ibh.setData(ibd)
                console.log("fixed inBoundary for", sectionHandle.nodeId, "changed uid %1 to %2".arg(removedUid).arg(addedUid))
            }
        })
    }
    function fixOutBoundary(sectionHandle, removedUid, addedUid) {
        sectionHandle.outEdges("BoundaryVertex").forEach(function(obh) {
            var obd = obh.getData()
            if (obd.from == removedUid) {
                obd.from = addedUid
                obh.setData(obd)
                i_bounds.objectAt(model_boundaries.getIndexByUid(obh.uid))
                console.log("fixed outBoundary for", sectionHandle.nodeId, "changed uid %1 to %2".arg(removedUid).arg(addedUid))
            }
        })
    }

    function fixBoundaries(sectionHandle, removedUid, outUids, inUids) {
        console.log("Fix bounds for", sectionHandle, removedUid)
        inUids.forEach(function(uid) { fixInBoundary(sectionHandle, removedUid, uid) })
        outUids.forEach(function(uid) { fixOutBoundary(sectionHandle, removedUid, uid) })
    }
}
