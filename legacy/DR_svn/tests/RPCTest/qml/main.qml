import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import DataReactor.RPC 1.0
import Qt.labs.settings 1.0
import QtGraphicalEffects 1.0
import DataReactor.Log 1.0
import QtQuick.Window 2.0
import "testrpc"
import "widgets"
ApplicationWindow {
    id: app
    visible: true
    width: 1280
    height: 800
    title: qsTr("RPC test rig")

    RPC.engine {
        id: engine
    }

    RowLayout {
        id: mainLayout
        anchors.fill: parent
        spacing: 8

        ColumnLayout {
            spacing: 3
            Layout.fillWidth: true
            Item {
                id: viewCont
                width: 300
                //                height: 450
                Layout.fillHeight: true
                clip: true
                PeerList {
                    anchors.fill: parent
                    anchors.margins: 8
//                    model: engine.peers
                    listItemDelegate: PeerListItem {}
                    addDialogComponent: AddPeerForm {}
                }
            }
        }

        Rectangle {

            id: objField
            color: "#444"
            Layout.minimumWidth: 350
            Layout.fillWidth: true
            anchors.top: parent.top
            anchors.bottom: parent.bottom


            Rectangle {
                id: cursor
                width: 40
                height: 40
                x: 200
                y: 200

                radius: 10
                color: "#00D070"

                onXChanged: console.log("x changed", x)

                MouseArea {
                    id: mousearea
                    anchors.fill: parent
                    drag {
                        threshold: 0
                        target: cursor
                    }
                }
            }



        }
    }



//    RPC {
//        id: engine
//        nodeName: rpc_conf.nodeName
//        onPeersChanged: {
//            var p = []
//            for (var i = 0; i < engine.peers.length; ++i) {
//                p.push({"url": engine.peers[i].netPeer.url.toString(), "active": engine.peers[i].netPeer.active, "node": engine.peers[i].nodeName })
//            }
//            console.log("rpc peers", JSON.stringify(p, undefined, 2))
//        }

//        onObjectRegistered: {
//            queryObject(peer, name, className)
//        }

//        onPeerAdded: {
//            console.log("peer added", peer)
//        }



//    }

//    RPC.engine.
//    QtObject {
//        id: buffer
//        property var rpcPeers: []
//    }

//    RPCEngine.peers: rpc_conf.peers

    Settings {
        id: rpc_conf
        category: "rpc"
        property string nodeName
        property var peers
        onPeersChanged: console.log(peers)
        //        property alias readablePeers: buffer.rpcPeers
    }

    Settings {
        id: netman_conf
        category: "netmanager"
//        property alias listen: netman.listening
//        property alias port: netman.serverPort
    }

    function queryObject(peer, name, classname) {
        console.log("object added for", peer, name, classname)
        if(name === "log" && classname === "Logger") {
            var puppet = engine.queryRemoteObject(peer, name)
            console.log("got logger", puppet)
            //            log(engine.nodeName, "Joined to logger")
            //            logger.insert(new Date, engine.nodeName, engine.objectName, "Joined", Logger.Info)
        }

        //        var cur = curComp.createObject(objField, { color: puppet.color, x: puppet.x, y: puppet.y });
        //        cur.x = Qt.binding(function() { return puppet.x })
        //        cur.y = Qt.binding(function() { return puppet.y })
        //        cursor.x = Qt.binding(function() { return puppet.x; })
        //        cursor.y = Qt.binding(function() { return puppet.y; })
        //        console.log("got puppet", puppet)
    }



    TestObject {
        id: myobj
        objectName: "dummy"
        intProperty: 42
        textProperty: "Lorem ipsum"
        varProperty: { foo: "bar"; bsz: 532 }
        onTextPropertyChanged: console.log('text property changed', textProperty)
    }

    Component {
        id: logComp
        Window {
            property alias logger: logView.logger
            width: 800
            height: 600
            id: logwin
            LogViewer {
                id: logView
                anchors.fill: parent
                timestampFont: {
                    family: "Consolas"
                }
            }
        }
    }

    //    Component {
    //        id: curComp
    //        Rectangle {
    //            width: 20
    //            height: 20
    //            color: "#FF8000"
    //            radius: 8
    //        }
    //    }
    //    Component {
    //        id: trailComp
    //        Rectangle {
    //            id: rect91
    //            property Item targetItem
    //            width: 20
    //            height: 20
    //            color: "#00aad0"
    //            radius: 8
    //            x: targetItem.x
    //            y: targetItem.y
    //            //            Behavior on x { SpringAnimation { epsilon: 0.1; spring: 4.5; damping: 0.2; velocity: 1500 } }
    //            //            Behavior on y { SpringAnimation { epsilon: 0.1; spring: 4.5; damping: 0.2; velocity: 1500 } }
    //            Behavior on x { NumberAnimation { easing.type: Easing.OutInExpo; duration: 100 } }
    //            Behavior on y { NumberAnimation { easing.type: Easing.OutInExpo; duration: 100 } }

    //        }
    //    }


    //    Component {
    //        id: puppetDelegate
    //        PuppetDelegate {

    //        }
    //    }




}
