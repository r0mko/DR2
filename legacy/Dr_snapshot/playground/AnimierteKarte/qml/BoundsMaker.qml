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

ModelWeaver {
    id: i_bounds
    active: i_sections.ready
    model: model_boundaries
    property bool vis: true
    BoundaryObject {
        visible: vis
        id: boundary
        scene: m_scene
        property string note
        property var dragRunner
        property var lt: Math.random() > 0.5 ? BoundaryObject.Out2In : BoundaryObject.In2Out
        //            property var handle: boundaryModel.getHandle(index)
        flags: VisualInstance.ItemIsSelectable | VisualInstance.NoSelectionOutline
        prevSection: i_sections.instance(startNode)
        nextSection: i_sections.instance(endNode)
        uidFrom: edgeData.from
        uidTo: edgeData.to
//        onUidToChanged: console.log("Whoa! uidTo changed", uidTo)
//        onUidFromChanged: console.log("Whoa! uidTo changed", uidFrom)

        //            iHeight: 1
        //            iGap: 0.5
        //            scaleFactor: 6.0 / drawarea.scale
        
        iHeight: 1.0
        iGap: 0.5
        scaleFactor: 9.0 * m_scene.visualScale
        posLink: ( Math.random() > 0.5 ) ? BoundaryObject.ClosedBreaker : BoundaryObject.OpenBreaker
        negLink: ( Math.random() > 0.5 ) ? BoundaryObject.ClosedBreaker : BoundaryObject.OpenBreaker
        posFeedPass: Math.random() > 0.5 ? lt : BoundaryObject.NoPass
        negFeedPass: Math.random() > 0.5 ? lt : BoundaryObject.NoPass
        posOperation: Math.random() > 0.5 ? BoundaryObject.OperationCompleted : BoundaryObject.OperationInProgress
        negOperation: Math.random() > 0.5 ? BoundaryObject.OperationCompleted : BoundaryObject.OperationInProgress
        
        onMouseClicked: {
            diupWindow.boundary = boundary
            diupWindow.prevSection = startNode
            diupWindow.nextSection = endNode
            diupWindow.visible = true
        }

        function moveBoundary(pos, force) {
            var r_prev = prevSection.findAtPoint(pos)
            var r_next = nextSection.findAtPoint(pos)
            if (!hover_handle.visible) {
                hover_handle.runner = r_prev.sourceRunner
                return
            }

            if (r_prev.sourceRunner.atEnd) {
                // cut from next and append to prev
                hover_handle.runner = r_next.sourceRunner
                var nextPart = r_next.segment.source.part(0, r_next.sourceRunner.position)
//                if (!force && nextPart.length < 8 / m_scene.viewportScale)
//                    return
//                console.log("next part l =", nextPart.length, "segment %1".arg(r_next.segment.uid))
                r_prev.segment.source.appendGuide(nextPart)
                r_next.segment.source.trimStart(nextPart.length)
                prevSection.updateGeometry()
                nextSection.updateGeometry()
            } else if (r_next.sourceRunner.atStart) {
                // cut tail from prev and prepend to next
                hover_handle.runner = r_prev.sourceRunner
                var prevPart = r_prev.segment.source.part(r_prev.sourceRunner.position, r_prev.segment.source.length)
//                if (!force && prevPart.length < 8 / m_scene.viewportScale)
//                    return
//                console.log("prev part l =", prevPart.length, "segment %1".arg(r_prev.segment.uid))
                r_next.segment.source.prependGuide(prevPart)
                r_prev.segment.source.trimEnd(prevPart.length)
                prevSection.updateGeometry()
                nextSection.updateGeometry()
            }

        }
        StateGroup {
            id: bound_states

            states: [
                State {
                    name: "editPosition"
                    when: a_moveBoundary.checked
                    PropertyChanges {
                        target: boundary
                        flags: VisualInstance.ItemIsSelectable
                        onMousePress: {
                            mouse.accepted = true
                            moveBoundary(mouse.pos)
                            hover_handle.visible = true
                        }

                        onMouseClicked: { /* do nothing */ }

                        onMouseMove: moveBoundary(mouse.pos)
                        onMouseRelease: {
                            moveBoundary(mouse.pos, true)
                            hover_handle.visible = false
                        }
                        
                    }
                }
            ]
            
        }
        DiupWindow {
            id: diupWindow
        }

    }
    
    onObjectAdded:  {
        object.prevSection.addOutBoundary( object.uidFrom, object.nextSection, object.uidTo )
        object.nextSection.addInBoundary( object.uidTo, object.prevSection, object.uidFrom )
    }
}
