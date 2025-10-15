import DataReactor.RPC 1.0
import DataReactor.Log 1.0
import QtQuick 2.3
import Qt.labs.settings 1.0

Object {
    id: core
    Logger {
        id: logger
        objectName: "log"
        Component.onCompleted:  {
            engine.registerObject(logger, true)
            logger.insert(new Date, engine.nodeName, objectName, "Registered log service in RPC Engine", Logger.Info)
        }
    }

    function log(sender, message, type, attachment) {
        if (logger) {
            if (!attachment)
                attachment = {}
            if (!type)
                type = Logger.Info
            logger.insert(new Date, engine.nodeName, sender, message, type, attachment)
        }
    }


    RPC.engine {
        id: engine
        objectName: "core"
        onPeerAdded: log(engine.objectName, "Peer " + peer.nodeName + " added", Logger.Info, {name: peer.nodeName, host: peer.netPeer.host, port: peer.netPeer.port})
        onPeerRemoved: log(engine.objectName, "Peer " + peer.nodeName + " removed", Logger.Info)
        onObjectRegistered: log(engine.objectName, "Peer " + peer.nodeName + " registered object " + name + " className" + className)
    }

    Settings {
        id: logconf
        property alias path: logger.archivePath
    }

    Settings {
        id: conf
        category: "rpcengine"
        property alias listen: netman.listening
        property alias nodeName: engine.nodeName
    }

    RPC.engine.networkMessenger {
        id: netman
        listening: true
    }


}
