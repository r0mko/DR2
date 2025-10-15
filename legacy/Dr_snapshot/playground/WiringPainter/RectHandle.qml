import QtQuick 2.0

Rectangle {

    id: slider
    property alias mouseArea: marea
    property int size: 14
    width: size
    height: size
    transform: Translate {
        x: -slider.width / 2
        y: -slider.height / 2
    }

    color: "transparent"
    border.width: 2
    border.color: "#fff"
    radius: 3
    scale: 1 / m_scene.viewportScale

    MouseArea {
        id: marea
        anchors.fill: parent
        hoverEnabled: true
    }
}
