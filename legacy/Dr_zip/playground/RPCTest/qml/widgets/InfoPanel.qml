import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    id: root
    width: 300
    height: 400
    DropShadow {
        anchors.fill: glow1
        source: glow1
        samples: 16
        verticalOffset: 2
        radius: 8
        cached: true
        color: "#a0000000"
    }

    Glow {
        id: glow1
        source: cont
        anchors.fill: cont
        color: "#80fffFFF"
        radius: 2
        samples: 16
        spread: 1.0
        visible: false
    }

    Item {
        id: cont
        anchors.fill: parent

        Item {
            id: wrapper
            anchors.fill: parent
            anchors.margins: 8

            Rectangle {
                id: angle
                transformOrigin: Item.TopLeft
                width: 30
                height: 30
                x: 50
                color: "#000"
                transform: Rotation {
                    angle: 45
                    origin.x: 0
                    origin.y: 0
                }

            }
            Rectangle {
                id: panel
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: angle.verticalCenter
                color: "#000000"
                radius: 8
            }

        }





    }
}
