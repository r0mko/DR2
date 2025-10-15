import QtQuick 2.5

Rectangle {
    property string text: "."
    width: 100
    height: 100
    color: "green"
    Text {
        id: txt
        text: parent.text
    }
}

