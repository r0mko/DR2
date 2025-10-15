import QtQuick 2.3

Rectangle {
    property alias text: label.text
    property alias font: label.font
    property alias name: value
    implicitHeight: 40
    implicitWidth: 40
    radius: 20
    border.width: 1
    border.color: "#60000000"
    gradient: Gradient {
            GradientStop { position: 0.0; color: "#ffffff" }
            GradientStop { position: 0.1; color: "#a9d2f3" }
            GradientStop { position: 0.25; color: "#90bae4" }
            GradientStop { position: 0.37; color: "#90bcea" }
            GradientStop { position: 0.50; color: "#90bff0" }
            GradientStop { position: 0.51; color: "#6ba8e5" }
            GradientStop { position: 0.83; color: "#a2daf5" }
            GradientStop { position: 1.0; color: "#bdf3fd" }

    }
    Text {
        id: label
        anchors.fill: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
