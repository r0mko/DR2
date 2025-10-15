import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import DataReactor.UI 1.0

SliderStyle {
    id: style
    property color color
    property color textColor: "white"
    property string label

    handle: Rectangle {
        id: rect
        anchors.centerIn: parent
        color: style.color
        border.color: "black"
        border.width: 1
        implicitWidth: 38
        implicitHeight: 24
        radius: 6
        Text {
            id: text
            font.pointSize: 10
            color: UI.getColor(rect.color, 0.0, 0.0, 0.9)
            anchors.centerIn: parent
            text: control.label
            onColorChanged: console.log("text color", color)
        }
    }
}
