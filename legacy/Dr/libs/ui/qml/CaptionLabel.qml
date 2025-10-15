import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import Dr.MetaGraph 1.0

Item {
    property alias backgroundColor: _bkg.color
    property alias text: _lbl.text
    property alias font: _lbl.font
    property int level: 1

    height: 36 - level * 4
    clip: true

    Rectangle {
        id: _bkg
        anchors.fill: parent
        color: "#aacaec"
        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.tint(_bkg.color, "#50ffffff") }
            GradientStop { position: 1.0; color: _bkg.color }
        }
    }

    Label {
        id: _lbl
        font.pixelSize: 22 - level * 2
        anchors.left: parent.left
        anchors.leftMargin: 5
        anchors.verticalCenter: parent.verticalCenter
        font.family: "Roboto"
        font.weight: Font.Light
    }
}
