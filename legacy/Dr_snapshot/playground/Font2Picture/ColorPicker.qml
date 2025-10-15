import QtQuick 2.4
import QtQuick.Dialogs 1.1
Item {
    id: root
    property color color

    Binding on color {
        value: colorDialog.currentColor
        when: colorDialog.visible
    }

    Rectangle {
        id: rect
        anchors.fill: parent
        radius: 3
        border.width: 1
        color: colorDialog.visible ? colorDialog.currentColor : root.color
//        anchors.margins: 4

        ColorDialog {
            id: colorDialog
            onAccepted: root.color = color
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                colorDialog.color = root.color
                colorDialog.open()
            }
        }
    }
}
