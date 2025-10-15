import QtQuick 2.5
import QtQuick.Controls 1.4
import Dr.UI 1.0
EditableText {
    id: root
    property bool isCurrent: styleData.selected

    property color tint: "#000"
    textColor: Qt.tint(styleData.textColor, tint)

//    mouseArea.onPressed: {

//        if (mouse.button === Qt.LeftButton) {
//            view.selection.clear()
//            view.selection.select(styleData.row)
//            view.currentRow = styleData.row
//        }
//    }

    Component.onCompleted: {
        console.log("row complete", styleData.row, styleData.value)
    }
}
