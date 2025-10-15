import QtQuick 2.3
import DataReactor.UI 1.0
import DataReactor 1.0

Object {
    property Edge edge1
    property Edge edge2

    Line {
        id: l1
        p1: edge1.n1.p
        p2: edge1.n2.p
    }
    Line {
        id: l2
        p1: edge2.n1.p
        p2: edge2.n2.p
    }

    property real angle: l1.angleTo(l2)
    onAngleChanged: console.log("angle: %1".arg(angle))

    function paint(ctx, raduis) {

    }
}
