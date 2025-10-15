import QtQuick 2.0
import DataReactor.RPC 1.0
import DataReactor.Log 1.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2


Item {
    id: root
    property QtObject logger
    property int session
    property alias timestampFont: h_timestamp.font
    property alias nodeNameFont: h_nodeName.font

    onLoggerChanged: {
                console.log("logger initialized:", logger, "building model")
        var logSize = logger.recordCount
        //        console.log("log total size", logSize)
        if (logSize > 2000)
            logSize = 2000
        session = logger.beginSession(logSize)
                console.log("started logger session", session)
        var dat = []
        dat = logger.fetch(session, 0, logSize)
        dat.forEach(function(element){
            logmodel.append(element)
        })
        logView.positionViewAtEnd()
        logger.recordAdded.connect(updateModel)
                console.log("model complete", dat.length)

    }

    function updateModel(record) {
        logmodel.append(record)
        logView.positionViewAtEnd()
    }

    Rectangle {
        id: header
        height: 22
        width: root.width
        gradient: Gradient {
            GradientStop {
                position: 0.00;
                color: "#ffffff"
            }
            GradientStop {
                position: 0.44;
                color: "#f4f4f4";
            }
            GradientStop {
                position: 0.56;
                color: "#eaeaea";
            }
            GradientStop {
                position: 1.00;
                color: "#efefef";
            }
        }

        SplitView {
            orientation: Qt.Horizontal
            anchors.fill: header

            ThinToolButton {
                id: h_timestamp
                height: header.height
                text: "Timestamp"
                Layout.preferredWidth: 100
            }
            ThinToolButton {
                id: h_nodeName
                height: header.height
                text: "Node name"
                Layout.preferredWidth: 150
            }
            ThinToolButton {
                id: h_objName
                height: header.height
                text: "Object name"
                Layout.preferredWidth: 150
            }
            ThinToolButton {
                id: h_message
                text: "Message"
                height: header.height
                Layout.fillWidth: true
            }
        }
    }

    ScrollView {
        anchors.bottom: root.bottom
        anchors.left: root.left
        anchors.right: root.right
        anchors.top: header.bottom
        ListView {
            id: logView
            anchors.fill: parent
            model: logmodel
            delegate: entryDelegate
        }
    }

    Component {
        id: entryDelegate
        Rectangle {
            id: bg
            width: parent.width
            height: 20
            color: "#fff"
            Row {
                anchors.fill: parent
                Text {
                    id: timestamp
                    font: h_timestamp.font
                    height: parent.height
                    verticalAlignment: Text.AlignVCenter
                    text: Qt.formatDateTime(eventDateTime, "d.MM.yy h:mm:ss,zzz")
                    width: h_timestamp.width
                }
                Text {
                    font: h_nodeName.font
                    height: parent.height
                    verticalAlignment: timestamp.verticalAlignment
                    text: nodeName
                    width: h_nodeName.width
                }
                Text {
                    font: h_objName.font
                    height: parent.height
                    verticalAlignment: timestamp.verticalAlignment
                    text: originName
                    width: h_objName.width
                }
                Text {
                    font: h_message.font
                    height: parent.height
                    verticalAlignment: timestamp.verticalAlignment
                    text: message
                    width: h_message.width
                }
            }


        }
    }

    ListModel {
        id: logmodel
    }




}



