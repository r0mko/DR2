import QtQuick 2.3
import QtQuick.Controls 1.2

Rectangle {
    id: root
    property bool checked: false
    property ExclusiveGroup exclusiveGroup: null

    height: 36
    color: marea.containsMouse ? "#234" : "#222"
    border.width: 2
    border.color: checked ? "#FFAD00" : "black"

    onExclusiveGroupChanged: {
        if (exclusiveGroup)
            exclusiveGroup.bindCheckable(root)
    }

    MouseArea {
        id: marea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: checked ^= true
    }
}
