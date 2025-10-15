import QtQuick 2.5
import QtQuick.Window 2.2
import Dr.MetaGraph 1.0
import Dr.QRPC 1.0
import QtQuick.Controls 1.4

Window {
    width: 1000
    height: 620
    visible: true

    RPCEngine {
        id: engine
        nodeName: '<SERVER>'
        networkMessenger {
            id: netman
//            serverPort: 42033
            listening: true
        }
        onPeerAdded: {
            console.log("Peer added", peer)
        }
        onObjectRegistered: {
            console.log( 'Object registered', peer, name, className )
        }
    }

    TableView {
        model: main_model
        TableViewColumn {
            role: "object"
        }
    }

    GraphModel {
        id: main_model
        property variant agent: ""
        DataNodeFactory { id: fDataNode }
        NodeFactory { id: fNode }
        MetaNodeFactory { id: fMetaNode }
        CollectionFactory { id: fCollection }
        TypeBuilder { id: builder }
        BlobFactory { id: fBlob }
        PointNodeFactory{ id: fPoint }
        RootNodeFactory { }

        ModelWatcher {
            id: spview
            mrl: "spview/layers"
        }

        function init() {
            loadModel('file:///home/dnemov/QT/DB/moscow9.mgf')
//            loadModel('file:///Users/romko/Desktop/moscow10.mgf')
//            loadModel('file:///Users/Tox/Desktop/moscow10.mgf')
            agent = getHandle('spview/datasources/db1').agent
            engine.registerObject( agent )
        }
        Component.onCompleted: init()
    }
}

