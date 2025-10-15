import QtQuick 2.3

QtObject{
    property point p
    property real x: p.x
    property real y: p.y
    onXChanged: p.x = x
    onYChanged: p.y = y

    property var relations: []
}
