import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.2

Button {
    id: root
    text: 'label'
//    anchors.verticalCenter: parent
//    anchors.topMargin: 2
//    anchors.bottomMargin: 2

    property int fontSize: 20
    property int margin: 5
    style: ButtonStyle {
        label: RowLayout {
            anchors.fill: parent
            Item{ width: control.margin }
            Text {
                Layout.fillWidth: true
                Layout.fillHeight: true
                text: control.text
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: control.fontSize
            }
            Item{ width: control.margin }
        }
    }
}
