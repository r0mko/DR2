import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Window 2.2

import Dr.Graphics 1.0
import Dr.UI 1.0
import Dr.MetaGraph 1.0
import Dr.Cartograph 1.0

SidebarPanel {
    id: segmentsPanel
    property NodeChildrenModel model

    property var currentNetHandle

    onCurrentNetHandleChanged: {
        if( currentNetHandle.modelIndex === undefined ) {
            segmentsPanel.model = null
        }else{
            segmentsPanel.model = null
            segmentsCurrent.rootIndex = currentNetHandle.modelIndex
            segmentsPanel.model = segmentsCurrent
        }


    }

    ColumnLayout {
        TableView {
            id: segmentTable
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: segmentsPanel.model
            onModelChanged: console.log( 'onModelChanged', model !== null ? model.count : 0 )
            TableViewColumn {
                title: 'mrl'
                role: "mrl"
            }
            TableViewColumn {
                title: 'nodeId'
                role: "nodeId"
            }
            
            TableViewColumn {
                title: 'in'
                delegate: Text{
                    text: join()
                    function join() {
                        var x = ''
                        segmentTable.model.get(styleData.row).inEdges().forEach(function(e) {
                            x += e.getData.from + ' -> ' + e.getData.to + '; '
                        })
                        return x
                    }
                }
            }
            TableViewColumn {
                title: 'out'
                delegate: Text{
                    text: join()
                    function join() {
                        var x = ''
                        segmentTable.model.get(styleData.row).outEdges().forEach(function(e) {
                            x += e.getData.from + ' -> ' + e.getData.to + '; '
                        })
                        return x
                    }
                }
            }
            
            
            TableViewColumn {
                title: 'out'
                delegate: Text{
                    text: join()
                    function join() {
                        var x = ''
                        segmentTable.model.get(styleData.row).outNodes().forEach(function(e) {
                            x += e.mrl + '; '
                        })
                        return x
                    }
                }
            }
            TableViewColumn {
                title: 'in'
                delegate: Text{
                    text: join()
                    function join() {
                        var x = ''
                        segmentTable.model.get(styleData.row).inNodes().forEach(function(e) {
                            x += e.mrl + '; '
                        })
                        return x
                    }
                }
            }
            
        }
        Button {
            text: 'test'
            Layout.minimumHeight: 50
            onClicked: {
                segmentTable.rowCount
                console.log( 'onModelChanged', segmentTable.model !== null ? segmentTable.model.count : 0 )
            }
        }
        NodeChildrenModel {
            id: segmentsCurrent
            onRootIndexChanged: console.log( 'onRootIndexChanged', count )
        }

    }
}
