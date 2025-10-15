import QtQuick 2.0

QtObject {
    property Node node
    property real position
    property real offset
    property var factPosition: []

    function toJsObject() {
        return { node: node, position: position, offset: offset }
    }

}


