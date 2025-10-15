import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import Dr.Cartograph 1.0
import Dr.UI 1.0
Rectangle {
    property GeomBuffer buffer
    property var handle
    property color textColor: styleData.textColor
    color: buffer ? "transparent" : "#80ff0000"

    property int status: buffer ? buffer.status : 0
    property bool highlight: styleData.selected && buffer != null
    onHighlightChanged: buffer.highlight = highlight

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 4
        spacing: 1

        Item {
            width: height
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            GeometryIcon {
                anchors.fill: parent
                anchors.margins: 4
                geometryType: buffer ? buffer.layer.geometryType : -1
            }
        }
        Label {
            //            text: handle.data.where
            elide: Text.ElideMiddle
            text: handle.nodeId
//            text: buffer ? (buffer.clause.length ? buffer.clause : buffer.layer.table) : "—"
        }


        Item {
            Layout.fillWidth: true
        }

        ColumnLayout {
            width: 60
            height: parent.height
            spacing: 0
            Item { height: 1 }
            Rectangle {
                width: 60
                height: 12
                border.color: "#40000000"
                radius: 3
                Rectangle {
                    height: parent.height
                    width: status == 4 ? parent.width * buffer.completedEntityCount / buffer.entityCount : parent.width
                    color: {
                        switch(status) {
                        case 0: return "#80ff0000"    // "INVALID"
                        case 1: return "#80000000"    // "Empty"
                        case 2: return "#80808080"    // "Queued"
                        case 3: return "#8000ffff"    // "Receiving"
                        case 4: return "#800080ff"    // "Processing"
                        case 5: return "#80ffffff"    // "Ready"
                        default: return "#000"
                        }
                    }
                }

                Label {
                    font.pixelSize: 10
                    anchors.centerIn: parent
                    text: {
                        switch(status) {
                        case 0: return "INVALID"
                        case 1: return "Empty"
                        case 2: return "Queued"
                        case 3: return "Receiving"
                        case 4: return "Processing"
                        case 5: return "Ready"
                        default: return "WAT???"
                        }
                    }
                }
            }
            Item {
                width: parent.width
                Layout.fillHeight: true
                Label {
                    color: styleData.textColor
                    anchors.centerIn: parent
                    font.pixelSize: 8
                    text: buffer ? buffer.entityCount : ""
                }
            }
        }




        VectorIconButton {
            tooltip: "Settings"
            iconVPadding: 2
            iconHPadding: 2
            width: 20
            height: 20
            iconData: aux.icons.gear_1.data
            //            flat: false
            onClicked: geom_style_dialog.open()
        }


    }
}
