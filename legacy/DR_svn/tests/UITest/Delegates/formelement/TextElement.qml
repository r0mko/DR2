import QtQuick 2.3

Rectangle {
    property alias text: text.text
    color: "transparent"
//    width:
        anchors.fill: parent
    Text {
        id: text
        anchors.fill: parent
        font.family: "Roboto"
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignRight
        color: "#7c1010"
    }
}
