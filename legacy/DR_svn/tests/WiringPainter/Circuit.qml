import QtQuick 2.3

QtObject {
    id: root
    property color color

    property ListModel segments: ListModel {}      // list<WireSegment>
    property ListModel fillets: ListModel {id: filletsModel}       // list<WireFillet>

    property list<QtObject> __data
    default property alias data: root.__data




    Component.onCompleted: {
        var graph = {}
        for (var i = 0; i < __data.length; ++i) {
            var t = getTypeName(__data[i])
            if (t === "WireSegment") {
                var seg = __data[i]
                drawboard.handlesMoved.connect(seg.resetRealPos)
                var n1 = seg.begin.node.toString()
                var n2 = seg.end.node.toString()

                if (!graph[n1]) {
                    graph[n1] = { ins: [], outs: [] }
                }
                if (!graph[n2]) {
                    graph[n2] = { ins: [], outs: [] }
                }

//                console.log("Segment %1 [%2-%3]".arg(seg).arg(n1).arg(n2))
                segments.append({ i: seg })
                graph[n1].outs.push(seg)
                graph[n2].ins.push(seg)

            }
        }

        for (var n in graph) {
            var c = graph[n]
            if (c.ins.length > 0) {
                if (c.ins.length > 1 && c.outs.length === 0) {
                    console.log("Multiple dead-ends not allowed!")
                    continue
                }
                if (c.outs.length > 0) {
                    for (i = 0; i < c.ins.length; ++i) {
                        for (var j = 0; j < c.outs.length; ++j) {
                            fillets.append({ i: filletComp.createObject(filletsModel, { from: c.ins[i], to: c.outs[j] }) })
                        }
                    }
                }
            }
        }

    }

    property Component filletComp: Component {
        WireFillet { }
    }
}

