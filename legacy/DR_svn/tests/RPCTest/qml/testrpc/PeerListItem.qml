import QtQuick 2.0
import QtQuick.Layouts 1.1
import DataReactor.RPC 1.0
import "../widgets"

Rectangle {
    id: root
    width: parent.width
    height: 40
    Component.onCompleted: console.log(netPeer, netPeer.status)
    color: ListView.isCurrentItem ? "#2000AFFF" : "transparent"
    RowLayout {
        Item {
            id: status
            height: parent.height
            width: 20
            Rectangle {
                id: indicator
                height: 10
                width: 10
                radius: 4
                anchors.centerIn: parent
                color: {
                    switch (netPeer.status) {
                    case Peer.Initializing: return "#000000"
                    case Peer.SocketReady: return "#404040"
                    case Peer.Disconnected: return "#800000"
                    case Peer.HostLookup: return "#800040"
                    case Peer.Connecting: return "#804000"
                    case Peer.Negotiation: return "#80FF00"
                    case Peer.WaitingForHello: return "#80FF00"
                    case Peer.Connected: return "#00FF00"
                    case Peer.Closing: return "#FF00FF"
                    case Peer.Error: return "#FF0000"
                    case Peer.Reconnecting: return "#808050"
                    default: return "#FFFFFF"
                    }
                }

            }
        }

        id: rowLayout1
        anchors.leftMargin: 8
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.topMargin: 0
        anchors.bottomMargin: 0
        ColumnLayout {
            id: columnLayout1
            Layout.fillWidth: true
            height: parent.height
            Text {
                id: nodeLabel
                text: nodeName
                font.bold: false
                font {
                    family: "HelveticaNeueCyr"
                    weight: Font.Light
                    pointSize: 12
                }

            }
            RowLayout {
                Text {
                    id: addrLabel
                    text: netPeer.host + ":" + netPeer.port
                    font {
                        family: "HelveticaNeueCyr"
                        weight: Font.Light
                        pointSize: 8
                    }

                }
            }
        }
        Item {
            id: cont1
            Layout.alignment: Qt.AlignRight
            height: parent.height
            Layout.fillWidth: true

            Rectangle {
                id: barbg
                anchors.fill: bar

            }
            Row {
                id: bar
                anchors.right: parent.right
                anchors.rightMargin: 8
                anchors.verticalCenter: parent.verticalCenter
                height: 20
                spacing: -1
                ThinToolButton {
                    font.family: "Entypo"
                    font.pixelSize: 32
                    text: "🔄"
                }
                ThinToolButton {
                    font.family: "Entypo"
                    font.pixelSize: 32
                    text: "⚙"
                }

                ThinToolButton {
                    font.family: "Entypo"
                    font.pixelSize: 32
                    text: ""
//                    enabled: false
                }
                ThinToolButton {
                    font.family: "Entypo"
                    font.pixelSize: 32
                    text: "❎"
//                    enabled: false
                }
            }




        }

    }
}
