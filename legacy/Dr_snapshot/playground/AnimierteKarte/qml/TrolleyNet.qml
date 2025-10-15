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

GuideView {
    id: net_delegate
    scene: m_scene
    flags: VisualInstance.ItemIsSelectable
    hoverEnabled: true
    radius: 15

    property alias waysModel: ways
    property alias nodesModel: net_nodes

    onSelectedChanged: {
        if (selected) {
            nets_panel.addToSelection(index)
        } else {
            nets_panel.deselect(index)
        }
    }

    onHoverEnter: {
        hover_handle.visible = hover.modifiers & Qt.ControlModifier
        if (hover_handle.visible) {
            var r = findNearestPoint(hover.pos)
            hover_handle.runner = r
        }
    }
    onHoverMove: {
        hover_handle.visible = hover.modifiers & Qt.ControlModifier
        if (hover_handle.visible) {
            var r = findNearestPoint(hover.pos)
            hover_handle.runner = r
        }


        var r = findNearestPoint(hover.pos)
        isolator.x = r.point.x
        isolator.y = r.point.y
        isolator.rotation = r.angle
    }
    //            onHoverLeave: {
    //                hover_handle.visible = hover.modifiers & Qt.ControlModifier
    //                if (hover_handle.visible) {
    //                    var r = findNearestPoint(hover.pos)
    //                    hover_handle.runner = r
    //                }
    //            }

    onMousePress: {
        if (mouse.modifiers & Qt.ControlModifier) {
            console.log("ctrl+click")
            nets_panel.setCurrent(index)
            mouse.accepted = false
            editor.state = "append"
        }
        if (mouse.modifiers & Qt.AltModifier) {
            nets_panel.setCurrent(index)
            mouse.accepted = true
            //                    editor.state = "append"
            console.log("splitting way", index)
            split(findNearestPoint(mouse.pos), index, mouse.pos)
        }
    }
    function split(runner, index, mpos){
        console.log( findAtPoint(mpos).segment.uid )
        var eh = ways.getHandle(index)
        var headNode = eh.startNode
        var tailNode = eh.endNode
        var ehdata = eh.getData()
        var tags = eh.tags
        var parts = ehdata.guide.splitted(runner.position)
        var leftData = {
            guide: parts[0],
            color: ehdata.color,
            offset: ehdata.offset
        }
        var rightData = {
            guide: parts[1],
            //                    color: ehdata.color,
            color: Qt.hsla(Math.random(), 0.8, 0.5),
            offset: ehdata.offset
        }
        eh.destroy()

        f_DataNode.parentHandle = route.currentSectionHandle
        f_DataNode.data = {
            type:'JointVertex',
            x: parts[0].tailPoint.x,
            y: parts[0].tailPoint.y
        }
        var node = f_DataNode.create();
        gmodel.addEdge(headNode, node, tags, leftData)
        gmodel.addEdge(node, tailNode, tags, rightData)

    }

    NodeChildrenModel {
        id: net_nodes
        onDataChanged: {
            if (topLeft.row < 0)
                return
            var h = getHandle(topLeft.row)
            if (!h)
                return;
            var e_in = h.inEdges()
            var e_out= h.outEdges()
            if (h.data.type == 'JunctionVertex') {
                e_in.forEach(function(ie) {
                    e_out.forEach(function(oe) {
                        //                                console.log("Adding interlink %1 -> %2".arg(JSON.stringify(ie.endNode.data)).arg(JSON.stringify(oe.startNode.data)))
                        var e1 = ways.getIndexByUid(ie.uid)
                        var e2 = ways.getIndexByUid(oe.uid)
                        console.log(e1, e2, net_delegate.segment(e1), net_delegate.segment(e2))
                        net_delegate.addLink(e1, e2, GuideNode.JunctionType)
                    })
                })
                //                        console.log(h.data.type)
                //                        console.log(h.mrl, "out", e_out, "in", e_in)

            }


            //                    console.log("Changed data", h.data, h.mrl, h.outEdges(), h.inEdges())
        }
    }

    EdgeModel {
        id: ways


        onModelReset: {
            for (var i = 0; i < count; ++i) {
                var h = getHandle(i)
                buildSegment(h, i)
            }
        }

        onItemAdded: {
            var h = getHandleByUid(uid)
            buildSegment(h, index)
        }

        onEdgeRemoved: {
            net_delegate.removeSegment(index)
            console.log("Removed edge", index, uid)
        }


        function buildSegment(h, index) {
            var ed = h.getData()
            console.log("building twin segment %1 from edge uid %2".arg(index).arg(h.uid), h.startNode, h.endNode)
            var seg1 = addSegment(index, ed.guide, ed.color1, 2)
            seg1.offset = Math.round(Math.random() * 3)
            var seg2 = addSegment(index + 50, ed.guide, ed.color2, 2)
            seg2.offset = seg1.offset + 3
        }
    }
}
