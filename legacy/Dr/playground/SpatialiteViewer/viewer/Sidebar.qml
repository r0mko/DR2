import QtQuick 2.5
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtQml.Models 2.2

Rectangle {
    id: root
    default property alias __data: column.data
    color: palette.window

    ScrollView {
        id: sv
        anchors.fill: parent
        contentItem: Rectangle {
            width: sv.viewport.width
            height: column.height
            color: "#888"
//            SplitView {
//                id: column
//                width: parent.width
//                orientation: Qt.Vertical
//                height: childrenRect.height
//                onChildrenChanged: console.log( 'childrenChanged', children.length, height, implicitHeight, childrenRect.width, childrenRect.height )
//                onHeightChanged: console.log( 'height changed', height )
//            }

            ColumnLayout {
                id: column
                width: parent.width
                spacing: 1
            }
        }
    }

    SystemPalette { id: palette }

}

