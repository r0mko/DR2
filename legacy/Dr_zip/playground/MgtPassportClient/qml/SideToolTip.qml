import QtQuick 2.5
import QtGraphicalEffects 1.0
Item {
    id: root
    property alias text: textItem.text
    property Item item: parent
    y: item.height * 0.5 - height * 0.5
    width: textItem.contentWidth + height * 1.5
    height: 50

    state: 'hidden'
    states: [
        State {
            name: 'hidden'
            PropertyChanges {
                target: root
                x: root.item.width + 400
                opacity: 0
            }
        },
        State {
            name: 'shown'
            PropertyChanges {
                target: root
                x: root.item.width
                opacity: 1
            }
        }
    ]
    transitions: [
        Transition {
            from: "hidden"
            to: "shown"
            NumberAnimation { target: root; property: 'x'; duration: 200; easing.type: Easing.OutElastic }
            NumberAnimation { target: root; property: 'opacity'; duration: 20 }
        },
        Transition {
            from: "shown"
            to: "hidden"
            NumberAnimation { target: root; property: 'opacity'; duration: 100 }
        }
    ]
    Behavior on width { NumberAnimation { duration: 100; easing.type: Easing.OutElastic } }
    function show( msg ) {
        root.text = msg
        root.state = 'shown'
    }
    function hide() {
        root.text = ''
        root.state = 'hidden'
    }

    Item {
        id: bg
        visible: false
        anchors.fill: parent
        Rectangle {
            anchors.right: parent.right
            width: parent.height
            height: parent.height
            radius: parent.height
        }
        Rectangle {
            id: arr
            x: ( parent.height - width ) * 0.5
            y: x
            width : Math.sqrt( 2 ) * parent.height * 0.5
            height: width
            transformOrigin: Item.Center
            rotation: 45
        }
        Rectangle {
            x: parent.height * 0.5
            height: parent.height
            width: parent.width - parent.height
        }
    }
    DropShadow {
        anchors.fill: bg
        source: bg
        horizontalOffset: 3
        verticalOffset: 7
        fast: false
        radius: 12
        samples: 32
        color: "#80000000"
        transparentBorder: true
    }
    Text {
        id: textItem
        anchors.fill: parent
        anchors.leftMargin: parent.height * 0.5
        anchors.rightMargin: parent.height * 0.5
        anchors.bottomMargin: 2
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: parent.height * 0.3
    }
}

