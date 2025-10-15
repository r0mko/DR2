import QtQuick 2.3
Item{
    id: root
    property alias source: shader.src
    property alias animationSpeed: shader.speed
    ShaderEffect {
        id: shader
        anchors.fill: parent
        blending: true

        property var src
        property int time: 0
        property real speed: 50

        Timer{
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
        uniform lowp float qt_Opacity;
        uniform sampler2D src;

        void main(void)
        {
            /* if( mod( coord.x, 8.0 ) < 4.0 == mod( coord.y, 8.0 ) < 4.0 ) // checkerboard /**/
            highp vec4 c;
            if( mod( coord.x + coord.y - float(time), 8.0) > 4.0 )
                c = vec4( 1 );
            else
                c = vec4( vec3( 0 ), 1.0 );

            highp vec4 ic = texture2D( src, tcoord );

            if( ic.rgb == vec3( 1.0, 0.0, 1.0 ) )
                ic = c;
            else
                ic = vec4( 0 );

            gl_FragColor = ic * qt_Opacity;
        }'
    }
}
