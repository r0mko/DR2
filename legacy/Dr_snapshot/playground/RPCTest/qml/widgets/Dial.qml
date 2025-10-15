import QtQuick 2.0

Rectangle {
    property real size
    property real minValue: 0
    property real maxValue: 100
    property real value: 0
    property real handWidth: 3
    width: size
    height: size
    radius: size/2

    Text {
        x: 0
        y: size / 2
        width: size
        height: size / 2
        text: Math.round(value)
        font.pixelSize: 10
        color: "black"
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter

    }

    Rectangle {
        id: hand
        color: "black"
        property real val: (value - minValue) / (maxValue-minValue) * 360
        x: size / 2 - handWidth / 2
        y: 0
        width: handWidth
        height: size / 2
        transform: Rotation { origin.x: 0; origin.y: size / 2; angle: hand.val }

        Behavior on val { SpringAnimation { spring: 1.5; damping: 0.3; epsilon: 0.01; modulus: 360 }}
    }


}
