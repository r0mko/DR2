import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import Dr.Cartograph 1.0
import Dr.MetaGraph 1.0


GroupBox {

    property alias dataSourceModel: ds_combo.model
    ColumnLayout {

        ComboBox {
            id: ds_combo
            Layout.fillWidth: true
        }

        TableView {
            TableView {
                id: layers_table
                Layout.fillWidth: true
                Layout.fillHeight: true
                selectionMode: SelectionMode.ExtendedSelection
                function getIconSource(type) {
                    switch(type) {
                    case -1: return "qrc:/img/img/undef.png" // "Invalid"
                    case 0: return "qrc:/img/img/undef.png" // Geometry
                    case 1: return "qrc:/img/img/point.png" // "Point"
                    case 2: return "qrc:/img/img/polyline.png" // "LineString"
                    case 3: return "qrc:/img/img/polygon.png" // "Polygon"
                    case 4: return "qrc:/img/img/point.png" //  "MultiPoint"
                    case 5: return "qrc:/img/img/polyline.png" // "MultiLineString"
                    case 6: return "qrc:/img/img/polygon.png" // "MultiPolygon"
        //                            case 7: return "GeometryCollection"
                    case 8: return "qrc:/img/img/polyline.png" // "CircularString"
                    case 9: return "qrc:/img/img/polyline.png" // "CompoundCurve"
                    case 10: return "qrc:/img/img/polygon.png" // "CurvePolygon"
                    case 11: return "qrc:/img/img/polyline.png" // "MultiCurve"
                    case 12: return "qrc:/img/img/polygon.png" // "MultiSurface"
                    case 13: return "qrc:/img/img/polyline.png" // "Curve"
                    case 14: return "qrc:/img/img/polygon.png" // "Surface"
        //                            case 15: return "PolyhedralSurface"
        //                            case 16: return "TIN"
                    case 17: return "qrc:/img/img/polygon.png" //  "Triangle"
                    default: return ""
                    }
                }

                TableViewColumn {
                    title: "Слой"
                    role: "tableName"
                }
                TableViewColumn {
                    title: "Тип"
                    role: "geometryType"
                    resizable: false
                    width: 32
                    delegate: Image {
                        fillMode: Image.Pad
                        source: _table.getIconSource(styleData.value)
                    }
                }
            }
            RowLayout {
                Button { action: a_selectAll }
                Button { action: a_addSelection }
                Button { action: a_cancel }
            }
        }


    }

}
