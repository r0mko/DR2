import QtQuick 2.3

Item {
    id: root
    anchors.fill: parent
    property alias color1: shader.color1
    property alias color2: shader.color2
    property alias size: shader.size
    ShaderEffect {
        id: shader
        anchors.fill: parent
        blending: true

        property color color1: '#555'
        property color color2: '#fff'
        property int size: 8

        vertexShader: "
            uniform highp mat4 qt_Matrix;
            attribute highp vec4 qt_Vertex;
            varying highp vec2 coord;
            varying highp vec2 tcoord;
            attribute highp vec2 qt_MultiTexCoord0;
            void main() {
                coord = qt_Vertex.xy;
                gl_Position = qt_Matrix * qt_Vertex;
            }"
        fragmentShader: '
//precision mediump float;
            varying highp vec2 coord;
            varying highp vec2 tcoord;

            uniform lowp float qt_Opacity;
            uniform int size;
            uniform highp vec4 color1;
            uniform highp vec4 color2;

            void main(void)
            {
                highp vec4 c;
                highp float s;
                s = float(size);
                if( mod( coord.x, s * 2.0 ) < s == mod( coord.y, s * 2.0 ) < s )
                    c = color1;
                else
                    c = color2;
                gl_FragColor = c * qt_Opacity;
            }'
    }
}
