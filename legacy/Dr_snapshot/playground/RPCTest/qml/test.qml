import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import Qt.labs.settings 1.0
import QtQuick.Window 2.2
import Dynagraph.Window 1.2
import DataReactor.UI 1.0
ApplicationWindow {
    id: appwindow
    visible: true
    width: 1080
    height: 760


    Item {
        id: root
        anchors.fill: parent
        anchors.margins: 10

        Rectangle {
            id: dpanel
            width: 200
            height: parent.height
            color: "#80004000"
            Column {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 8
                Repeater {
                    model: 6
                    delegate: Rectangle {
                        id: content
                        property DGWindow win: null
                        width: parent.width
                        height: Math.random() * 100 + 50
                        color: Qt.hsla(Math.random(), 0.6, 0.8, 1.0)

                        states: [
                            State {
                                name: "undocked"
                                ParentChange {
                                    target: content
                                    parent: content.win.contentItem
                                    x: 0
                                    y: 0
                                }
                            }
                        ]
                        Column {
                            anchors.fill: parent
                            Text {
                                width: parent.width
                                height: 30
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                text: "Item " + index + "\nHeight: " + Number(parent.height).toLocaleString()
                            }
                            Button {
                                id: spawnBtn
                                text: UI.getChar(FontAwesome.Gear)
                                enabled: content.state !== "undocked"
                                onClicked: {
                                    var c = spawnBtn.mapToItem(null, 0, 0)
                                    content.win = dockwincomp.createObject(appwindow, {x: c.x + appwindow.x, y: c.y+appwindow.y, width: content.width, height: content.height, dock: dpanel, parentWindow: appwindow})
                                    content.win.show()
                                    content.state = "undocked"
                                    appwindow.alert(500)
                                }
                            }
                        }

                    }
                }
            }


            Component {
                id: dockwincomp
                DGWindow {
                    id: fltwin
                    property Window parentWindow
                    property Item dock

                    flags: Qt.Tool
                    onClosing: {
                        contentItem.children[0].state = ""
                    }
                    onXChanged: console.log("x", x)
                    onYChanged: console.log("y", y)

                }
            }
        }
    }
}
