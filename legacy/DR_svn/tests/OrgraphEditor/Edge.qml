import QtQuick 2.0
import DataReactor 1.0
import DataReactor.UI 1.0

Object {
    property Node n1: null
    property Node n2: null

    function paint(ctx) {
        ctx.moveTo(n1.x, n1.y)
        ctx.lineTo(n2.x, n2.y)
    }
}
