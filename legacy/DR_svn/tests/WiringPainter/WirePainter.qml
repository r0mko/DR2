import QtQuick 2.3
import QtQuick.Controls 1.2
import Qt.labs.settings 1.0
import "CircuitDelegate"
Canvas {
    id: drawboard
    //    canvasSize: Qt.size(1000, 1000)
    renderTarget: Canvas.FramebufferObject
    renderStrategy: Canvas.Cooperative
    property var nodes: [] // list<Node>
    property var nodes_var: [] // list<Node>

    property alias modes: wiresModel

    ListModel {
        id: wiresModel
    }

    property alias addNodesAction: addNodesMode
    property alias addWireAction: addWireMode

    property var clickCanvas: function(mouse) {}
    property var pressEsc: function() {}
    property var pressReturn: function() {}
    property var pressEnter: function() {}

    property var clickHandle: function(handle) { selectNode(handle) }
    property var pressHandle: function(index) {}
    property var releaseHandle: function(index) {}
    property bool nodesMovable: true

    property MouseArea currentMouseArea: null

    property ExclusiveGroup exclusiveGroup

    property color currentColor

    property Wire currentWire: null

    Keys.onEscapePressed: pressEsc()
    Keys.onReturnPressed: pressReturn()
    Keys.onEnterPressed: pressEnter()

    onPaint: {
        var ctx = drawboard.getContext("2d")
        //        console.log("paint", JSON.stringify(ctx))
        ctx.clearRect(0, 0, width, height)
        ctx.lineWidth = 2
        for (var i = 0; i < model.length; ++i) {
            var wire = model[i]
            ctx.strokeStyle = wire.color
            ctx.beginPath()
//            console.log("Wire color", wire.color)
            for (var j = 0; j < wire.segments.length; ++j) {
                wire.segments[j].strokeAxis(ctx)
            }
            ctx.stroke()
//            ctx.endPath()
        }
    }

    function addNode(x, y) {
        var handle = nodeHandleComp.createObject(drawboard, {x: x, y: y, exclusiveGroup: exclusiveGroup} )
        nodesModel.append({ i: handle.node })
        nodes.push(handle)
    }

    function deleteNode(index) {
        var node = nodes.splice(index, 1, null)[0]
        node.destroy()
        nodes_var[index] = null
        settings.nodes = nodes_var
    }

    function selectNode(handle) {
        if(nodesMovable) {
            handle.checked ^= true
        }
    }

    function addWireSegment(from, to) {
//        console.log("Adding segment %1-%2".arg(from).arg(to))
        var seg = segComp.createObject()
        seg.begin.node = nodes[from]
        seg.end.node = nodes[to]
        currentWire.segments.append({i: seg})
    }

    function acceptCurrentWire() {
        model.push(currentWire)
        var wires = []
        for (var i = 0; i < model.length; ++i) {
//            console.log("Wire %1: %2".arg(i).arg(model[i]))
            var wire = { color: model[i].color, segments: [], fillets: [] }
            var fillets = []
            for (var j = 0; j < model[i].segments.length; ++j) {
                var s = model[i].segments[j]
                var begin = { node: s.begin.node.index, position: s.begin.position, offset: s.begin.offset }
                var end = { node: s.end.node.index, position: s.end.position, offset: s.end.offset }
                wire.segments.push({ begin: begin, end: end })
            }
            wires.push(wire)
            modelChanged()
        }
//        console.log("Wires", JSON.stringify(wires, null, 2))
        settings.wires = wires
        cancelCurrentWire()
    }

    function cancelCurrentWire() {
        currentWire = null
        drawboard.requestPaint()
        hud.requestPaint()
        addWireAction.checked = false
    }

    function highlight(index) {
        hud.currentHighlighted = index
    }

    states: [
        State {
            name: "addnodes"
            when: addNodesAction.checked
            PropertyChanges {
                target: drawboard
                clickCanvas: function(mouse) {
                    addNode(mouse.x, mouse.y)
//                    nodes_var.push({ x: mouse.x, y: mouse.y })
//                    settings.nodes = nodes_var
//                    settings.nodeCount = drawboard.nodes.length
                }
                pressEsc: function() {
                    addNodesAction.checked = false
                }
            }
            PropertyChanges {
                target: marea
                enabled: true
                acceptedButtons: Qt.LeftButton
                cursorShape: Qt.CrossCursor
            }
        },
        State {
            name: "addWire"
            when: addWireAction.checked

            PropertyChanges {
                target: drawboard
                nodesMovable: false
                pressHandle: function(index) {
                    currentMouseArea = nodes[index].mouseArea
                    hud.startX = nodes[index].x
                    hud.startY = nodes[index].y
                    hud.offsetX = currentMouseArea.mouseX
                    hud.offsetY = currentMouseArea.mouseY
                }
                releaseHandle: function(index) {
                    if (hud.currentHovered >= 0 && hud.currentHovered != index) {
                        addWireSegment(index, hud.currentHovered)
                    }
                    currentMouseArea = null
                    hud.currentHovered = -1
                    hud.requestPaint()
                }
                pressEnter: acceptCurrentWire
                pressReturn: acceptCurrentWire
                pressEsc: cancelCurrentWire

            }

            PropertyChanges {
                target: exclusiveGroup
                current: null
            }



        }
    ]


    Canvas {
        id: hud
        anchors.fill: drawboard
        property real startX
        property real startY
        property real curX
        property real curY
        property real offsetX
        property real offsetY

        visible: drawboard.state !== "" || currentHighlighted >= 0

        property int currentHovered: -1
        property int currentHighlighted: -1
        onCurrentHighlightedChanged: requestPaint()

        onCurXChanged: {
            findNode()
            requestPaint()
        }
        onCurYChanged: {
            findNode()
            requestPaint()
        }

        Connections {
            target: currentMouseArea
            onMouseXChanged: { hud.curX = hud.startX + currentMouseArea.mouseX - hud.offsetX }
            onMouseYChanged: { hud.curY = hud.startY + currentMouseArea.mouseY - hud.offsetY }
        }

        function findNode() {
            var underMouse = drawboard.childAt(curX, curY)
            if (typeof underMouse.index !== 'undefined') {
                currentHovered = underMouse.index
            } else {
                currentHovered = -1
            }
        }

        onPaint: {
            var ctx = getContext('2d')
            ctx.clearRect(0, 0, width, height)
            ctx.beginPath()
            ctx.strokeStyle = currentColor
            ctx.lineWidth = 2
            if (drawboard.currentWire)
                for (var i = 0; i < drawboard.currentWire.segments.count; ++i) {
                    drawboard.currentWire.segments[i].strokeAxis(ctx)
                }
            if (currentMouseArea) {
            ctx.moveTo(startX, startY)
            if (pointer.visible)
                ctx.lineTo(pointer.x, pointer.y)
            else
                ctx.lineTo(curX, curY)
            }
            ctx.stroke()

            if (currentHighlighted >= 0) {
                var wire = drawboard.model[currentHighlighted]
                ctx.lineWidth = 6
                ctx.strokeStyle = Qt.rgba(0.4,0.9,1.0,0.6)
                ctx.beginPath()
                console.log("Highlighted wire %1".arg(currentHighlighted))
                for (var j = 0; j < wire.segments.length; ++j) {
                    wire.segments[j].strokeAxis(ctx)
                }
                ctx.stroke()
            }

        }

        Rectangle {
            id: pointer
            x: visible ? nodes[hud.currentHovered].x : 0
            y: visible ? nodes[hud.currentHovered].y : 0
            height: 30
            width: 30

            color: "transparent"
            border.width: 2
            border.color: "#ff2929"
            radius: 6
            visible: hud.currentHovered >= 0
            transform: Translate {
                x: -pointer.width / 2
                y: -pointer.height / 2
            }

        }


    }

    MouseArea {
        id: marea
        enabled: false
        acceptedButtons: Qt.NoButton
        anchors.fill: parent
        onClicked: drawboard.clickCanvas(mouse)
    }




    Action {
        id: addNodesMode
        checkable: true
        text: "Add nodes"
    }

    Action {
        id: addWireMode
        checkable: true
        text: "Add wire"
        onCheckedChanged: {
            if(checked) {
                currentWire = wireComp.createObject(null, { color: currentColor })
                drawboard.focus = true
            }
        }

    }

    Component {
        id: nodeHandleComp
        NodeHandle {
            function toJsObject() {
                return {x: x, y: y}
            }
        }
    }

    Component {
        id: wireComp
        Wire {}
    }

    Component {
        id: segComp
        WireSegment {}
    }

    Component {
        id: nodeComp
        Node {}
    }

    ListModel {
        id: nodesModel
        property var objectData: toJsObject()

        function typeDetails(objectString) {
            var rx = new RegExp("([A-Z_][A-Za-z0-9_]+?)(_QMLTYPE_\\d+)?(_QML_\\d+)?\\(0x[A-Fa-f0-9]+\\)")
            return rx.exec(objectString)
        }

        function toTypeName(objectString) {
            return typeDetails(objectString)[1]
        }

        function toJsObject() {
            var ret = []
            for (var i = 0; i < count; ++i) {
                var o = get(i)
                var obj = {}
                for (var k in o) {
//                    console.log("Item %1: property=%2".arg(i).arg(k), typeof o[k])
                    if (!(typeof o[k] in {"function": true, "undefined": true})) {
                        if (typeof o[k] === "object") {
                            var _o = { type: "", "value": {} }
                            if (typeof o[k].toJsObject === 'function') {
                                _o.type = toTypeName(o[k].toString())
                                _o.value[k] = o[k].toJsObject()
                            } else {
                                _o.value[k] = "no toJsObject() method for type %1".arg(o[k].toString())
                            }
                            obj = _o
                        }
                        else
                            obj[k] = o[k]
                    }
                }
                ret.push(obj)
            }
            return ret
        }

    }

    Settings {
        id: settings
        property var nodes
        property int nodeCount
        property var wires
        property alias nodesModel: nodesModel.objectData
        Component.onCompleted: {
            if(typeof nodes.length === 'undefined')
                return
            drawboard.nodes_var = nodes
            for (var i = 0; i < nodes.length; ++i) {
                if (nodes[i]) {
                    addNode(nodes[i].x, nodes[i].y)
                } else {
                    addNode(0, 0)
                    deleteNode(i)
                }
            }
            for (var i = 0; i < wires.length; ++i) {
                var w = wires[i]
                var wire = wireComp.createObject()
                wire.color = w.color
                for (var j = 0; j < w.segments.length; ++j) {
                    var s = w.segments[j]
                    var seg = segComp.createObject()
//                    seg.begin.node = drawboard.nodes[s.begin.node]
                    seg.begin.node = nodesModel.get(s.begin.node).i
                    seg.begin.position = s.begin.position
                    seg.begin.offset= s.begin.offset
                    seg.end.node = nodesModel.get(s.end.node).i
                    seg.end.position = s.end.position
                    seg.end.offset = s.end.offset
                    wire.segments.push(seg)
                }
                model.push(wire)
            }
            drawboard.modelChanged()
            drawboard.requestPaint()
        }
    }


}

