import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4

Column {
    id: root
    spacing: 1
    property var object
    property var model
    property alias __repeater: par_form
    height: (par_form.h + spacing) * par_form.count + spacing
//    height: par_form.h * par_form.count
    //    default property alias children: par_form.children
    property Component valueDelegate: Component {
        Label {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            text: modelData
            font.family: "Roboto"
            font.weight: Font.Light
        }
    }

    onObjectChanged: {
        _model.clear()
        if (object && object instanceof Object) {
            for (var k in object) {
                if (object.hasOwnProperty(k))
                    _model.append( { "key": String(k), "value": String(object[k]) } )
            }
        }
    }
    onModelChanged: {
        _model.clear()
        if (model && model instanceof Array) {
            model.forEach(function(e) {
                if (!e)
                    return
                if (e.hasOwnProperty("key") && (e.hasOwnProperty("value"))) {
                    _model.append( { "key": String(k), "value": String(object[k]) } )
                } else {
                    for (var k in e) {
                        if (e.hasOwnProperty(k))
                            _model.append( { "key": String(k), "value": String(e[k]) } )
                    }
                }
            })
        }
    }
    SystemPalette { id: sysPalette }
    Repeater {
        id: par_form
        width: parent.width
        property int w
        property int h: 20
        model: _model
        function fixColumnWidth(item) {
            if (item && item.hasOwnProperty("keyLabelWidth"))
                if (par_form.w < item.keyLabelWidth) par_form.w = item.keyLabelWidth
        }
        delegate: JSObjectViewRow {
            labelWidth: par_form.w
            text: key
            valueDelegate: TextInput {
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 5
                font.family: "Roboto"
                font.weight: Font.Light
                text: value
                readOnly: true
                selectByMouse: true
//                activeFocusOnPress: false
                selectedTextColor: sysPalette.highlightedText
                selectionColor: sysPalette.highlight
                renderType: Text.NativeRendering
            }
            repeater: par_form
        }
    }
    ListModel { id: _model }
}
