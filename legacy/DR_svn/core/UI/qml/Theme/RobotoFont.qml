import QtQuick 2.3
import DataReactor.UI 1.0
import DataReactor 1.0

Object {

    property alias black: black
    property alias black_italic: blackitalic
    property alias bold: bold
    property alias bold_italic: bolditalic
    property alias italic: italic
    property alias light: light
    property alias light_italic: lightitalic
    property alias medium: medium
    property alias medium_italic: mediumitalic
    property alias regular: regular
    property alias thin: thin
    property alias thin_italic: thinitalic

    FontLoader { id: black;         source: "../../fonts/Roboto-Black.ttf" }
    FontLoader { id: blackitalic;   source: "../../fonts/Roboto-BlackItalic.ttf" }
    FontLoader { id: bold;          source: "../../fonts/Roboto-Bold.ttf" }
    FontLoader { id: bolditalic;    source: "../../fonts/Roboto-BoldItalic.ttf" }
    FontLoader { id: italic;        source: "../../fonts/Roboto-Italic.ttf" }
    FontLoader { id: light;         source: "../../fonts/Roboto-Light.ttf" }
    FontLoader { id: lightitalic;   source: "../../fonts/Roboto-LightItalic.ttf" }
    FontLoader { id: medium;        source: "../../fonts/Roboto-Medium.ttf" }
    FontLoader { id: mediumitalic;  source: "../../fonts/Roboto-MediumItalic.ttf" }
    FontLoader { id: regular;       source: "../../fonts/Roboto-Regular.ttf" }
    FontLoader { id: thin;          source: "../../fonts/Roboto-Thin.ttf" }
    FontLoader { id: thinitalic;    source: "../../fonts/Roboto-ThinItalic.ttf" }
}
