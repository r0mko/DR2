import QtQuick 2.3
import DataReactor.UI 1.0
import DataReactor 1.0

Object {
    property int group: 0

    property alias edges: m_edges
    property alias fillets: m_fillets
    readonly property int edgeCount: m_edges.count

    signal modelChanged

    ListModel {
        id: m_edges
        property var listData: []
        onRowsRemoved: {
            console.log("Removed edges", first, last, listData.splice(first, last))
            modelChanged()
        }
        onRowsInserted: modelChanged()
    }

    ListModel {
        id: m_fillets
        property var listData: []
        onRowsRemoved: {
            console.log("Removed fillets", first, last, listData.splice(first, last))
        }
    }

    function addEdge(n1, n2) {
        var e = edgeComp.createObject(null, { n1: n1, n2: n2 })
        m_edges.append({ i: e, _idx: m_edges.listData.length })
        m_edges.listData.push(e)
//        console.log("inserted edge, count", m_edges.count)
    }

    function addFillet(edge1, edge2) {
        var e = filletComp.createObject(null, { n1: n1, n2: n2 })
        m_fillets.append({ i: e, _idx: m_fillets.listData.length })
        m_fillets.listData.push(e)
    }

    Component {
        id: edgeComp
        Edge {
            Connections {
                target: n1
                onPChanged: modelChanged()
            }
            Connections {
                target: n2
                onPChanged: modelChanged()
            }
        }
    }

    Component {
        id: filletComp
        Fillet {}
    }
}
