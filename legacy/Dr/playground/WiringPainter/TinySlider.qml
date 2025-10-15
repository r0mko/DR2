import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Slider {
    style: SliderStyle {
        groove: Rectangle {
            implicitHeight: 2
            implicitWidth: 200
            color: "#80000000"
        }
        handle: Rectangle {
            anchors.centerIn: parent
            color: control.pressed ? "#ffffff" : "#ccc"
            radius: 4
            implicitHeight: 12
            implicitWidth: 20
            border.width: 1
            border.color: "#80000000"
            clip: true
            Text {
                anchors.fill: parent
                font.pointSize: 7
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                text: value > 1 ? value.toPrecision(3) : value.toPrecision(2)
            }

        }

    }
}
