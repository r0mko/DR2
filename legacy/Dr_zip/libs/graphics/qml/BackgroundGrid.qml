import QtQuick 2.3

ShaderEffect {
    id: grid
    property color fillColor
    property color gridColor
    property real fillOpacity
    property real gridMinOpacity
//    readonly property real gridMaxOpacity: 4 * gridMinOpacity

    property real currentStep
    property point pos
    property real step
    property real divisor: 2
    property real scaleFactor
    property real minimumStep
//    onCurrentStepChanged: console.log("currentStep", currentStep)
    onScaleFactorChanged: {
        var st = step * scaleFactor
        do {
            st /= divisor
        } while( st > minimumStep )
        currentStep = (st * divisor) / scaleFactor
    }

    vertexShader: "
        uniform highp mat4 qt_Matrix;
        attribute highp vec4 qt_Vertex;
        varying vec2 coord;
        void main() {
            coord = qt_Vertex.xy;
            gl_Position = qt_Matrix * qt_Vertex;
        }"
    fragmentShader: "
        varying highp vec2 coord;
        uniform highp vec2 pos;
        uniform highp float step;
        uniform highp float minimumStep;
        uniform highp float divisor;
        uniform highp float scaleFactor;

        uniform highp vec4 fillColor;
        uniform highp vec4 gridColor;
        uniform lowp float fillOpacity;
        uniform lowp float gridMinOpacity;

        void main() {
            highp vec2 c = vec2( coord.x + pos.x, coord.y + pos.y );
            highp vec4 fill = fillColor * fillOpacity;
            highp vec4 grid = gridColor * gridMinOpacity;
            highp vec4 res = fill;
            highp float st = step * scaleFactor;
            while (st < minimumStep)
                st *= divisor;
            do {
                if (mod(c.y, st) < 1.0 && mod(c.x, 2.0) < 1.0 || mod(c.x, st) < 1.0 && mod(c.y, 2.0) < 1.0)
                    res += grid;
                st /= divisor;
            } while ( st > minimumStep );

            gl_FragColor = clamp(res, vec4(0), vec4(0.4, 0.4, 0.4, 0.0));
        }
"
}

