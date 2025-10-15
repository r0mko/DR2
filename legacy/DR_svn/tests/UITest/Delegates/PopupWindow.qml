import QtQuick 2.3
import QtQuick.Window 2.1
import QtGraphicalEffects 1.0


Window {
    id: rootwin
    color: "transparent"
    opacity: 0.85
    flags: Qt.ToolTip
    property Item content: Item {}
    property int tailSize: 16
    property int xPos
    property int yPos
    x: xPos - root.width / 2
    y: yPos - root.height
    width: calcWidth()
    height: calcHeight()
    onContentChanged: {
        width = calcWidth()
        height = calcHeight()
        console.log("Content updated", content.width, content.height, width, height)

    }
    function calcWidth() {
        return content.width + body.anchors.leftMargin - body.anchors.rightMargin + cnt.anchors.leftMargin + cnt.anchors.rightMargin
    }

    function calcHeight() {
        return content.height + body.anchors.bottomMargin + body.anchors.topMargin + cnt.anchors.bottomMargin + cnt.anchors.topMargin
    }

    Item {
        id: root
        anchors.fill: parent

        Rectangle {
            id: tail
            width: tailSize
            height: tailSize
            color: "black"
            x: body.width / 2
            y: body.y + body.height
            rotation: 45
            transform: Translate {
                x: -tail.width / 2
                y: -tail.height / 2
            }
        }

        Rectangle {
            id: body
            anchors.fill: parent
            anchors.topMargin: 2
            anchors.leftMargin: 2
            anchors.rightMargin: 2
            anchors.bottomMargin: tailSize
            radius: 8
            color: "black"
            Item {
                id: cnt
//                anchors.centerIn: body\
                anchors.fill: body
                anchors.margins: 10
                data: [content]
            }
        }
    }

    Glow {
        cached: true
        anchors.fill: root
        samples: 8
        source: root
        radius: 2
        spread: 1.0
        color: "#dddddd"

    }


}
