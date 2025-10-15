import QtQuick 2.3
import Dr.MetaGraph 1.0
import Dr.QRPC 1.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import Qt.labs.settings 1.0
import QtGraphicalEffects 1.0
import QtQuick.Window 2.0
import "testrpc"
import "widgets"

ApplicationWindow {
    id: app
    visible: true
    width: 1280
    height: 800
    title: qsTr("RPC test rig")

    property var agents: []

    ListModel {
        id: agents_model

    }

    RPCEngine {
        id: engine
        property QtObject puppet
        nodeName: rpc_conf.nodeName
//        peers: rpc_conf.peers ? rpc_conf.peers : []
        networkMessenger {
            id: netman
            listening: true
        }
        Component.onCompleted: {

            registerObject(cursor)
            registerObject(main_model)
        }
        onPeerAdded: {
            console.log("Added peer", peer)
        }

        onObjectRegistered: {
            var p = engine.queryRemoteObject(peer, name)
            console.log("Got object %1 %2 from %3".arg(name).arg(className).arg(peer))
            agents.push(p)
            agents_model.append({ agentId: agents.length - 1, puppetName: name, puppetClassName: className })

//            console.log(JSON.stringify(puppet.datanode$data))
//            console.log(puppet.datanode$dataChanged)
////            puppet.datanode$dataChanged.connect(function () { console.log("Data changed",  ) })
//            p_datanodeAgent.target = puppet
//            puppet.clear()

//            queryObject(peer, name, className)
        }

    }

//    RemotePeer {
//        host: "46.151.157.8"
//        port: 42001
//    }

    RemotePeer {
        host: "192.168.1.16"
        port: 42001
    }

    Connections {
        id: p_datanodeAgent
        target: null
        onDatanode$dataChanged: console.log("data changed", JSON.stringify(target.datanode$data))
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
                    model: engine.peers
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

            Repeater {
                id: i_blobs

                anchors.fill: parent
                model: agents_model
                delegate: Rectangle {
                    id: puppet_del
                    width: 200
                    height: 70
                    radius: 10
                    color: app.agents[agentId].blob$metadata.color
//                    color: Qt.hsla(Math.random(), Math.random(), 0.8)

                    onXChanged: sendData(app.agents[agentId], { color: Qt.hsla(x / i_blobs.width, y / i_blobs.height, 0.7).toString() } )
                    onYChanged: sendData(app.agents[agentId], { color: Qt.hsla(x / i_blobs.width, y / i_blobs.height, 0.7).toString() } )

                    ColumnLayout {
                        spacing: 0
                        anchors.fill: parent
                        Label {
                            text: puppetName
                        }
                        Label {
                            text: puppetClassName
                        }
                        Label {
                            text: agentId
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        drag.target: puppet_del
                        drag.threshold: 0
                    }
                    function sendData(puppet, data) {
                        if (delay.running)
                            return
                        puppet.blob$metadata = data
                        delay.start()
                    }
                }


            }

            Rectangle {
                id: cursor
                objectName: "cursor"
                width: 40
                height: 40
                x: 200
                y: 200

                radius: 10
                color: "#f0D070"
//                fucntion
                function sendData() {
                    if (delay.running)
                        return
                    var d = { xp: x, yp: y }
                    engine.puppet.datanode$data = d
                    delay.start()
                }
                Timer {
                    id: delay
                    interval: 16
                    repeat: false
                }

                onXChanged: sendData()
                onYChanged: sendData()
//                onXChanged: console.log("x changed", x)

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

    GraphModel {
        id: main_model
        property url file
        property string fileName: file.toString().length ? file.toString().match(/(\w+)\..+$/)[1] : "untitled"
        property bool changed
        onDataChanged: changed = true
        DataNodeFactory { id: fDataNode }
        NodeFactory { id: fNode }
        MetaNodeFactory { id: fMetaNode }
        CollectionFactory { id: fCollection }
        TypeBuilder { id: builder }
        BlobFactory { id: fBlob }
        PointNodeFactory{ id: fPoint }
        RootNodeFactory { }
        function init() {

            loadModel('file:///Users/Tox/Desktop/moscow10.mgf')

        }
        Component.onCompleted: init()
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
        property alias listen: netman.listening
        property alias port: netman.serverPort
    }

    function queryObject(peer, name, classname) {
        console.log("object added for", peer, name, classname)
        var puppet = engine.queryRemoteObject(peer, name)

        var cur = curComp.createObject(objField, { color: puppet.color, x: puppet.x, y: puppet.y });
        cur.x = Qt.binding(function() { return puppet.x })
        cur.y = Qt.binding(function() { return puppet.y })
//        cursor.x = Qt.binding(function() { return puppet.x; })
//        cursor.y = Qt.binding(function() { return puppet.y; })
        console.log("got puppet", puppet)
    }



    TestObject {
        id: myobj
        objectName: "dummy"
        intProperty: 42
        textProperty: "Lorem ipsum"
        varProperty: { foo: "bar"; bsz: 532 }
        onTextPropertyChanged: console.log('text property changed', textProperty)
    }

//    Component {
//        id: logComp
//        Window {
//            property alias logger: logView.logger
//            width: 800
//            height: 600
//            id: logwin
//            LogViewer {
//                id: logView
//                anchors.fill: parent
//                timestampFont: {
//                    family: "Consolas"
//                }
//            }
//        }
//    }

    Component {
        id: curComp
        Rectangle {
            width: 20
            height: 20
            color: "#FF8000"
            radius: 8
        }
    }
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
