import QtQuick 2.3
import Dr.UI 1.0
import Dr 1.0

Object {
    property alias bold: bold
    property alias bolditalic: bolditalic
    property alias italic: italic
    property alias regular: regular
    property alias bold_cond: bold_cond
    property alias bolditalic_cond: bolditalic_cond
    property alias italic_cond: italic_cond
    property alias regular_cond: regular_cond
    FontLoader { id: bold; source: "../../fonts/texgyreheros-bold.otf" }
    FontLoader { id: bolditalic; source: "../../fonts/texgyreheros-bolditalic.otf" }
    FontLoader { id: italic; source: "../../fonts/texgyreheros-italic.otf" }
    FontLoader { id: regular; source: "../../fonts/texgyreheros-regular.otf" }
    FontLoader { id: bold_cond; source: "../../fonts/texgyreheroscn-bold.otf" }
    FontLoader { id: bolditalic_cond; source: "../../fonts/texgyreheroscn-bolditalic.otf" }
    FontLoader { id: italic_cond; source: "../../fonts/texgyreheroscn-italic.otf" }
    FontLoader { id: regular_cond; source: "../../fonts/texgyreheroscn-regular.otf" }
}
