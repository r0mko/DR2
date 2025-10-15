import QtQuick 2.0
import Dr.UI 1.0
import Dr 1.0

Object {
    property alias mono: mono
    Object {
        id: mono
        property alias bold_italic: mono_bolditalic
        property alias bold: mono_bold
        property alias regular_italic: mono_regularitalic
        property alias regular: mono_regular
        FontLoader { id: mono_bolditalic; source: "../../fonts/UbuntuMono-BI.ttf" }
        FontLoader { id: mono_bold; source: "../../fonts/UbuntuMono-B.ttf" }
        FontLoader { id: mono_regularitalic; source: "../../fonts/UbuntuMono-RI.ttf" }
        FontLoader { id: mono_regular; source: "../../fonts/UbuntuMono-R.ttf" }
    }
    property alias condensed: condensed
    property alias regular_italic: regularitalic
    property alias light: light
    property alias bold_italic: bolditalic
    property alias light_italic: lightitalic
    property alias regular: regular
    property alias bold: bold
    property alias medium: medium
    property alias medium_italic: mediumitalic
    FontLoader { id: condensed; source: "../../fonts/Ubuntu-C.ttf" }
    FontLoader { id: regularitalic; source: "../../fonts/Ubuntu-RI.ttf" }
    FontLoader { id: light; source: "../../fonts/Ubuntu-L.ttf" }
    FontLoader { id: bolditalic; source: "../../fonts/Ubuntu-BI.ttf" }
    FontLoader { id: lightitalic; source: "../../fonts/Ubuntu-LI.ttf" }
    FontLoader { id: regular; source: "../../fonts/Ubuntu-R.ttf" }
    FontLoader { id: bold; source: "../../fonts/Ubuntu-B.ttf" }
    FontLoader { id: medium; source: "../../fonts/Ubuntu-M.ttf" }
    FontLoader { id: mediumitalic; source: "../../fonts/Ubuntu-MI.ttf" }
}
