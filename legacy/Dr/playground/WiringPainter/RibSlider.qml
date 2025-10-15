import QtQuick 2.3
import QtGraphicalEffects 1.0
import Dr.Graphics 1.0
import Dr.UI 1.0

GuideTool {
    id: root

    property alias position: runner.position
    property alias atEnd: runner.atEnd
    property alias atStart: runner.atStart
    property alias mouseArea: m_handle.mouseArea

    property GrooveRunner runner: GrooveRunner {
        id: runner
        guide: root.guide
    }
    handle: RectHandle {
        id: m_handle
        x: runner.point.x
        y: runner.point.y
        visible: guide !== null
        rotation: guide ? runner.angle : 0
        border.color: mouseArea.containsMouse ? "#0ff" : "#fff"
        states: [
            State {
                name: "drag"
                when: mouseArea.pressed
                PropertyChanges {
                    target: mouseArea
                    onPositionChanged: {
                        var point = mapToItem(m_scene, mouseX, mouseY)
                        var p = runner.guide.nearestPos(Qt.point(point.x, point.y))
                        runner.position = p
                    }
                }
            }
        ]
    }
}


