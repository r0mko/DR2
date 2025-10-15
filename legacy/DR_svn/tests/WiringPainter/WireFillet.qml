import QtQuick 2.3

QtObject {
    property WireSegment from
    property WireSegment to

    property real c: cvalue.value
    function stroke(ctx, width, color) {
        _i.stroke(ctx, width, color)
    }

    function calc() {
        _i.calculate()
    }

    property QtObject r: QtObject {
        id: _i
        property real radius: Math.max(from.end.offset, to.begin.offset)
        property real angle_from
        property real angle_to
        property real angle
        property bool ccw: angle > 0
        property vector2d p


        function calculate() {
            var dp = from.unit.dotProduct(to.unit) // dot product
            angle = from.azimuth - to.azimuth
            angle %= 360
            angle = (angle + 360) % 360
            if (angle > 180)
                angle -= 360;

            var k = angle < 0 ? kposvalue.value : knegvalue.value // no offset of rounding center in case of left turn (wire offset > 0!)

//            var r = k > 0 ? radius * 0.5 : radius
            var subtence = radius / Math.sqrt((dp + 1) / 2) * k // length of vector from node to intersection

            var vec1 = from.normalVector()
            var vec2 = to.normalVector()
            var bisect = vec1.plus(vec2).normalized().times(subtence*c) // fillet center vector (relative to node)
            p = Qt.vector2d(to.begin.node.x, to.begin.node.y).plus(bisect)

            angle_from = from.azimuth / 180 * Math.PI + Math.PI / 2
            angle_to = to.azimuth / 180 * Math.PI + Math.PI / 2
            if (angle < 0) {
                angle_from -= Math.PI
                angle_to -= Math.PI
            }

            // fillet offsets
            var o_from = (from.vector.length() + from.unit.dotProduct(bisect)) / from.vector.length()
            var o_to = to.unit.dotProduct(bisect) / to.vector.length()
            to.begin.factPosition.push(o_to)
            from.end.factPosition.push(o_from)
//            radius = r
        }

        function stroke(ctx, width, color) {
            ctx.beginPath()
            ctx.strokeStyle = color
            ctx.lineWidth = width
            if (angle < 0)
                ctx.arc(p.x, p.y, radius*c, angle_from, angle_to, ccw)
            else
                ctx.arc(p.x, p.y, radius*c, angle_from, angle_to, ccw)
            ctx.stroke()
        }
    }


    //    Component.onCompleted: calculate()



}
