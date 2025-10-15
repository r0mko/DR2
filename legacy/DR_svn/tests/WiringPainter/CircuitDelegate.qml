import QtQuick 2.3
import "CircuitDelegate"

Canvas {
    id: canvas
    renderTarget: Canvas.FramebufferObject
    renderStrategy: Canvas.Cooperative

    property Circuit circuit

    Component.onCompleted: {
        requestPaint()
    }

    Connections {
        target: drawboard
        onHandlesMoved: requestPaint()
    }

    onPaint: {
        var ctx = canvas.getContext("2d")
        ctx.clearRect(0, 0, width, height)

        for (var i = 0; i < circuit.fillets.count; ++i) {
            var f = circuit.fillets.get(i).i
            f.calc()
            f.stroke(ctx, w, circuit.color)
        }
        for (i = 0; i < circuit.segments.count; ++i) {
//            seg.strokeAxis(ctx)
            var seg = circuit.segments.get(i).i
            seg.stroke(ctx, w, circuit.color)
        }
    }
}
