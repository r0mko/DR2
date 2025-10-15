import QtQuick 2.3
Item{
    id: root
    property alias source: shader.src
    property alias color0: shader.color0
    property alias color1: shader.color1
    property alias animationSpeed: shader.speed
    property alias animate: _timer.running
    ShaderEffect {
        id: shader
        anchors.fill: parent
        blending: true

        property var src
        property int time: 0
        property real speed: 50
        property color color0: "#000000"
        property color color1: "#ffffff"

        Timer{
            id: _timer
            interval: shader.speed
            onTriggered: parent.time = ( parent.time + 1 ) % 8
            repeat: true
            Component.onCompleted: start()
        }

        vertexShader: "
        uniform highp mat4 qt_Matrix;
        attribute highp vec4 qt_Vertex;
        varying highp vec2 coord;
        varying highp vec2 tcoord;
        attribute highp vec2 qt_MultiTexCoord0;
        void main() {
            coord = qt_Vertex.xy;
            tcoord = qt_MultiTexCoord0;
            gl_Position = qt_Matrix * qt_Vertex;
        }"
        fragmentShader: '
        varying highp vec2 coord;
        varying highp vec2 tcoord;
        uniform int time;
        uniform vec4 color0;
        uniform vec4 color1;
        uniform int stepSize;
        uniform lowp float qt_Opacity;
        uniform sampler2D src;

        void main(void)
        {
            /* if( mod( coord.x, 8.0 ) < 2.0 == mod( coord.y, 4.0 ) < 2.0 ) // checkerboard /**/
            highp vec4 c;
            if( mod( coord.x + coord.y - float(time), 8.0) >= 4.0 )
                c = color0;
            else
                c = color1;

            highp vec4 ic = texture2D( src, tcoord );

            if( ic.rgb == vec3( 1.0, 0.0, 1.0 ) )
                ic = c;
            else
                ic = vec4( 0 );

            gl_FragColor = ic * qt_Opacity;
        }'
    }
}
