import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import Dr.MetaGraph 1.0
import QtQuick.Dialogs 1.2
import Dr.UI 1.0

Item {
    id: root
    property TypeBuilder builder
    property var handle: objectData.handle

    signal done
    signal cancelled

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent

        JSObjectView {
            id: heading
            //            enabled: !registrationSwitch.checked
            Layout.fillWidth: true
            JSObjectViewRow {
                text: "Название"
                valueDelegate: EditableText {
                    id: className
                    height: row2.rowHeight
                    font.family: "Consolas"
                    Layout.fillWidth: true
                    editable: true
                    delayEdit: false
                    value: builder.className
                    editorComponent: TextFieldEditor {
                        text: builder.className
                        font: className.font
                        onValueChanged: builder.className = value
                    }
                }
            }
            JSObjectViewRow {
                id: row2
                text: "Базовый класс"
                valueDelegate: EditableText {
                    id: baseClassName
                    property var objects: []
                    height: row2.rowHeight
                    Layout.fillWidth: true
                    editable: true
                    font.family: "Consolas"
                    onEditorActivated: objects = builder.qObjectTypes()
                    delayEdit: false
                    value: builder.baseClassName
                    editorComponent: TextFieldEditor {
                        id: baseClassName_edit
                        text: builder.baseClassName
                        onValueChanged: builder.baseClassName = value
                        TextFieldCompleter {
                            textField: baseClassName_edit
                            onSuggestionAccepted: baseClassName_edit.setValue(suggestion)
                            completionModel: baseClassName.objects
                            //                            textProperty: "typeName"
                        }
                    }
                }
            }
        }

        TabView {

            Layout.fillHeight: true
            Layout.fillWidth: true

            Tab {
                id: tab_props
                title: "Свойства"


                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    property var h2: root.handle

                    onH2Changed: {
                        propTable.model = null
                        builder.clear()
                        builder.loadTypeDef(h2)
                        propTable.model = builder.properties
                    }

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 8

                        TableView
                        {

                            id: propTable
                            enabled: !registrationSwitch.checked
                            property bool isNewProperty: false
                            property var currentRowItems
                            __listView.onCurrentItemChanged: currentRowItems = __listView.currentItem.children[0].children[1].children
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            onEnabledChanged: if (!enabled) {
                                                  selection.clear()
                                                  currentRow = -1
                                              }

                            rowDelegate: Rectangle {
                                color: styleData.selected ? '#3679d7' : styleData.alternate ? '#edf3fe' : '#fff'
                                border.width: (propTable.isNewProperty && styleData.row === propTable.currentRow) ? 2 : 0
                                border.color: "#00FFFF"
                                height: 18
                                width: parent.width
                                MouseArea {
                                    anchors.fill: parent
                                    acceptedButtons: Qt.RightButton
                                    onClicked: ctxmenu.popup()
                                }
                            }

                            Menu {
                                id: ctxmenu
                                MenuItem {
                                    action: a_addProperty
                                }
                            }

                            Action {
                                id: a_addProperty
                                shortcut: "Ctrl+N";
                                onTriggered: {
                                    propTable.selection.clear()
                                    propTable.isNewProperty = true
                                    var row = builder.properties.length
                                    builder.addProperty("", "", true)
                                    propTable.model = builder.properties
                                    propTable.selection.clear()
                                    propTable.selection.select(row)
                                    propTable.currentRow = row

                                }
                                text: qsTr("Добавить свойство")
                            }
                            Action {
                                id: a_removeProperty
                                enabled: propTable.currentRow >= 0 && propTable.currentRow < builder.properties.length && !propTable.isNewProperty
                                onTriggered: {
                                    console.log("Удаляем свойство", propTable.currentRow)
                                    builder.removeProperty(builder.properties[propTable.currentRow])
                                    propTable.currentRow = -1

                                }
                                text: qsTr("Удалить свойство")
                            }

                            NavigableTableViewColumn {
                                id: type_col
                                role: "type"
                                title: "Type"
                                width: 200

                                delegate: ColumnDelegate {

                                    id: col_type
//                                    view: propTable
                                    value: styleData.value
                                    editable: isCurrent
                                    onEditableChanged: {
                                        if(!builder.isValidType(value)) {
                                            console.log("activate editor for incorrect type", value)
                                            //                                            delayTypeEditor.restart()
                                            col_type.activateEditor()
                                        }
                                    }

                                    font.family: "Consolas"
                                    tint: "#60ff0000"


                                    //                                fontFamily: "Consolas"
                                    //                                    states: [

                                    //                                        State {
                                    //                                            name: "newprop"
                                    //                                            when: propTable.isNewProperty && propTable.currentRow === styleData.row
                                    //                                            StateChangeScript { script: activateEditor() }
                                    //                                            PropertyChanges {
                                    //                                                target: col_type
                                    //                                                onEditorDeactivated: {
                                    //                                                    propTable.currentRowItems[styleData.column + 1].item.activateEditor()
                                    //                                                }
                                    //                                            }
                                    //                                        }
                                    //                                    ]

                                    editorComponent: TextFieldEditor {
                                        id: edit
                                        text: styleData.value
                                        font: col_type.font
                                        placeholderText: "тип"
                                        validator: typeValidator
                                        onValueChanged:  {
                                            //                                            console.log(text, value)
                                            if (styleData.value === value)
                                                return
                                            modelData[styleData.role] = text
                                            var currentCell = type_col.findDelegateItem(styleData.row, styleData.column)
                                            var nextCell = type_col.findDelegateItem(styleData.row, styleData.column + 1)
                                            //                                            console.log(styleData.row, styleData.column, nextCell.value, nextCell.editable, nextCell)
                                            //                                            if (nextCell && nextCell.value == "") {
                                            //                                                currentCell.focus = false
                                            //                                                nextCell.forceActiveFocus()
                                            //                                                nextCell.activateEditor()
                                            //                                            }

                                        }
//                                        ClassCompleter {
//                                            id: completer
//                                            textField: edit
//                                            onSuggestionAccepted: edit.setValue(suggestion)
//                                            completionModel: builder.typesInfo
//                                            textProperty: "typeName"
//                                        }
                                    }
                                }
                            }

                            NavigableTableViewColumn {
                                id: name_col
                                role: "name"
                                title: "Name"
                                delegate: ColumnDelegate {
                                    id: col_name
//                                    view: propTable
                                    editable: isCurrent
                                    value: styleData.value
                                    textColor: styleData.textColor
                                    onValueUpdated: modelData[styleData.role] = newValue
                                    font.family: "Consolas"

                                    //                                    editorComponent: TextFieldEditor {
                                    //                                        text: styleData.value
                                    ////                                        textColor: col_name.textColor
                                    //                                        placeholderText: "название"
                                    //                                        onValueChanged: {
                                    //                                            modelData[styleData.role] = value
                                    //                                            console.log("entered value", value, styleData.role)
                                    //                                        }
                                    //                                    }
                                }
                            }

                            TableViewColumn {
                                role: "readable"
                                title: "R"
                                width: 20
                                resizable: false
                                delegate: BoolColumnDelegate {
                                    enabled: styleData.row === propTable.currentRow
                                    value: styleData.value
                                    onValueUpdated: modelData[styleData.role] = value
                                }
                            }

                            TableViewColumn {
                                role: "writable"
                                title: "W"
                                width: 20
                                resizable: false
                                delegate: BoolColumnDelegate {
                                    enabled: styleData.row === propTable.currentRow
                                    value: styleData.value
                                    onValueUpdated: modelData[styleData.role] = value
                                }
                            }
                            TableViewColumn {
                                role: "constant"
                                title: "C"
                                width: 20
                                resizable: false
                                delegate: BoolColumnDelegate {
                                    enabled: styleData.row === propTable.currentRow
                                    value: styleData.value
                                    onValueUpdated: modelData[styleData.role] = value
                                }
                            }
                        }
                        RowLayout {
                            Layout.fillWidth: true
                            Button {
                                width: 30
                                text: "Добавить..."
                                enabled: !propTable.isNewProperty
                                action: a_addProperty
                            }
                            Button {
                                width: 30
                                text: "Удалить..."

                                action: a_removeProperty
                            }
                        }
                    }
                }
            }

            Tab {
                id: tab_methods
                title: "Методы"

                Item {
                    Layout.fillHeight: true
                    Layout.minimumWidth: 300

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 8

                        TableView {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            model: builder.methods

                            TableViewColumn {
                                role: "methodType"
                                title: "Тип"
                                width: 60

                                delegate: ColumnDelegate {
                                    //                                text.text: {
                                    //                                    var m = view.model[styleData.row]
                                    //                                    switch (m.methodType) {
                                    //                                    case MethodBuilder.Slot:
                                    //                                        return "slot"
                                    //                                    case MethodBuilder.Signal:
                                    //                                        return "sig"
                                    //                                    case MethodBuilder.Method:
                                    //                                        return "method"
                                    //                                    default:
                                    //                                        return "???"
                                    //                                    }
                                    //                                }
                                    //                                text.horizontalAlignment: Text.AlignHCenter

                                    Rectangle {
                                        anchors.fill: parent
                                        anchors.margins: 1
                                        z: -1
                                        color: {
                                            var m = view.model[styleData.row]
                                            switch (m.methodType) {
                                            case MethodBuilder.Slot:
                                                return "#ff8000"
                                            case MethodBuilder.Signal:
                                                return "#0080ff"
                                            case MethodBuilder.Method:
                                                return "#80ff80"
                                            default:
                                                return "transparent"
                                            }
                                        }
                                        border.width: 1
                                        opacity: 0.2
                                        radius: 3
                                    }
                                }
                            }

                            TableViewColumn {
                                role: "returnType"
                                title: "Returns"
                                width: 100
                                delegate: ColumnDelegate {}
                            }

                            TableViewColumn {
                                role: "signature"
                                title: "Signature"
                                delegate: ColumnDelegate {}
                                width: 120
                            }
                        }
                    }
                }

            }
        }

        RowLayout {
            //            enabled: !registrationSwitch.checked
            Button {
                text: "Применить"
                //                enabled: _className.text.text.length > 0
                onClicked: {
                    builder.save(objectData.handle)


                    //objectData.nodeId = название
                    //                    var h = builder.create()
                    //                    console.log("created type", h)
                    //                    done()
                }
            }

            //            Button {
            //                text: "Зарегистрировать"
            //                enabled: className.text.text.length > 0
            //                onClicked: {
            //                    builder.registerType()
            //                    //objectData.nodeId = название
            ////                    var h = builder.create()
            //                    console.log("created type", h)
            //                    done()
            //                }
            //            }
            Button {
                text: "Отмена"
                onClicked: cancelled()
            }

            Item {
                id: _spacer
                Layout.fillWidth: true
            }
        }
        RowLayout {
            Label {
                text: 'Register type'
            }
            Switch {
                id: registrationSwitch
                checked: !!!!!!!!objectData.type$isRegistered
                onCheckedChanged:
                    if( checked )
                        builder.registerType(objectData.handle)
                    else
                        builder.unregisterType(objectData.handle)
            }
            Label {
                text: 'MetatypeId: ' + objectData.type$metaTypeId
            }
        }

    }
    TypeValidator {
        id: typeValidator
        typeFactory: builder
    }
}
