import QtQuick 2.3
import Dr 1.0
import Dr.UI 1.0

Object {

    property alias black: black
    property alias blackitalic: blackitalic
    property alias bold: bold
    property alias bolditalic: bolditalic
    property alias extrabold: extrabold
    property alias extrabolditalic: extrabolditalic
    property alias extralight: extralight
    property alias extralightitalic: extralightitalic
    property alias italic: italic
    property alias light: light
    property alias lightitalic: lightitalic
    property alias medium: medium
    property alias mediumitalic: mediumitalic
    property alias regular: regular
    property alias semibold: semibold
    property alias semibolditalic: semibolditalic
    property alias thin: thin
    property alias thinitalic: thinitalic

    FontLoader { id: black;             source: "../../fonts/Exo2-Black.ttf" }
    FontLoader { id: blackitalic;       source: "../../fonts/Exo2-BlackItalic.ttf" }
    FontLoader { id: bold;              source: "../../fonts/Exo2-Bold.ttf" }
    FontLoader { id: bolditalic;        source: "../../fonts/Exo2-BoldItalic.ttf" }
    FontLoader { id: extrabold;         source: "../../fonts/Exo2-ExtraBold.ttf" }
    FontLoader { id: extrabolditalic;   source: "../../fonts/Exo2-ExtraBoldItalic.ttf" }
    FontLoader { id: extralight;        source: "../../fonts/Exo2-ExtraLight.ttf" }
    FontLoader { id: extralightitalic;  source: "../../fonts/Exo2-ExtraLightItalic.ttf" }
    FontLoader { id: italic;            source: "../../fonts/Exo2-Italic.ttf" }
    FontLoader { id: light;             source: "../../fonts/Exo2-Light.ttf" }
    FontLoader { id: lightitalic;       source: "../../fonts/Exo2-LightItalic.ttf" }
    FontLoader { id: medium;            source: "../../fonts/Exo2-Medium.ttf" }
    FontLoader { id: mediumitalic;      source: "../../fonts/Exo2-MediumItalic.ttf" }
    FontLoader { id: regular;           source: "../../fonts/Exo2-Regular.ttf" }
    FontLoader { id: semibold;          source: "../../fonts/Exo2-SemiBold.ttf" }
    FontLoader { id: semibolditalic;    source: "../../fonts/Exo2-SemiBoldItalic.ttf" }
    FontLoader { id: thin;              source: "../../fonts/Exo2-Thin.ttf" }
    FontLoader { id: thinitalic;        source: "../../fonts/Exo2-ThinItalic.ttf" }
}
