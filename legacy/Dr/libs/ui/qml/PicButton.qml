import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Button {
    implicitWidth: 24
    implicitHeight: 24
    style: ButtonStyle {
        label: Image {
            source: control.iconSource
            sourceSize: Qt.size(control.implicitWidth - 4, control.implicitHeight - 4)
            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit
            opacity: control.enabled ? 1.0 : 0.6
        }
    }
}
