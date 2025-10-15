import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import DataReactor 1.0
import DataReactor.UI 1.0
//import "InfoPanel" as InfoPanel

ApplicationWindow {
    id: mainwindow
    visible: true
    width: 1280
    height: 1050
    title: qsTr("Hello World")

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

    Component.onCompleted: {
        wires.append({i: circ1})
        wires.append({i: circ2})
        wires.append({i: circ3})
        wires.append({i: circ4})
        console.log("nodes", nodes.children.length)
    }

    Object {
        id: nodes
        Node { id: n0; x: 100; y: 100; index: 0 }
        Node { id: n1; x: 750; y: 150; index: 1 }
        Node { id: n2; x: 400; y: 550; index: 2 }
        Node { id: n3; x: 300; y: 800; index: 3 }
        Node { id: n4; x: 950; y: 900; index: 4 }
        Node { id: n5; x: 200; y: 900; index: 5 }
        Node { id: n6; x: 300; y: 920; index: 6 }
        Node { id: n7; x: 400; y: 800; index: 7 }
        Node { id: n8; x: 600; y: 100; index: 8 }
        Node { id: n9; x: 100; y: 400; index: 9 }
    }

    ListModel {
        id: nodesModel
    }

    property real offset: offsetslider.value
    property real d: dslider.value
    property real w: wslider.value
    Circuit {
        id: circ1
        color: Theme.palette.mainSeq[0]
        WireSegment {
            begin.node: n0
            end.node: n1
            begin.offset: offset
            end.offset: offset
        }
        WireSegment {
            begin.node: n1
            end.node: n2
            begin.offset: offset
            end.offset: offset
        }
        WireSegment {
            begin.node: n2
            end.node: n4
            begin.offset: offset
            end.offset: offset
        }
        WireSegment {
            begin.node: n2
            end.node: n3
            begin.offset: offset
            end.offset: offset
        }
        WireSegment {
            begin.node: n4
            end.node: n7
            begin.offset: offset
            end.offset: offset
        }
        WireSegment {
            begin.node: n7
            end.node: n3
            begin.offset: offset
            end.offset: offset
        }
        WireSegment {
            begin.node: n3
            end.node: n5
            begin.offset: offset
            end.offset: offset
        }
        WireSegment {
            begin.node: n5
            end.node: n9
            begin.offset: offset
            end.offset: offset
        }
    }

    Circuit {
        id: circ2
        color: Theme.palette.mainSeq[2]
        WireSegment {
            begin.node: n6
            end.node: n3
            begin.offset: offset
            end.offset: offset
        }
        WireSegment {
            begin.node: n3
            end.node: n2
            begin.offset: offset
            end.offset: offset
        }
        WireSegment {
            begin.node: n2
            end.node: n1
            begin.offset: offset
            end.offset: offset
        }
        WireSegment {
            begin.node: n1
            end.node: n0
            begin.offset: offset
            end.offset: offset
        }
        WireSegment {
            begin.node: n1
            end.node: n8
            begin.offset: offset
            end.offset: offset
        }
        WireSegment {
            begin.node: n4
            end.node: n2
            begin.offset: offset
            end.offset: offset
        }
    }



    Circuit {
        id: circ3
        color: Theme.palette.mainSeq[7]
        WireSegment {
            begin.node: n6
            end.node: n3
            begin.offset: offset + d
            end.offset: offset + d
        }
        WireSegment {
            begin.node: n3
            end.node: n2
            begin.offset: offset + d
            end.offset: offset + d
        }
        WireSegment {
            begin.node: n2
            end.node: n1
            begin.offset: offset + d
            end.offset: offset + d
        }
        WireSegment {
            begin.node: n1
            end.node: n0
            begin.offset: offset + d
            end.offset: offset + d
        }
        WireSegment {
            begin.node: n1
            end.node: n8
            begin.offset: offset + d
            end.offset: offset + d
        }
        WireSegment {
            begin.node: n4
            end.node: n2
            begin.offset: offset + d
            end.offset: offset + d
        }
    }

    Circuit {
        id: circ4
        color: Theme.palette.midSeq[4]
        WireSegment {
            begin.node: n6
            end.node: n3
            begin.offset: offset*2
            end.offset: offset*2

        }
        WireSegment {
            begin.node: n5
            end.node: n3
            begin.offset: offset*2
            end.offset: offset*2

        }
        WireSegment {
            begin.node: n3
            end.node: n2
            begin.offset: offset*2
            end.offset: offset*2
        }
        WireSegment {
            begin.node: n2
            end.node: n1
            begin.offset: offset*2
            end.offset: offset*2
        }
        WireSegment {
            begin.node: n1
            end.node: n8
            begin.offset: offset*2
            end.offset: offset*2
        }
        WireSegment {
            begin.node: n1
            end.node: n0
            begin.offset: offset*2
            end.offset: offset*2
        }

    }

    RowLayout {
        anchors.fill: parent
        anchors.rightMargin: 5
        Rectangle {
            id: drawboard
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#333"
            Layout.preferredWidth: 1200
//            Layout.preferredHeight: viewItem.canvasSize.height
            signal handlesMoved
            onHandlesMoved: {
//                console.log("c: %1, kpos: %2, kneg: %3".arg(cvalue.value).arg(kposvalue.value).arg(knegvalue.value))
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                onClicked: {
                    nodesModel.append({x: mouseX, y: mouseY})
                }
            }

            Repeater {
                id: wireView
                anchors.fill: parent
                model: wires
                delegate: CircuitDelegate {
                    anchors.fill: wireView
                    circuit: i

                }
            }

            Repeater {
                id: handles
                anchors.fill: parent
//                model: nodesModel
                model: nodes.children
                Rectangle {
                    id: rect
                    opacity: mArea.containsMouse || mArea.drag.active ? 1 : 0.5
                    onXChanged: {
                        modelData.x = x
                        drawboard.handlesMoved()
                    }
                    onYChanged: {
                        modelData.y = y
                        drawboard.handlesMoved()
                    }

                    width: 14
                    height: 14
                    radius: 7
                    color: "transparent"
                    border.width: 1
                    border.color: mArea.containsMouse || mArea.drag.active ? "#00FFFF" : "white"

                    Behavior on border.color {
                        ColorAnimation { duration: 200 }
                    }
                    Behavior on opacity {
                        NumberAnimation { duration: 200 }
                    }

                    transform: Translate {
                        x: -rect.width / 2
                        y: -rect.height / 2
                    }

                    Text {
                        anchors.fill: parent
                        color: rect.border.color
                        font.pixelSize: 12
                        text: modelData.index
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                    }

                    MouseArea {
                        id: mArea
                        anchors.fill: parent
                        hoverEnabled: true
                        drag.target: rect
                        drag.threshold: 0
                    }

                    Component.onCompleted: {
                        x = modelData.x
                        y = modelData.y
                    }

                }

            }


            Repeater {
                id: nodesView
                anchors.fill: parent
                model: nodesModel
                delegate: nodeDelegate

            }
        }

        ColumnLayout {
            Layout.fillHeight: true
            Layout.preferredWidth: 300
            spacing: 1
            Item {
                height: 3
            }
            Label {
                text: "Scale"
            }

            Slider {
                id: cvalue
                Layout.fillWidth: true
                minimumValue: 1
                maximumValue: 5
                value: 1
                onValueChanged: {
                    drawboard.handlesMoved()
                }
            }
            Label {
                text: "KPos"
            }
            Slider {
                id: kposvalue
                Layout.fillWidth: true
                minimumValue: 0
                maximumValue: 10
                value: 2
                onValueChanged: drawboard.handlesMoved()
            }
            Label {
                text: "KNeg"
            }
            Slider {
                id: knegvalue
                Layout.fillWidth: true
                minimumValue: -10
                maximumValue: 0
                value: 0
                onValueChanged: drawboard.handlesMoved()
            }
            Label {
                text: "Offset"
            }
            Slider {
                id: offsetslider
                Layout.fillWidth: true
                minimumValue: 0
                maximumValue: 30
                value: 16
                onValueChanged: drawboard.handlesMoved()
            }
            Label {
                text: "Dst"
            }
            Slider {
                id: dslider
                Layout.fillWidth: true
                minimumValue: 0
                maximumValue: 10
                value: 4
                onValueChanged: drawboard.handlesMoved()
            }
            Label {
                text: "Width"
            }
            Slider {
                id: wslider
                Layout.fillWidth: true
                minimumValue: 0
                maximumValue: 10
                value: 4
                onValueChanged: drawboard.handlesMoved()
            }


//            ToolButton {
//                Layout.fillWidth: true
//                id: addNodesButton
//                text: canvas.addNodesAction.text
//                checkable: true
//                checked: canvas.addNodesAction.checked
//                onClicked: canvas.addNodesAction.trigger()
//            }
//            RowLayout {
//                Layout.fillWidth: true
//                ToolButton {
//                    id: addWireButton
//                    Layout.fillWidth: true
//                    text: canvas.addWireAction.text
//                    checkable: true
//                    checked: canvas.addWireAction.checked
//                    onClicked: canvas.addWireAction.checked = checked
//                    Connections {
//                        target: canvas.addWireAction
//                        onToggled: addWireButton.checked = checked
//                    }
//                }

//                Rectangle {
//                    id: wireColor
//                    border.width: 1
//                    width: addWireButton.height
//                    height: addWireButton.height
//                    border.color: "black"
//                    color: canvas.currentColor

//                    ColorDialog {
//                        id: colpick
//                        color: wireColor.color
//                        onColorChanged: canvas.currentColor = color
//                    }

//                    MouseArea {
//                        anchors.fill: parent
//                        onClicked: colpick.visible = true
//                    }
//                }
//            }

//            ListWidget {
//                id: info
//                Layout.fillWidth: true
//                Layout.fillHeight: true

//                model: canvas.model

//                delegate: InfoPanel.WireDelegate {
//                    width: info.listItem.width
//                    exclusiveGroup: selection
//                    onCheckedChanged: if (checked) canvas.highlight(index)
//                }
//            }

        }
    }
    ExclusiveGroup {
        id: selection
    }

    Component {
        id: nodeDelegate
        Rectangle {
            id: nd
            width: 20
            height: 20

        }
    }

    ListModel {
        id: wires
    }
    function typeDetails(objectString) {
        var rx = new RegExp("([A-Z_][A-Za-z0-9_]+?)(_QMLTYPE_\\d+)?(_QML_\\d+)?\\(0x[A-Fa-f0-9]+\\)")
        return rx.exec(objectString)
    }

    function getTypeName(object) {
        return typeDetails(object.toString())[1]
    }
}
