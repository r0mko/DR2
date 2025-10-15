import QtQuick 2.3

Canvas {
    id: canvas
    anchors.fill: parent
    property real lineWidth: 3
    property color color: "#FFEE00"
    renderStrategy: Canvas.Cooperative
    renderTarget: Canvas.FramebufferObject

    Connections {
        target: i
        onModelChanged: requestPaint()
    }

    Component.onCompleted: {
        console.log("graph delegate", canvas, canvas.parent, i)
    }

    //    property var paint: function(ctx) {}
    onPaint: {
        var ctx = getContext("2d")
        ctx.clearRect(0, 0, width, height)
        ctx.strokeStyle = color
        ctx.lineWidth = lineWidth
        ctx.beginPath()
        for (var n = 0; n < i.edgeCount; ++n) {
            var e = i.edges.get(n).i
            e.paint(ctx)
        }
        ctx.stroke()
    }
}
