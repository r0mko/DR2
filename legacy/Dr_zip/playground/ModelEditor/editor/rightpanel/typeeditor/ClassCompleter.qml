import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.2
import Dr.MetaGraph 1.0
import QtQuick.Dialogs 1.2
import Dr.UI 1.0

TextFieldCompleter {
    suggestionDelegate: RowLayout {
        height: 16
        spacing: 0
        Item {
            width: parent.height
            height: width
            Rectangle {
                anchors.fill: parent
                anchors.margins: 3
                border.color: "#000"
                radius: 4
                border.width: 1
                color: modelData.hasCtor ? "#8df" : modelData.object ? "#fe4" : "#fff"
            }
        }
        
        Text {
            id: _text
            font: textField.font
            Layout.fillWidth: true
            text: modelData.typeName
            color: isCurrent ? "#fff" : "#000"
            horizontalAlignment: Text.AlignLeft
        }
    }

    onPrefixChanged: {
        if (prefix.length > 2) {
            applyFilter()
            if (suggestions.length < 10) {
                popupList()
            }
        }
    }
}
