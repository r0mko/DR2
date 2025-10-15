import QtQuick 2.5
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.3
import QtQuick.Window 2.0
import QtQuick.Dialogs 1.2

Item {
    id: root
    SystemPalette { id: palette; colorGroup: SystemPalette.Active }
    function getObject() {
        var ret = {}
        for (var i = 0; i < _model.count; ++i) {
            var h = _model.get(i)
            ret[h.key] = h.val.value
        }
//        console.log(JSON.stringify(ret))
        return ret
    }
    property var object
    property var currentValue
    signal accepted(var object)
    signal cancelled
    signal modified

    onObjectChanged: fillModel(object)

    Component.onCompleted: {
        var p = root.parent
        while (p)
            if (p.hasOwnProperty("globalPos")) {
                pos_tracker.target = p
                break;
            } else
                p = p.parent
    }
    property point screenPos


     MessageDialog {
        id: d_confirmDel
        icon: StandardIcon.Question
        standardButtons: StandardButton.Yes | StandardButton.Cancel
        onYes: a_delCurrent.trigger()
        text: visible ? 'Удалить запись "%1"?"'.arg(_model.get(styleData.row).key) : ""
    }

    Connections {
        id: pos_tracker
        target: null
        onGlobalPosChanged: {
            var rootPos = root.mapToItem(null, 0, 0)
            rootPos.x += pos_tracker.target.globalPos.x
            rootPos.y += pos_tracker.target.globalPos.y
            root.screenPos = rootPos
        }
    }

    QtObject {
        id: footer
        property real buttonWidth: item.width - _spacer.width + buttons_row.anchors.margins * 2 + buttons_row.spacing
        property Item item: Item {
            anchors.fill: parent
            clip: true
            Loader {
                anchors.fill: parent
                property var styleData: { "selected": false }
                sourceComponent: comp_formrow
            }
            RowLayout {
                id: buttons_row
                anchors.fill: parent
                spacing: -1
                anchors.margins: 4
                ToolButton {
                    style: s_button
                    action: a_addKV
                }
                ToolButton {
                    style: s_button
                    action: a_delCurrent
                }
                Item {
                    id: _spacer
                    Layout.fillWidth: true
                }
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        TableView {
            id: _table
            //                        alternatingRowColors: false
            Layout.fillWidth: true
            Layout.fillHeight: true

            signal requestEditor(int row)
            property Item currentEditor: null

            model: _model
            itemDelegate: Item{}
            rowDelegate: comp_formrow
            contentFooter: Item {
                implicitWidth: _table.width
                height: 25
                data: [footer.item]
            }
            Keys.onReturnPressed: {
                if (!currentEditor && currentRow >= 0) {
                    requestEditor(currentRow)
                } else {
                    event.accepted = false
                }
            }

            Keys.onEnterPressed: {
                if (!currentEditor && currentRow >= 0) {
                    requestEditor(currentRow)
                } else {
                    event.accepted = false
                }
            }

            NavigableTableViewColumn {
                id: col_key
                title: "Ключ"
                role: "key"
                width: 80
                delegate: Item {
                    clip: true
                    implicitWidth: Math.max(label.implicitWidth, footer.buttonWidth)
                    states: [
                        State {
                            name: "editing"
                            when: label.editor !== null
                            PropertyChanges {
                                target: _table
                                currentEditor: label.editor
                            }
                        }
                    ]
                    Rectangle {
                        color: "transparent"
                        //                        color: styleData.selected ? palette.highlight : "transparent"
                        //                        opacity: 0.3
                        anchors.fill: parent
                        border.width: styleData.selected ? 1 : 0
                        border.color: "#3679d7"
                        anchors.rightMargin: -3
                        radius: 3
                    }
                    EditableText {
                        id: label
                        anchors.fill: parent
                        editable: styleData.selected
                        value: styleData.value
                        placeholderText: "key"
                        onValueUpdated: {
                            var h = _model.get(styleData.row)
                            h.key = newValue
                        }
                    }
                }

            }
            NavigableTableViewColumn {
                id: vc
                title: "Значение"
                role: "val"
                property var components: { "string": comp_textEdit, "number": comp_numberEdit, "object": comp_object, "boolean": comp_boolean, "array": comp_array }
                width: _table.width - _table.getColumn(0).width - 2
                delegate: Item {
                    Loader {
                        id: _editorLoader
                        anchors.fill: parent
                        property var styleInfo: styleData
                        sourceComponent: vc.components[styleData.value.type]
                    }
                    Connections {
                        target: _table
                        onRequestEditor: {
                            if (row == styleData.row) {
                                console.log("Requested editor for", styleData.value)
                                _editorLoader.item.activateEditor()
                            }
                        }
                    }
                    Item {
                        id: del_btn
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.rightMargin: 2
                        width: b_delrow.width
                        opacity: m_area.containsMouse ? (m_area.containsPress ? 1.0 : 0.7) : 0.0
                        Behavior on opacity { NumberAnimation { duration: 150 } }
                        visible: styleData.selected

                        Rectangle {
                            id: b_delrow
                            anchors.verticalCenter: parent.verticalCenter
                            border.width: 1
                            border.color: "black"
                            color: "#ff8070"
                            width: b_label.implicitWidth + 8
                            height: b_label.implicitHeight - 2
                            radius: 3
                            Text {
                                //                                anchors.horizontalCente/r: parent.horizontalCenter
                                anchors.centerIn: parent
                                id: b_label
                                text: a_delCurrent.text
                                font.pixelSize: 13
                                color: "#000"
                            }
                        }


                        MouseArea {
                            anchors.fill: parent
                            id: m_area
                            hoverEnabled: true
                            acceptedButtons: Qt.LeftButton
                            onClicked: d_confirmDel.open()
                        }

                        //                        Button {
                        //                            anchors.fill: parent
                        //                            visible: styleData.selected
                        //                            opacity: hovered ? 1 : 0
                        //                            style: s_button
                        //                            action: a_delCurrent

                        //                        }
                    }

                }
            }
            Component.onCompleted: getColumn(0).resizeToContents()

        }
//        RowLayout {
//            Button {
//                action: a_apply
//            }
//            Button {
//                action: a_cancel
//            }
//        }
    }

    Component {
        id: comp_textEdit
        EditableText {
            id: ed_text
            Binding {
                target: _table
                property: "currentEditor"
                value: editor
                when: styleInfo.selected
            }

            displayText: '"%1"'.arg(styleInfo.value.value)
            value: String( styleInfo.value.value )
            textColor: styleInfo.selected ? "#fff" : "#000"
            editable: styleInfo.selected
            placeholderText: "текст"
            onValueUpdated: {
                var h = _model.get(styleInfo.row)
                h.val = { "value": newValue, "type": h.val.type }
            }
        }
    }
    Component {
        id: comp_numberEdit
        EditableText {
            Binding {
                target: _table
                property: "currentEditor"
                value: editor
                when: styleInfo.selected
            }

            value: Number( styleInfo.value.value )
            textColor: styleInfo.selected ? "#fff" : "#000"
            editable: styleInfo.selected
            placeholderText: "число"
            onEditorChanged: if (editor) editor.validator = num_validator

            DoubleValidator { id: num_validator }

            onValueUpdated: {
                var h = _model.get(styleInfo.row)
                h.val = { "value": newValue, "type": h.val.type }
            }
        }
    }

    Component {
        id: comp_object
        RowLayout {

            spacing: 2
            //            anchors.fill: parent
            Item {
                width: 2
            }
            Button {
                width: parent.heigth
                height: parent.heigth - 2
                text: "..."
                style: s_button
                onClicked: {
                    var pos = mapToItem(root, width, height)
                    objedit.row = styleInfo.row
                    objedit.x = root.screenPos.x + pos.x
                    objedit.y = root.screenPos.y + pos.y
                    objedit.visible = true
                }
            }

            Text {
                color: styleInfo.selected ? "#fff" : "#000"
                renderType: Text.NativeRendering
                verticalAlignment: Text.AlignVCenter
                //                font.pixelSize: 10
                Layout.fillWidth: true
                Layout.fillHeight: true
                elide: Text.ElideMiddle
                text: "<b>Object</b> (<i>%1</i>)".arg(Object.keys(styleInfo.value.value).join(", "))
            }


            Window {
                id: objedit
                width: 320
                height: 200
                visible: false
                modality: Qt.ApplicationModal
                flags: Qt.Tool
                property int row

                Loader {
                    id: editor_loader
                    active: objedit.visible
                    anchors.fill: parent
                    source: "JSObjectEditor.qml"
                    onActiveChanged: {
                        if (active) {
                            item.object = _model.get(objedit.row).val.value
                        }
                    }

                }

                Connections {
                    target: editor_loader.item
                    onAccepted: {
                        var h = _model.get(objedit.row)
                        h.val = { "value": object, "type": "object" }
                        console.log("object", JSON.stringify(object))
                        objedit.close()
                    }
                    onCancelled: objedit.close()
                }


            }
        }

    }

    Component {
        id: comp_array
        RowLayout {

            spacing: 2
            //            anchors.fill: parent
            Item {
                width: 2
            }
            Button {
                width: parent.heigth
                height: parent.heigth - 2
                text: "..."
                style: s_button
                onClicked: {
                    var pos = mapToItem(root, width, height)
                    arrayedit.row = styleInfo.row
                    arrayedit.x = root.screenPos.x + pos.x
                    arrayedit.y = root.screenPos.y + pos.y
                    arrayedit.visible = true
                }
            }

            Text {
                color: styleInfo.selected ? "#fff" : "#000"
                renderType: Text.NativeRendering
                verticalAlignment: Text.AlignVCenter
                //                font.pixelSize: 10
                Layout.fillWidth: true
                Layout.fillHeight: true
                elide: Text.ElideMiddle
                text: "<b>Array</b>[%1]".arg(styleInfo.value.value.length)
            }


            Window {
                id: arrayedit
                width: 320
                height: 200
                visible: false
                modality: Qt.ApplicationModal
                flags: Qt.Tool
                property int row

                Loader {
                    id: editor_loader
                    active: arrayedit.visible
                    anchors.fill: parent
                    source: "JSObjectEditor.qml"
                    onActiveChanged: {
                        if (active) {
                            item.object = _model.get(arrayedit.row).val.value
                        }
                    }
                }

                Connections {
                    target: editor_loader.item
                    onAccepted: {
                        var h = _model.get(arrayedit.row)
                        h.val = { "value": object, "type": "array" }
                        arrayedit.close()
                    }
                    onCancelled: arrayedit.close()
                }
            }
        }
    }

    Component {
        id: comp_boolean
        RowLayout {

            Item {
                id: _spacer
                width: 2
            }
            Switch {
//                checked: styleInfo.value.value
                Component.onCompleted: checked = styleInfo.value.value
                onCheckedChanged: {
                    _model.get(styleInfo.row).val = { "value": checked, "type": "boolean" }
                }
                style: SwitchStyle {
                    groove: Rectangle {
                        implicitWidth: 40
                        implicitHeight: 14
                        radius: 5
                        color: control.checked ? palette.highlight : palette.dark
                        border.color: palette.shadow
                        border.width: 1
                        clip: true
                        //                    Behavior on color { NumberAnimation { duration: 200 } }

                    }
                    handle: Rectangle {
                        width: 22
                        radius: 5
                        border.width: 1
                        border.color: palette.shadow
                        color: palette.window
                        Text {
                            anchors.fill: parent
                            font.pixelSize: 8
                            font.bold: true
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            text: control.checked ? "ON" : "OFF"
                        }
                    }
                }
            }
            Item {
                Layout.fillWidth: true
            }
        }

    }

    Component {
        id: s_button
        ButtonStyle {
            SystemPalette { id: _p; colorGroup: control.enabled ? SystemPalette.Active : SystemPalette.Disabled }
            background: Rectangle {
                implicitWidth: 28
                implicitHeight: 8
                border.width: 1
                border.color: palette.shadow
                radius: 3
                opacity: control.enabled ? 1.0 : 0.5
                gradient: Gradient {
                    GradientStop { position: 0 ; color: control.pressed ? "#ccc" : control.hovered ? _p.alternateBase : _p.base }
                    GradientStop { position: 1 ; color: control.pressed ? "#aaa" : control.hovered ? _p.highlight : _p.button }
                }
            }
            label: Item {
                implicitHeight: 8
                Text {
                    anchors.fill: parent
                    color: _p.buttonText
                    text: control.text
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.bold: true
                }
            }
        }
    }

    Menu {
        id: js_type
        MenuItem {
            text: "String"
            onTriggered: addKvPair("field%1".arg(_model.count), "", "string")
        }
        MenuItem {
            text: "Number"
            onTriggered: addKvPair("field%1".arg(_model.count), 0, "number")
        }
        MenuItem {
            text: "Boolean"
            onTriggered: addKvPair("field%1".arg(_model.count), true, "boolean")
        }
        MenuItem {
            text: "Object"
            onTriggered: addKvPair("field%1".arg(_model.count), {}, "object")
        }
        MenuItem {
            text: "Array"
            onTriggered: addKvPair("field%1".arg(_model.count), [], "array")
        }

    }
    function addKvPair(key, value, type) {
        if( type === 'array' ) {
            _model.append({key: key, val: { value: value, type: type }})
            _table.currentRow = _model.count - 1
            _table.selection.clear()
            _table.selection.select(_table.currentRow, _table.currentRow)
        }else{
            _model.append({key: key, val: { value: value, type: type }})
            _table.currentRow = _model.count - 1
            _table.selection.clear()
            _table.selection.select(_table.currentRow, _table.currentRow)
        }
    }

    Action {
        id: a_addKV
        text: "+"
        onTriggered: {
            js_type.popup()

        }
    }

    Action {
        id: a_delCurrent
        //        text: "✕"
        text: "✖"
        enabled: _table.currentRow >=0 && _table.currentRow < _model.count
        onTriggered: {
            _model.remove(_table.currentRow, 1)
            _table.currentRow = -1
            _table.selection.clear()
        }
    }

    Action {
        id: a_apply
        text: "OK"
        onTriggered: {
            root.object = _model.getObject()
        }
    }
    Action {
        id: a_cancel
        text: "Отмена"
        onTriggered: {
            root.fillModel(root.object)
            cancelled()
        }
    }

    Component {
        id: comp_formrow
        Item {
            height: 18
            width: parent.width
            clip: true
            Rectangle {
                color: styleData.selected ? "#3679d7" : palette.base
                radius: 3
                anchors.fill: parent
                anchors.topMargin: -2
                anchors.leftMargin: -2
                anchors.rightMargin: -2
                //                border.width: 1
                //                border.color: palette.dark
                Item {
                    height: parent.height
                    width: _table.getColumn(0).width + 2
                    clip: true
                    Rectangle {
                        anchors.fill: parent
                        color: "#e5e5e5"
                        border.color: palette.dark
                        border.width: 1
                        anchors.leftMargin: -1
                        anchors.topMargin: -1
                        anchors.bottomMargin: -1
                    }
                }
            }

        }
    }

    ListModel {
        id: _model
        onDataChanged: {
            currentValue = getObject()
            modified()
        }
        onRowsInserted: modified()
        onRowsRemoved: modified()
    }

    function fillModel(obj) {
        _model.clear()
        if( Array.isArray(obj) ) {
            obj.forEach( function( e, i ) {
                _model.append({ key: i, val: { value: e, type: typeof(e) }})
            })
        }else{
            for (var p in obj) {
                if (obj.hasOwnProperty(p)) {
//                    console.log( "started ", typeof(obj[p]), p)
                    _model.append({ key: p, val: { value: obj[p], type: Array.isArray(obj[p]) ? "array" : typeof(obj[p]) }})
                }
            }
        }
    }

}
