import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Window 2.2

import Dr.Graphics 1.0
import Dr.UI 1.0
import Dr.MetaGraph 1.0
import Dr.Cartograph 1.0
import "ValueEditors"
ModelWeaver {
    id: i_sections
    property bool vis: true
    WiringSection {
        id: net_delegate
        scene: m_scene
        visible: vis
        property color posColor: handle.dataHandle.posColor.toString()
        property color negColor: handle.dataHandle.negColor.toString()
        onPosColorChanged: {
            posPen.color = posColor
            update()
        }
        onNegColorChanged: {
            negPen.color = negColor
            update()
        }
        
        StateGroup {
            states: [
                State {
                    name: 'split'
                    when: a_split.checked
                    PropertyChanges {
                        target: net_delegate
                        //                            hoverEnabled: true
                        onMousePress: mouse.accepted = true
                        onMouseClicked: {
                            split(mouse.pos)
                            m_scene.clearSelection()
                            a_split.checked = false
                        }
                    }
                },
                State {
                    name: "add_connection"
                    when: a_addConnection.checked
                    PropertyChanges {
                        target: net_delegate
                        hoverEnabled: true
                        onHoverEnter: {
                            var r = findAtPoint(hover.pos)
                            hover_handle.runner = r.posResultRunner
                            hover_handle.visible = true
                        }
                        onHoverMove: {
                            var r = findAtPoint(hover.pos)
                            hover_handle.runner = r.posResultRunner
                            hover_handle.visible = true
                        }
                        onHoverLeave:  hover_handle.visible = false
                        onMouseClicked: {
                            var r = findAtPoint(mouse.pos)
                            console.log("Add label and power input at seg", r.segment.uid, "at pos", r.posResultRunner.position, "angle", r.posResultRunner.angle, "leftSide", r.posResultRunner.angle > 0 )
                            addPort(r.segment.uid, r.posResultRunner.position, r.posResultRunner.angle > 0 )
                        }
                    }
                }
            ]
        }
        
        //            segmentsModel.rootIndex: handle.modelIndex
        segmentsModel {
            rootIndex: handle.modelIndex
            tag: "Catenary"
            model: gmodel
        }
        nodesModel.rootIndex: handle.modelIndex
        
        scalingFactor: m_scene.visualScale
        radius: m_sidebar.curveSmoothing
        precision: m_sidebar.curvePrecision
        
        property bool debugNodesMode: m_sidebar.debugMode
        onDebugNodesModeChanged: setDebugMode(debugNodesMode)
        
        Component.onCompleted: {
            console.log("Double wire", doubleWire, nodeData.type)
            if (nodeData.type == "TrolleySection") {
                posPen.color = nodeData.posColor
                posPen.width = 3
                posPen.cosmetic = true
                negPen.color = nodeData.negColor
                negPen.width = 3
                negPen.cosmetic = true
                spacing = 6
            } else if (nodeData.type == "TramSection") {
                posPen.color = nodeData.posColor
                //                    posPen.color = "#ff0000"
                posPen.width = 4
                posPen.cosmetic = true
                negPen.strokeStyle = Qt.NoPen
            }
        }
        
        function addPort(uid, position, leftSide) {
            f_DataNode.parentHandle = handle
            f_DataNode.data = {
                position: position,
                uid: uid,
                leftSide: leftSide
            }
            var h = f_DataNode.create("cable_in")
        }
    }
    
    function instance(handle) { return objectAt(handle.rowIndex) }
    
}
