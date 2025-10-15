import QtQuick 2.3
import QtGraphicalEffects 1.0
import DataReactor.UI 1.0
import "Delegates"

Item {
    width: 60
    height: 60
    id: root

    property real allHue: UI.colorToHsla(rect.color).h
    property real allSat
    property real allLum

    Connections {
        id: allSatNotif
        target: root
        onAllSatChanged: rect.color = luminanceMode ? UI.fromHSLum(allHue, allSat, allLum) : Qt.hsla(allHue, allSat, allLum)
    }

    Connections {
        id: allLumNotif
        target: root
        onAllLumChanged: rect.color = luminanceMode ? UI.fromHSLum(allHue, allSat, allLum) : Qt.hsla(allHue, allSat, allLum)
    }

    property var hsla: UI.colorToHsla(colorValue)
    property alias color: rect.color
    property alias border: rect.border
    property ColorEditor editor: ColorEditor {
        hue: hsla.h
        sat: hsla.s
        lum: luminanceMode ? UI.luminance(colorValue) : hsla.l
        alpha: hsla.a
        function updateColor() {

            rect.color = luminanceMode ? UI.fromHSLum(hue, sat, lum) : Qt.hsla(hue, sat, lum, alpha)
        }
        onHueChanged: updateColor()
        onSatChanged: updateColor()
        onLumChanged: updateColor()
    }

    Rectangle {
        id: rect
        anchors.fill: parent
        radius: 10
        border.width: 1
        color: Qt.hsla(hueslider.value, satslider.value, lumslider.value, alpha)
    }

    InnerShadow {
        anchors.fill: rect
        radius: 8.0
        samples: 16
        spread: 0.3
        horizontalOffset: 0
        verticalOffset: 2
        color: "#f0000000"
        source: rect
    }


    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (palette.currentIndex === index && !toolwin.visible) {
                toolwin.visible = true
                return
            }
            palette.currentIndex = index
        }
    }



}


