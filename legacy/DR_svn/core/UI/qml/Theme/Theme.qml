pragma Singleton
import QtQuick 2.3
import DataReactor.UI 1.0
import DataReactor 1.0

Object {
    id: root
    readonly property alias palette: palette
//    property alias exo2: exo2
//    property alias roboto: roboto
//    property alias texgyreheros: texgyreheros
    readonly property alias defaultFont: _txt.font


    Object {
        id: palette;
        readonly property alias formColors: formColors
        readonly property var lightSeq: ["#ff8686","#ffb973","#fff373","#dbff75","#82ff82","#86ffcf","#8affff","#86c3ff","#aaaaff","#cc99ff","#ed86ed","#fb76b8"]
        readonly property var midSeq:  ["#bf2d2d","#bd7630","#bba82f","#76bd30","#2cae2c","#2bab6b","#289e9e","#3882cd","#3d50e4","#6a3ad4","#8e31ac","#bd3076"]
        readonly property var pastelSeq: ["#ac5353","#ac8053","#aca653","#79ac53","#53ac53","#53ac77","#53acac","#6089b2","#7777bd","#8e66b5","#9d559d","#ac5380"]
        readonly property var darkSeq: ["#650000","#653200","#655300","#576500","#236500","#006035","#005757","#003c79","#2124a5","#3e0782","#4d0059","#650032"]
        readonly property var mainSeq: ["#ff0000","#ff8000","#ffff00","#80ff00","#00ff00","#00ff80","#00ffff","#0080ff","#0000ff","#8000ff","#ff00ff","#ff0080"]
        readonly property var darkPastel: ["#582929","#584129","#585429","#415829","#295829","#295841","#275353","#284159","#2e3a70","#3b3169","#4a2c5f","#58294e"]
        readonly property var grays: ["#111","#222","#333","#444","#555","#666","#777","#888","#999","#aaa","#bbb","#ccc","#ddd","#eee"]

        Object {
            id: formColors;
            readonly property alias type: type
            readonly property color dataType: "#80ffff"
            Object {
                id: type;
                readonly property color number: "#809fff"
                readonly property color text: "#fff"
                readonly property color string: "#fc5"
                readonly property color reference: "#86abff"
                readonly property color object: "#d36c8e"
            }
        }
    }

//    FontLoader { id: entypo; source: "../fonts/entypo.ttf" }
//    FontLoader { id: fontawesome; source: "../fonts/FontAwesome.otf" }

    Text {
        id: _txt
        font {
//            family: root.roboto.regular.name
            //            family:font_helv_ultralightitalic.name
            pointSize: 12
            //            weight: Font.Light
        }
    }
}
