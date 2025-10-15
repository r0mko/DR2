import QtQuick 2.3
import Dr.Graphics 1.0
import Dr.UI 1.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2

SmoothWire {
    id: root
    property Component panelComponent: ColumnLayout {
        id: panel
        property int fontSize: 10
        Text{
            text: String( root )
            font.pixelSize: fontSize + 2
        }
        Label{
            text: 'Отступ'
            font.pixelSize: fontSize
        }
        TinySlider{
            value: root.offset
            onValueChanged: root.offset = value
            stepSize: 0.5
            maximumValue: 20
        }

        Row {
            spacing: 4
            Column {
                spacing: 2
                Label{
                    text: 'Цвет'
                    font.pixelSize: fontSize
                }
                ColorDialog {
                    id: colorPicker
                    onColorChanged: root.pen.color = color
                    color: root.pen.color
                }
                Rectangle {
                    id: ribColor
                    width: 30
                    height: 18//colorText.implicitHeight
                    border.width: 1
                    border.color: "black"
                    color: root.pen.color
                    radius: 4
                    MouseArea {
                        anchors.fill: parent
                        onClicked: colorPicker.open()
                    }
                }
            }
            ToolButton {
                id: b_branch
                text: "Branch"
                checkable: true
                //                onCheckedChanged: {
                //                    if (checked)
                //                        root.states.state = "insertTransition"
                //                    else
                //                        root.states.state = ""
                //                }
            }
            ToolButton {
                id: b_split
                text: "Split"
                checkable: true
                //                onCheckedChanged: {
                //                    if (checked)
                //                        root.states.state = "insertSplit"
                //                    else
                //                        root.states.state = ""
                //                }

            }
        }
        states: [
            State {
                name:"selectPos"

                PropertyChanges {
                    target: root
                    hoverSlider.guide: guide
                    onHoverPositionChanged: {
                        var p = guide.nearestPos(pos)
                        console.log("pos", pos.x, pos.y, p, hoverSlider.runner.position)
                        hoverSlider.position = p
                    }
                }

                PropertyChanges {
                    target: root.hoverSlider
                    handle.enabled: false
                    handle.visible: root.isHovered
                    handle.width: 3
                    handle.height: 3 * root.outlineMargin
                    onPositionChanged: {
                        var offs = root.offset * root.pen.width * (root.fixedScale ? 1 : root.zoomFactor)
                        var wp = runner.getWirePoint(root.radius, offs)
                        console.log(wp.point.x)
                        handle.x = wp.point.x
                        handle.y = wp.point.y
                        handle.rotation = wp.angle
                    }
                }
            },
            State {
                name: "insertTransition"
                extend: "selectPos"
                when: b_branch.checked
                PropertyChanges {
                    target: root
                    onPressed: {
                        var len = guide.nearestPos(pos)
                        m_wireBuilder.insertTransition(index, len)
                    }
                }
            },
            State {
                name: "insertSplit"
                extend: "selectPos"
                when: b_split.checked
                PropertyChanges {
                    target: root
                    onPressed: {
                        var len = guide.nearestPos(pos)
                        m_wireBuilder.insertTerminator(index, len)
                    }
                }
            }
        ]
    }


    outlineMargin: 12
    offset: wireOffset
    z: 5

    guide: Groove {
        id: guide
        nodeModel: nodes
    }
    drawOutline: root.isHovered

    property RibSlider hoverSlider

    property GuideTool beginTool
    property GuideTool endTool


    onBeginToolChanged: {
        if (beginTool && beginTool.hasOwnProperty("right")) {
            console.log(("change beginTool to"), beginTool)
            beginTool.right = root
        }
        if (beginTool && beginTool.hasOwnProperty("wire")) {
            console.log(("change beginTool to"), beginTool)
            beginTool.wire = root
            beginTool.runner.position = 0
            beginSliderConnections.target = beginTool
            beginTool.runner.refresh()
        }
    }
    onEndToolChanged: {
        if (endTool && endTool.hasOwnProperty("left")) {
            console.log(("change endTool to"), endTool)
            endTool.left = root
        }
        if (endTool && endTool.hasOwnProperty("wire")) {
            console.log(("change endTool to"), endTool)
            endTool.wire = root
            endTool.runner.position = root.guide.length
            endSliderConnections.target = endTool
            endTool.runner.refresh()

        }
    }
    Component.onCompleted: console.log("wire completed")
    Component.onDestruction: hideTools()

    flags: SceneItem.ItemIsSelectable
    hoverEnabled: true
    onSelectedChanged: {
        if (isSelected) {
            showTools()
        } else {
            hideTools()
        }
    }

    function showTools() {
        beginTool.handle.visible = true
        endTool.handle.visible = true
    }

    function hideTools() {
        beginTool.handle.visible = false
        endTool.handle.visible = false
    }

    property Connections endSliderConnections: Connections {
//        id: endSliderConnection
        target: null
        onPositionChanged: {
            root.endMargin = root.guide.length - endTool.position
        }
        onAtEndChanged: {
            console.log("end slider at end", endTool.ribModelIndex, endTool.ribPosition, endTool.reversed)
        }
    }
    property Connections beginSliderConnections: Connections {
        //        id: endSliderConnection
        target: null
        onPositionChanged: {
            root.beginMargin = beginTool.position
        }
        onAtEndChanged: {
            console.log("begin slider at start", beginTool.ribModelIndex, beginTool.ribPosition, beginTool.reversed)
        }
    }

}
