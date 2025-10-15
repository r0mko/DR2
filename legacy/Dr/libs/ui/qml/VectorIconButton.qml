import QtQuick 2.5
import QtQuick.Controls 1.3
import Dr.UI 1.0

TinyButton {
    id: root
    property var iconData
    property real iconVPadding: 4
    property real iconHPadding: 4
    property real iconRotation
    property color iconColor
    property int aspectRatioMode: Qt.KeepAspectRatio
    text:""
//    property alias
    onIconSourceChanged: {
        if (typeof iconData == 'object')
            __isBlob = true
        else if (typeof iconData == 'string')
            __isBlob = false
    }
    /*! \internal */
    property bool __isBlob

    icon: DRPicture {
        id: pic
        aspectRatio: root.aspectRatioMode
        sourceData: iconData
        rotation: iconRotation
        color: iconColor
        anchors.fill: parent
        anchors.leftMargin: iconHPadding
        anchors.rightMargin: iconHPadding
        anchors.topMargin: iconVPadding
        anchors.bottomMargin: iconVPadding
    }
}

