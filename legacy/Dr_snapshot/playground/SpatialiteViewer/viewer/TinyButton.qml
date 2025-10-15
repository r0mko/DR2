import QtQuick 2.5
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.2


Button {
    id: root
    implicitHeight: height ? height : 24
    implicitWidth: width ? width : 24
    property color buttonColor: "transparent"
    property color hoverColor: "white"
    property Component icon
    property bool flat: true
    property int radius

    style: ButtonStyle {
        background: Item {
            property alias iconLoader: ico
            property bool down: control.pressed || (control.checkable && control.checked)
            implicitWidth: root.implicitWidth
            implicitHeight: root.implicitHeight
            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: down ? 2 : 0 // shadow
                color: "#40000000"
                radius: baserect.radius
//                border.width: flat ? 0 : 1
//                border.color: "#20000000"
            }
            Rectangle {
                id: baserect
                color: buttonColor
                anchors.fill: parent
                radius: root.radius

                Rectangle {
                    gradient: Gradient {
                        GradientStop { color: down ? "#aaa" : "#fefefe" ; position: 0}
                        GradientStop { color: down ? "#ccc" : "#e3e3e3" ; position: down ? 0.1: 1}
                    }
                    radius: baserect.radius
                    anchors.fill: parent
                    border.color: control.activeFocus ? "#47b" : "#888"
                    visible: !flat
                }

                Rectangle {
                    anchors.fill: parent
                    radius: parent.radius
                    color: control.activeFocus ? "#47b" : hoverColor
                    opacity: control.hovered ? 0.8 : control.activeFocus ? 0.3 : 0
                    Behavior on opacity {NumberAnimation{ duration: 100 }}
                }
            }
            Loader {
                id: ico
                opacity: control.enabled ? 1.0 : 0.5
                anchors.fill: parent
//                anchors.
                sourceComponent: root.icon
            }
        }
        //        label: control.label
    }
}
