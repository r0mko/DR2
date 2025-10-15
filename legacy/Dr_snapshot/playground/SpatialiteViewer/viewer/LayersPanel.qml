import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import Dr.MetaGraph 1.0
import QtQml.Models 2.2
import Dr.Graphics 1.0
import Dr.Cartograph 1.0

SidebarPanel {
    id: layers_panel
    property NodeChildrenModel layersModel
    property Instantiator layers
    property GraphModel mainModel
    property Instantiator views

    LayersView {
        id: v_layers
        Layout.fillWidth: true
        Layout.minimumHeight: 300
        model: layersModel
        layers: layers_panel.layers
        signal requestSettings(var handle, GeoLayer layerItem)
        onRequestSettings: {
            layer_settings_loader.handle = handle
            layer_settings_loader.layerItem = layerItem
        }
        Loader {
            id: layer_settings_loader
            property var handle
            property GeoLayer layerItem: null
            active: layerItem != null

            sourceComponent: LayerSettings {
                Component.onCompleted: open(layer_settings_loader.handle, layer_settings_loader.layerItem)
                onClosing: {
                    for (var i = 0; i < layerItem.iGeomBuffer.count; ++i) {
                        layerItem.iGeomBuffer.objectAt(i).highlight = false
                    }
                    layer_settings_loader.layerItem = null

                }

            }
        }
    }




    function getIconSource(type) {
        switch(type) {
        case -1: // "Invalid"
            return aux.icons.warn.data
        case 0: // "Geometry"
        case 16: // "TIN"
            return aux.icons.shapes.data
        case 1: // "Point"
        case 4: //  "MultiPoint"
            return aux.icons.dot_in_square.data
        case 2: // "LineString"
        case 5: // "MultiLineString"
        case 8: // "CircularString"
        case 9: // "CompoundCurve"
        case 11: // "MultiCurve"
        case 13: // "Curve
            return aux.icons.polyline.data
        case 3: // "Polygon"
        case 6: // "MultiPolygon"
        case 10: // "CurvePolygon"
        case 12: // "MultiSurface"
        case 14: // "Surface"
        case 15: // "PolyhedralSurface"
        case 17: // "Triangle"
            return aux.icons.polygon.data
        case 7: // "GeometryCollection"
            return aux.icons.shapes.data
        default: return aux.icons.error.data
        }
    }
    
}
