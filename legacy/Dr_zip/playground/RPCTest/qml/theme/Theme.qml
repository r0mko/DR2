pragma Singleton
import QtQuick 2.3
import Dr.QRPC 1.0

Object {
    property alias defaultFont: _txt.font
    property alias colors: colors
    property var bgColorSequence: [""]

//    FontLoader { id: font_helv_black; source: "http://dr.pge.local/qml/fonts/HelveticaNeueCyr-Black.otf" }
//    FontLoader { id: font_helv_blackitalic; source: "http://dr.pge.local/qml/fonts/HelveticaNeueCyr-BlackItalic.otf" }
//    FontLoader { id: font_helv_bold; source: "http://dr.pge.local/qml/fonts/HelveticaNeueCyr-Bold.otf" }
//    FontLoader { id: font_helv_bolditalic; source: "http://dr.pge.local/qml/fonts/HelveticaNeueCyr-BoldItalic.otf" }
//    FontLoader { id: font_helv_heavy; source: "http://dr.pge.local/qml/fonts/HelveticaNeueCyr-Heavy.otf" }
//    FontLoader { id: font_helv_heavyitalic; source: "http://dr.pge.local/qml/fonts/HelveticaNeueCyr-HeavyItalic.otf" }
//    FontLoader { id: font_helv_italic; source: "http://dr.pge.local/qml/fonts/HelveticaNeueCyr-Italic.otf" }
//    FontLoader { id: font_helv_light; source: "http://dr.pge.local/qml/fonts/HelveticaNeueCyr-Light.otf" }
//    FontLoader { id: font_helv_lightitalic; source: "http://dr.pge.local/qml/fonts/HelveticaNeueCyr-LightItalic.otf" }
//    FontLoader { id: font_helv_medium; source: "http://dr.pge.local/qml/fonts/HelveticaNeueCyr-Medium.otf" }
//    FontLoader { id: font_helv_mediumitaicl; source: "http://dr.pge.local/qml/fonts/HelveticaNeueCyr-MediumItaicl.otf" }
//    FontLoader { id: font_helv_roman; source: "http://dr.pge.local/qml/fonts/HelveticaNeueCyr-Roman.otf" }
//    FontLoader { id: font_helv_thin; source: "http://dr.pge.local/qml/fonts/HelveticaNeueCyr-Thin.otf" }
//    FontLoader { id: font_helv_thinitalic; source: "http://dr.pge.local/qml/fonts/HelveticaNeueCyr-ThinItalic.otf" }
//    FontLoader { id: font_helv_ultralight; source: "http://dr.pge.local/qml/fonts/HelveticaNeueCyr-UltraLight.otf" }
//    FontLoader { id: font_helv_ultralightitalic; source: "http://dr.pge.local/qml/fonts/HelveticaNeueCyr-UltraLightItalic.otf" }

    Text {
        id: _txt
        font {
            family:"Roboto"
//            family:font_helv_ultralightitalic.name
            pointSize: 12
//            weight: Font.Light
        }
        Component.onCompleted: console.log(font.family)
    }

    Object {
        id: colors
        property color number: "#809fff"
        property color dataType: "#80ffff"
        property color text: "#fff"
        property color stringType: "#Fc5"
        property color referene: "#8d9"
    }

}
