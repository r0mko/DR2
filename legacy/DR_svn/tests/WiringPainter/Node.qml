import QtQuick 2.3

QtObject {
    property point p

    property real x: p.x
    property real y: p.y

    property int index

    function toJsObject() {
        return {x: x, y: y}
    }
}
