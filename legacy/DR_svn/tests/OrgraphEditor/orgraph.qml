import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import DataReactor.UI 1.0
//import QtQuick.XmlListModel 2.0

import "GraphView" as GraphView
import "ListView" as List
import "Editor" as Editor

ApplicationWindow {
    visible: true
    width: 1200
    height: 800
    title: qsTr("Orgraph editor")

    ExclusiveGroup {
        id: toolBarGroup
        Action {
            id: a_nodes
            checkable: true
        }
    }

    ColumnLayout {
        id: root
        spacing: 0
        anchors.fill: parent
        ThinToolbar {
            id: toolbar
            Layout.fillWidth: true
            height: 30
            implicitHeight: 30
        }

        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Editor.Drawboard {
                id: drawboard
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "#333"

                mainToolBar: toolbar
                tools: [
                    Editor.NodeEditorTool {
                        datamodel: datamodel
//                        action: a_nodes
                    },
                    Editor.EdgeEditorTool {
                        datamodel: datamodel
                    }
                ]

                Repeater {
                    visible: false
                    anchors.fill: parent
                    model: datamodel.graphs
                    delegate: GraphView.GraphDelegate {}
                }

                Editor.NodesView {
                    id: nodesView
                }
            }

            Rectangle {
                id: sidebar
                color: "#444"
                Layout.fillHeight: true
                Layout.minimumWidth: 160
                ColumnLayout {
                    anchors.fill: parent

                    Rectangle {
                        anchors.fill: nodesList
                        color: "#eee"
                    }

                    ListView {
                        id: nodesList
                        focus: currentIndex >= 0
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        highlightMoveVelocity: 2500
                        model: datamodel.nodes
                        currentIndex: -1

                        delegate: List.NodeDelegate {}

                        highlight: Rectangle {
                            color: "#4030a0ff"
                            radius: 4
                            border.width: 1
                            border.color: "#000"
                        }
                    }
                }
            }
        }
    }

    //    Component { id: edgeComp; }

    Graph { id: graph }

    QtObject {
        id: datamodel
        property ListModel nodes: ListModel {
            //            id: m_nodes
            property var data: []
            property Component nodeComp: Node {}
            Component.onCompleted: { data.forEach(function(v) { append({ i: v }) }) }
            function addNode(mouseX, mouseY) {
                var n = nodeComp.createObject(null, { x: mouseX, y: mouseY })
                append({ i: n, idx: data.length })
                data.push(n)
            }
        }

        property ListModel graphs: ListModel {
            //            id: m_graphs
            property var data: [ graph ]
            Component.onCompleted: {
                data.forEach(function(v) { append({ i: v }) })
            }
        }
    }


    //    XmlListModel {
    //        id: osmNodes
    //        source: "qrc:/nodes.xml"
    //        query: "/osm/node"
    //        XmlRole { name: "lat"; query: "@lat/number()" }
    //        XmlRole { name: "lon"; query: "@lon/number()" }
    //        XmlRole { name: "id"; query: "@id/number()" }
    //        onProgressChanged: console.log("nodes progress", progress)
    //        onStatusChanged: console.log("nodes status", status)
    //        onCountChanged: console.log("nodes count", count)
    //    }
    //    XmlListModel {
    //        id: osmWays
    //        source: "qrc:/ways.xml"
    //        query: "/osm/way"
    //        XmlRole { name: "id"; query: "@id/number()" }
    //        XmlRole { name: "refs"; query: "nd/@ref/string(.)" }
    //        onProgressChanged: console.log("ways progress", progress)
    //        onStatusChanged: console.log("ways status", status)
    //        onCountChanged: console.log("ways count", count)
    //    }

}
