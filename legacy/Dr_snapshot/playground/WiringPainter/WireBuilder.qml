import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import Dr.Graphics 1.0
import Dr.UI 1.0
import QtGraphicalEffects 1.0
import Dr.Cartograph 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.2
import Dr.MetaGraph 1.0
import QtQuick.Window 2.2

Instantiator {
    id: m_wireBuilder
    //        model: model_wires
    model: null
    active: false
    
    property RibSlider slider: RibSlider {
        id: m_commonSlider
        handle.parent: drawarea.scene
        onPositionChanged: console.log("position", position, runner.position)
    }
    
    property GuideSplitter splitter: GuideSplitter {}
    
    function insertTerminator(index, position) {
        var o = objectAt(index)
        splitter.setSplitter(o.guide, position)
        var t = comp_terminator.createObject(drawarea.scene)
        t.handle.parent = drawarea.scene
        model_wires.addWire(splitter.left.polygonData, o.offset , Math.random(), 1, 0.5, { begin: o.beginTool, end: t } )
        model_wires.addWire(splitter.right.polygonData, o.offset, Math.random(), 1, 0.5, { begin: t, end: o.endTool } )
        model_wires.remove(index)
        m_commonSlider.handle.visible = false
    }
    
    function insertTransition(index, position) {
        var o = objectAt(index)
        splitter.setSplitter(o.guide, position)
        var spl = comp_transition.createObject(drawarea.scene)
        spl.handle.parent = drawarea.scene
        
        model_wires.addWire(splitter.left.polygonData, o.offset , Math.random(), 1, 0.5, { begin: o.beginTool, end: spl } )
        model_wires.addWire(splitter.right.polygonData, o.offset, Math.random(), 1, 0.5, { begin: spl, end: o.endTool } )
        model_wires.remove(index)
        m_commonSlider.handle.visible = false
        
    }
    property Brush outlineBrush: Brush {
        color: "#04f"
    }
    
    property Pen outlinePen: Pen {
        color: "#0ff"
        width: 1.5
        cosmetic: true
        strokeStyle: Qt.DotLine
        capStyle: Qt.FlatCap
    }
    delegate: SmoothWireDelegate {
        pen: Pen {
            width: sl_lineWidth.value
            cosmetic: true
            color: UI.fromHSLum(hue, sat, lum)
        }
        radius: radiusSlider.value
        fixedScale: cb_bindScale.checked
        debugNodes: cb_showVertices.checked
        scene: m_scene
        precision: sl_precision.value
        hoverSlider: m_wireBuilder.slider
        //            outlineBrush: m_wireBuilder.outlineBrush
        outlinePen: m_wireBuilder.outlinePen
        function checkGuideType(object) {
            if (object.hasOwnProperty("guide") && object.hasOwnProperty("handle"))
                return true
            return false
        }
        Component.onCompleted: {
            if (tools.begin) {
                if (checkGuideType(tools.begin)) {
                    console.log("found begin tool", tools.begin)
                    beginTool = tools.begin
                }
            }
            if (tools.end) {
                if (checkGuideType(tools.end)) {
                    console.log("found end tool", tools.end)
                    endTool = tools.end
                }
            }
            
        }
    }
}
