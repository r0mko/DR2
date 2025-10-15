import QtQuick 2.3
import QtQuick.Layouts 1.1
import DataReactor.UI 1.0
Rectangle {
    id: bkgrnd
    color: "#222"
    property alias model: repeater.model
    property alias delegate: repeater.delegate
    property alias scrollArea: propview
    property alias listItem: listItem
    Flickable {
        id: propview
        anchors.fill: parent
        interactive: false
        clip: true
        Column {
            id: listItem
            width: propview.width
            onHeightChanged: propview.resizeContent(width, height, 0)
            Repeater {
                id: repeater
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        property real velocity: 0

        NumberAnimation on velocity {
            id: decel
            easing.type: Easing.OutExpo
            to: 0
            duration: 300
            onStopped: {
                //                            console.log("Stopped")
                propview.contentY = Math.round(propview.contentY)
            }
        }

        SpringAnimation {
            id: rebound
            target: propview
            property: "contentY"
            running: false
            spring: 3
            damping: 0.4
            epsilon: 0.7
            alwaysRunToEnd: false
            onStarted: {
                if(mouseArea.velocity / propview.contentY < 0) {
                    mouseArea.velocity = 0
                }
            }
        }

        onVelocityChanged: {
            //                        console.log("velocity", velocity)
            propview.contentY -= velocity / 10

            if (propview.contentY < 0) {
                if (!rebound.running) {
                    rebound.to = 0
                    rebound.restart()
                }
            } else if (propview.contentY >= propview.contentHeight - propview.height) {
                if (!rebound.running) {
                    rebound.to = propview.contentHeight - propview.height
                    rebound.restart()
                }
            }
        }

        onWheel: {
            if (propview.height >= propview.contentHeight) {
                propview.returnToBounds()
                return
            }
            if (propview.contentY <= 0) {
                if (wheel.angleDelta.y > 0) {
                    return
                }
            } else if (propview.contentY >= propview.contentHeight - propview.height) {
                if (wheel.angleDelta.y < 0)
                    return
            } else if (rebound.running)
                rebound.stop()

            velocity += wheel.angleDelta.y * 3
            if((velocity / wheel.angleDelta.y) < 0) {
                console.log("reverse", velocity, wheel.angleDelta.y)
                velocity += wheel.angleDelta.y * 3
            }

            decel.restart()
        }
    }


}
