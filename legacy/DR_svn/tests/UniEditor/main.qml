import QtQuick 2.3
import QtQuick.Controls 1.2
import DataReactor.UI 1.0
import DataReactor 1.0
import DataReactor.Graphics 1.0
import QtQuick.Layouts 1.1

ApplicationWindow {
    visible: true
    width: 1200
    height: 900
    title: qsTr("Editor")
    color: "#eee"

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("&Open")
                onTriggered: console.log("Open action triggered");
            }
            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit();
            }
        }
    }
    toolBar: ThinToolbar {
        implicitHeight: 20
        height: 24
        ThinToolButton {
            width: 32
            Symbol {
                anchors.fill: parent
                font: Symbol.FontAwesome
                symbol: FontAwesome.DotCircleO
                pointSize: 18
            }
        }
    }

    SplitView {
        anchors.fill: parent
        Flickable {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
