import QtQuick 2.3
import DataReactor.UI 1.0
//import DataReactor 1.0
QtObject {
    property WireSegmentEdge begin: WireSegmentEdge {
        position: 0
    }
    property WireSegmentEdge end: WireSegmentEdge {
        position: 1.0
    }

//    property point p1: Qt.point(begin.node)

    property var next: []

    property vector2d vector: Qt.vector2d(end.node.x - begin.node.x, end.node.y - begin.node.y)
    property vector2d unit: vector.normalized()
    property real azimuth: Math.atan2(unit.y, unit.x) * 180 / Math.PI

    function resetRealPos() {
        begin.factPosition = []
        end.factPosition = []
    }

    function strokeAxis(ctx) {
        ctx.beginPath()
        ctx.strokeStyle = Qt.rgba(0.5,0.5,0.5,1)
        ctx.lineWidth = 1

        ctx.moveTo(begin.node.x, begin.node.y)
        ctx.lineTo(end.node.x, end.node.y)

        ctx.stroke()
    }

    function pointAtLength(pos, startX, startY) {
        if (arguments.length < 3) {
            startX = begin.node.x
            startY = begin.node.y
        }
        var v = unit.times(pos).plus(Qt.vector2d(startX, startY))
        return { x: v.x, y: v.y }
    }

    function pointAtPercent(pos, startX, startY) {
        if (arguments.length < 3) {
            startX = begin.node.x
            startY = begin.node.y
        }
        var v = vector.times(pos).plus(Qt.vector2d(startX, startY))
        return { x: v.x, y: v.y }
    }

    function pointAtSegmentLength(p1, p2, pos) {
        var vec = Qt.vector2d(p2.x - p1.x, p2.y - p1.y)
        var uv = vec.normalized()
        var p1Vec = Qt.vector2d(p1.x, p1.y)
        var retVec = p1Vec.plus(uv.times(pos))
        return Qt.point(retVec.x, retVec.y)
    }

    function pointAtSegmentPercent(p1, p2, pos) {
        var vec = Qt.vector2d(p2.x - p1.x, p2.y - p1.y)
        var uv = vec.normalized()
        var p1Vec = Qt.vector2d(p1.x, p1.y)
        var retVec = p1Vec.plus(uv.times(pos * vec.length()))
        return Qt.point(retVec.x, retVec.y)
    }

    function stroke(ctx, width, color) {
        var nv = normalVector()
        var p1 = Qt.vector2d(begin.node.x, begin.node.y).plus(nv.times(begin.offset))
        var p2 = Qt.vector2d(end.node.x, end.node.y).plus(nv.times(end.offset))
        // pos.sort(function(a,b){ return a>b})[0] min

        var bfp = begin.factPosition.length === 0   ? 0     : begin.factPosition.sort()[0]
        var efp = end.factPosition.length === 0     ? 1.0   : end.factPosition.sort().reverse()[0]
        var sp = pointAtSegmentPercent(p1, p2, bfp)
        var ep = pointAtSegmentPercent(p1, p2, efp)

        ctx.beginPath()
        ctx.strokeStyle = color
        ctx.lineWidth = width
        ctx.moveTo(sp.x, sp.y)
        ctx.lineTo(ep.x, ep.y)
        ctx.stroke()

    }

    function normalVector() {
        return Qt.vector2d(-unit.y, unit.x)
    }


}
