import QtQuick 2.3
import QtQuick.Layouts 1.1

Item {
    id: root
    property alias model: m_repeater.model
    property alias delegate: m_repeater.delegate
    property alias scrollArea: m_propview
    property alias listItem: m_listItem
    property alias spacing: m_listItem.spacing
    implicitHeight: m_listItem.height
    implicitWidth: m_listItem.width

    MouseArea {
        id: m_mouse
        anchors.fill: parent
        acceptedButtons: Qt.NoButton
        property real velocity: 0

        NumberAnimation on velocity {
            id: m_decel
            easing.type: Easing.OutExpo
            to: 0
            duration: 300
            onStopped: {
                //                            console.log("Stopped")
                m_propview.contentY = Math.round(m_propview.contentY)
            }
        }

        SpringAnimation {
            id: m_rebound
            target: m_propview
            property: "contentY"
            running: false
            spring: 3
            damping: 0.4
            epsilon: 0.7
            alwaysRunToEnd: false
            onStarted: {
                if(m_mouse.velocity / m_propview.contentY < 0) {
                    m_mouse.velocity = 0
                }
            }
        }

        onVelocityChanged: {
//            console.log("velocity", velocity)
            m_propview.contentY -= velocity / 10

            if (m_propview.contentY < 0) {
                if (!m_rebound.running) {
                    m_rebound.to = 0
                    m_rebound.restart()
                }
            } else if (m_propview.contentY >= m_propview.contentHeight - m_propview.height) {
                if (!m_rebound.running) {
                    m_rebound.to = m_propview.contentHeight - m_propview.height
                    m_rebound.restart()
                }
            }
        }

        onWheel: {
            if (m_propview.height >= m_propview.contentHeight) {
                m_propview.returnToBounds()
                return
            }
            if (m_propview.contentY <= 0) {
                if (wheel.angleDelta.y > 0) {
                    return
                }
            } else if (m_propview.contentY >= m_propview.contentHeight - m_propview.height) {
                if (wheel.angleDelta.y < 0)
                    return
            } else if (m_rebound.running)
                m_rebound.stop()

            velocity += wheel.angleDelta.y * m_propview.height / 500
            if((velocity / wheel.angleDelta.y) < 0) {
//                console.log("reverse", velocity, wheel.angleDelta.y)
                velocity += wheel.angleDelta.y * 3
            }

            m_decel.restart()
        }
    }
    Flickable {
        id: m_propview
        anchors.fill: parent
        interactive: false
        clip: true
        Column {
            id: m_listItem
            width: m_propview.width
            onHeightChanged: {
//                console.log("h=", height)
                m_propview.resizeContent(width, height, 0)
            }
            Repeater {
                id: m_repeater
            }
        }

    }

}
