import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import Dr.UI 1.0

Repeater {
    id: root
    property int geometryType
    model: getIconData(geometryType)

    DRPicture {
        sourceData: modelData.data
        color: "#059"
        anchors.fill: parent
        anchors.leftMargin: modelData.hmargin
        anchors.rightMargin: modelData.hmargin
        anchors.topMargin: modelData.vmargin
        anchors.bottomMargin: modelData.vmargin
        rotation: modelData.angle
    }

    function getIconData(type) {
        var ret = []
        switch(type) {
        case -1: // "Invalid"
            ret.push({ data: aux.icons.warn.data, angle: 0, vmargin: 2, hmargin: 2 })
            break
        case 0: // "Geometry"
        case 16: // "TIN"
            ret.push({ data: aux.icons.shapes.data, angle: 0, vmargin: 2, hmargin: 2 })
            break
        case 1: // "Point"
        case 4: //  "MultiPoint"
            ret.push({ data: aux.icons.filled_circle.data, angle: 0, vmargin: 10, hmargin: 10 })
            break
        case 2: // "LineString"
        case 5: // "MultiLineString"
        case 8: // "CircularString"
        case 9: // "CompoundCurve"
        case 11: // "MultiCurve"
        case 13: // "Curve
//            ret.push({ data: aux.icons.polyline.data, angle: 0, vmargin: 2, hmargin: 2 })
            ret.push({ data: aux.icons.polyline_3.data, angle: 0, vmargin: 2, hmargin: 0 })
            break
        case 3: // "Polygon"
        case 6: // "MultiPolygon"
        case 10: // "CurvePolygon"
        case 12: // "MultiSurface"
        case 14: // "Surface"
        case 15: // "PolyhedralSurface"
        case 17: // "Triangle"
//            ret.push({ data: aux.icons.polygon.data, angle: 30, vmargin: 2, hmargin: 2 })
            ret.push({ data: aux.icons.poly1.data, angle: 0, vmargin: 2, hmargin: 2 })
            break
        case 7: // "GeometryCollection"
            ret.push({ data: aux.icons.shapes.data, angle: 30, vmargin: 2, hmargin: 2 })
            break
        default:
            break
        }
        return ret
    }
}
