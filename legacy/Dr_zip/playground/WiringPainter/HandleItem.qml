import QtQuick 2.3

Rectangle {
    id: handle
    property alias mouseArea: m_area
    property alias drag: m_area.drag
    border.width: 2
    border.color: "#FF0000"
    radius: 2
    color: "transparent"
    width: 12
    height: 12
    transform: Translate {
        x: -handle.width / 2
        y: -handle.height / 2
    }
    scale: 1 / drawarea.scene.scale

    MouseArea {
        id: m_area
        anchors.fill: parent
        hoverEnabled: true
        drag.target: handle
        drag.threshold: 0
        cursorShape: Qt.OpenHandCursor

    }
}
