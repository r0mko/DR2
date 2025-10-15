import QtQuick 2.0

Component{
    id: colorDelegate
    Rectangle{
        anchors.fill: parent
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        color: modelData.value
        border.width: 1
        border.color: '#000'
        smooth: true
        antialiasing: true
    }
}
