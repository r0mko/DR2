import QtQuick 2.3
import QtGraphicalEffects 1.0

Rectangle {
    id: root
    property alias font: label.font
    property alias text: label.text
    property alias enabled: mousearea.enabled
    property bool checkable: false
    property bool checked: false
    property bool pressed: mousearea.pressed
    signal clicked(var mouse)
    signal pressed(var mouse)
    signal released(var mouse)
    signal entered
    signal exited
    implicitHeight: parent.height
    implicitWidth: 22
    radius: 2
    border.width: 1
    border.color: "#bbb"
    color: "transparent"
    clip: true

    Item {
        id: shadow
        anchors.fill: parent
        visible: false

        Rectangle {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: 3
            color: "#80000000"
        }
    }

    GaussianBlur {
        id: shadoweffect
        source: shadow
        radius: 3
        samples: 16
        cached: true
        anchors.fill: parent
        visible: false
    }

    Text {
        id: label
        anchors.fill: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    states: [
        State {
            name: "normal"
            PropertyChanges {
                target: root
                color: "transparent"
            }
        },
        State {
            name: "pressed"
            PropertyChanges {
                target: root
                color: "#20000000"
            }
            when: mousearea.pressed
        },
        State {
            name: "checked"
            when: root.checked // == true
            PropertyChanges {
                target: root
                color: "#80aeF0FF"
//                color: "#40008080"
            }
            PropertyChanges {
                target: shadoweffect
                visible: true
            }
        },
        State {
            name: "hovered"
            PropertyChanges {
                target: root
                color: "#40aeF0FF"
            }
            when: mousearea.containsMouse
        },
        State {
            name: "disabled"
            PropertyChanges {
                target: root
                color: "#20000000"
            }
            PropertyChanges {
                target: label
                color: "#808080"
            }
            when: !mousearea.enabled
        }

    ]

    MouseArea {
        id: mousearea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton
        onClicked: { root.clicked(mouse); if (root.checkable) root.checked = !root.checked }
    }
}
