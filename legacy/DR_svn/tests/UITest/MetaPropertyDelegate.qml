import QtQuick 2.3
import QtQuick.Layouts 1.1
import DataReactor.UI 1.0

Item {
    width: uniParent.width
    height: 20

    RowLayout {
        anchors.fill: parent
        Text {
//            id: label
            width: 60
            Layout.fillHeight: true
            verticalAlignment: Text.AlignVCenter
            text: name
        }

        UniLoader {
//            id: val
            target: "formelement"
            Layout.fillHeight: true
            Layout.fillWidth: true
            value: modelData.value
        }
    }
}
