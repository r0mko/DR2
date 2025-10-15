import QtQuick 2.3
import QtQuick.Controls 1.3

Item {
    id: root
    clip: true
    implicitHeight: _text.implicitHeight
    implicitWidth: _text.contentWidth + _text.anchors.leftMargin + _text.anchors.rightMargin + 2
    signal valueUpdated(var newValue)
    signal tabPressed(TextFieldEditor editor)

    property alias font: _text.font
    property alias horizontalAlignment: _text.horizontalAlignment
    property string placeholderText
    property string value
    property string displayText: value
    property bool delayEdit: false

    property Component editorComponent: TextFieldEditor {
        id: _editor
        font: _text.font
//        textColor: root.textColor
        placeholderText: root.placeholderText
        horizontalAlignment: _text.horizontalAlignment
        onValueChanged: valueUpdated(text)
        Component.onCompleted: {
            text = value
            selectAll()
        }
        onTabPressed: root.tabPressed(_editor)
    }
    property TextField editor: _loader.item
    property alias textColor: _text.color
    property alias mouseArea: _mouseArea
    property bool editable

    signal editorActivated(TextFieldEditor editor)
    signal editorDeactivated

    states: [
        State {
            name: "editing"
            PropertyChanges {
                target: _loader
                visible: true
                sourceComponent: editorComponent
            }
            PropertyChanges {
                target: root
                onEditableChanged: editor.editingFinished()
            }

        }
    ]

    Connections {
        target: _loader.item
        ignoreUnknownSignals: true
        onEditingFinished: root.state = ""
    }

    Text {
        id: _text
        anchors.fill: parent
        anchors.leftMargin: 4
        anchors.rightMargin: 4
        renderType: Text.NativeRendering
        text: displayText
        elide: Text.ElideMiddle
        verticalAlignment: Text.AlignVCenter
    }

    Timer {
        id: _clickTimer
        interval: delayEdit ? 500 : 1
        running: false
        onTriggered: root.state = "editing"
    }

    MouseArea {
        id: _mouseArea
        anchors.fill: parent
        propagateComposedEvents: true
        enabled: editable
//        onPressed: {
//            if (!editable) mouse.accepted = false
//        }
        onClicked: _clickTimer.restart()
        onDoubleClicked: {
            _clickTimer.stop()
            mouse.accepted = false
        }
    }

    Loader {
        id: _loader
        visible: false
        anchors.fill: parent
        sourceComponent: null
        onLoaded: editorActivated(item)
//        onSourceComponentChanged: console.log(sourceComponent   )
    }

    function activateEditor() {
        if (editable) {
            state = "editing"
        }
    }
}

