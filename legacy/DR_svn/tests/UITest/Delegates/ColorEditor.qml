import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import DataReactor.UI 1.0
ColumnLayout {
    property alias hue: hueslider.value
    property alias sat: satslider.value
    property alias lum: lumslider.value
    property real alpha
    id: sliders
    property int labelWidth: 70
    width: 320
    height: 81
    RowLayout {

        Item {
            width: sliders.labelWidth
            Layout.fillHeight: true
            Text {
                anchors.fill: parent
                text: "Hue:"
                color: "#fff"
            }
        }

        Slider {
            id: hueslider
            width: sliders.sliderWidth
            Layout.fillWidth: true
            property string label: "H:" + Number(hueslider.value * 360).toFixed(0)
            minimumValue: 0.0
            maximumValue: 1.0
            height: 25
            style: PaletteSliderStyle {
                color: Qt.hsla(hueslider.value, 1.0, 0.5)
            }
        }
    }
    RowLayout {
        Item {
            width: sliders.labelWidth
            Layout.fillHeight: true
            Text {
                anchors.fill: parent
                text: "Saturation:"
                color: "#fff"
            }
        }

        Slider {
            id: satslider
            Layout.fillWidth: true
            property string label: "S"
            minimumValue: 0.0
            maximumValue: 1.0
            height: 25
            style: PaletteSliderStyle {
                color: Qt.hsla(hueslider.value, satslider.value, 0.5)
            }
        }
    }

    RowLayout {
        Item {
            width: sliders.labelWidth
            Layout.fillHeight: true
            Text {
                anchors.fill: parent
                text: "Lightness:"
                color: "#fff"
            }
        }
        Slider {
            id: lumslider
            Layout.fillWidth: true
            property string label: "L" + Number(lum).toFixed(2)
            minimumValue: 0.0
            maximumValue: 1.0
            height: 25
            style: PaletteSliderStyle {
                color: Qt.hsla(0.0, 0.0, lumslider.value)
            }
        }
    }
}
