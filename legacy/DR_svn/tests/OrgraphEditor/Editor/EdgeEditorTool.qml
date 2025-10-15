import QtQuick 2.3
import QtQuick.Controls 1.2
import DataReactor.UI 1.0
import "../"

ToolBase {
    id: root
    property Node nodeFrom: null
    property Node nodeHover: null

    property real mx
    property real my
    property real offx: 0
    property real offy: 0

    action: Action {
        id: a_editEdges
        checkable: true
        onCheckedChanged: console.log("state", state.name, checked)
    }

    toolbarControl: ThinToolButton {
        id: b_editRibs
        onClicked: action.checked ^= true
        checked: action.checked
        //                checkable: true
        width: 36
        Symbol {
            anchors.fill: parent
            font: Symbol.FontAwesome
            symbol: FontAwesome.CodeFork
            pixelSize: 20
        }
    }


    states: [
        State {
            name: "editedges"
            when: action.checked
            PropertyChanges {
                target: nodesView
                nodeSize: 30
                nodeRadius: 15
                lockNodes: true
                opacity: 0.5
                //                    cursorShape: Qt.CrossCursor
                cursorShape: Qt.DragMoveCursor
                pressNode: function(index, mouse) {
                    nodeFrom = datamodel.nodes.get(index).i
                    offx = mouse.x
                    offy = mouse.y
                }

                releaseNode: function(index) {
                    if (nodeFrom && nodeHover)
                        graph.addEdge(root.nodeFrom, root.nodeHover)
                    editor.hud.requestPaint()
                    nodeFrom = null
                    nodeHover = null
                    offx = 0
                    offy = 0
                }

                mouseMove: function(index, mouse) {
                    if (mouse.buttons & Qt.LeftButton && root.nodeFrom) {
                        mx = mouse.x + nodeFrom.x - offx
                        my = mouse.y + nodeFrom.y - offy
                        var h = nodesView.childAt(root.mx, root.my)
                        if (h && typeof h.nodeIdx !== 'undefined') {
                            nodeHover = datamodel.nodes.get(h.nodeIdx).i
                        } else {
                            nodeHover = null
                        }
                        editor.hud.requestPaint()
                    }
                }
            }

            PropertyChanges {
                target: editor.hud
                paint: function(ctx) {
                    if(root.nodeFrom) {
                        ctx.beginPath()
                        ctx.moveTo(root.nodeFrom.x, root.nodeFrom.y)
                        if (root.nodeHover) {
                            ctx.strokeStyle = Qt.rgba(0, 1, 1, 1)
                            ctx.lineWidth = 2
                            ctx.lineTo(root.nodeHover.x, root.nodeHover.y)
                        } else {
                            ctx.strokeStyle = Qt.rgba(1, 1, 1, 0.5)
                            ctx.lineWidth = 0.7
                            ctx.lineTo(root.mx, root.my)

                        }
                        ctx.stroke()
                    }
                }

            }

            PropertyChanges {
                target: nodesList
                currentIndex: -1
            }

        }
    ]

}
