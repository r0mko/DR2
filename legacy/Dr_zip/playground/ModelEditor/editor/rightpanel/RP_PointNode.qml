import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import Dr.UI 1.0
RP_Node {
    id: root
    CaptionLabel {
        Layout.fillWidth: true
        text: 'PointNode data'// + objectData.typeName
        level: 3
    }
    property point pt: objectData.pointnode$point
    onHandleChanged: console.log(pt, handle.point)
    properties: [
        { "X": pt.x },
        { "Y": pt.y }
    ]


    Button {

    }
}
