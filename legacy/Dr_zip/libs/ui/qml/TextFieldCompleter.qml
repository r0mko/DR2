import QtQuick 2.0
import QtQuick.Controls 1.1

QtObject {
    id: root
    property int maxSuggestions: 10
    property Item suggestionList: null
    property TextField textField
    property int suggestKey: Qt.Key_Space
    property int suggestKeyModifier: Qt.MetaModifier
    property var suggestions: []
    property var completionModel: []
    property bool caseSensitive: false
    signal requestScopeUpdate
    signal suggestRequested(string prefix)
    signal suggestionAccepted(string suggestion)
    signal listPopup
    property string textProperty
    property string selectedSuggestion
    property string prefix: extractToken()

//    signal keyPressed(var event)
    property Item listParent: null

    Component.onDestruction: {
        if (suggestionList)
            suggestionList.destroy()
    }

    onTextFieldChanged: {
        console.log("textField changed", textField)
        if (textField) {
            textField.Component.destruction.connect(tfout)
            var pos = textField.mapToItem(null, 0, 0)
            var p = textField
            while(p.parent)
                p = p.parent
            listParent = p
        }
    }
    function tfout() {
        console.log("textfield destroyed")
        root.destroy()
    }

    property Item __listener: Item {
        id: _listener
        Keys.onPressed: {
            console.log("intercepted keypress")
            if ((event.key === suggestKey) && (event.modifiers & suggestKeyModifier)) {
                event.accepted = true
                popupList()
            } else
                event.accepted = false
        }
    }
    property Item __handler: Item {
        id: _handler

        property int tokenStart
        property int tokenEnd
        Keys.onEnterPressed: acceptSuggestion(suggestions[suggestionList.currentIndex])
        Keys.onReturnPressed: acceptSuggestion(suggestions[suggestionList.currentIndex])
        Keys.onDownPressed: suggestionList.view.incrementCurrentIndex()
        Keys.onUpPressed: suggestionList.view.decrementCurrentIndex()
        Keys.onEscapePressed: {
            suggestionList.destroy()
            suggestionList = null
        }
    }

    property Component suggestionListComponent: Component {
        ScrollView {
            id: _view
            property var pos: textField.mapToItem(listParent, textField.cursorRectangle.x, textField.cursorRectangle.y + textField.cursorRectangle.height + 4)
            x: pos.x
            y: pos.y
            frameVisible: true
            property alias view: m_list
            property alias currentIndex: m_list.currentIndex

            Rectangle {
                z: -1
                anchors.fill: parent
            }

            ListView {
                id: m_list
                anchors.fill: parent
                highlightFollowsCurrentItem: false
                clip: true
                model: suggestions
                delegate: Item {
                    id: _d
                    property var mdForward: modelData
                    height: _ldr.height
                    width: parent.width

                    Loader {
                        id: _ldr
                        width: parent.width
                        property var modelData: _d.mdForward
                        property bool isCurrent: _d.ListView.isCurrentItem
                        sourceComponent: suggestionDelegate
                    }

                    MouseArea {
                        anchors.fill: _ldr
                        onPressed: parent.ListView.view.currentIndex = index
                        onClicked: acceptSuggestion(modelData)
                    }
                }

                highlight: Rectangle {
                    width: ListView.view ? ListView.view.currentItem.width : 0
                    height: ListView.view ? ListView.view.currentItem.height : 0
                    color: "#3679d7"
                    y: ListView.view ? ListView.view.currentItem.y : 0
                }
            }
        }
    }

    property Component suggestionDelegate: Component {
        Text {
            text: textProperty.length ? modelData[textProperty] : modelData
            color: isCurrent ? "#fff" : "#000"
            horizontalAlignment: Text.AlignLeft
            font: textField.font
            renderType: Text.NativeRendering
        }
    }


    property StateGroup states: StateGroup {
        states: [
            State {
                name: "popup"
                when: suggestionList !== null
                PropertyChanges {
                    target: root
                    onPrefixChanged: applyFilter()
                }
                PropertyChanges {
                    target: textField
                    Keys.forwardTo: _handler
                }
            },

            State {
                name: "active"
                when: textField.activeFocus
                PropertyChanges {
                    target: textField
                    Keys.forwardTo: _listener
                }
//                PropertyChanges {
//                    target: root
//                    onKeyPressed: {
//                        if ((event.key === suggestKey) && (event.modifiers & suggestKeyModifier)) {
//                            event.accepted = true
//                            popupList()
//                        } else
//                            event.accepted = false
//                    }
//                }
            }
        ]
    }

    function popupList() {
        suggestionList = suggestionListComponent.createObject(root.listParent, { currentIndex: 0 })
        listPopup()
        applyFilter()
    }

    function applyFilter() {
        selectedSuggestion = ""
        var slist = []
        completionModel.forEach(function(token) {
            var txt = textProperty.length ? token[textProperty] : token
            var pfx = prefix
            if (!caseSensitive) {
                txt = txt.toUpperCase()
                pfx = prefix.toUpperCase()
            }
            if (txt.indexOf(pfx) === 0)
                slist.push(token)
        })
        suggestions = slist
    }

    function acceptSuggestion(suggestion) {
        var currentText = textField.text
        var suggestionText  = textProperty.length ? suggestion[textProperty] : suggestion

        textField.text = currentText.substring(0,_handler.tokenStart) + suggestionText + currentText.substring(_handler.tokenEnd)
        textField.cursorPosition = _handler.tokenEnd
        suggestionList.destroy()
        suggestionList = null
        suggestionAccepted(suggestionText)
    }

    function extractToken() {
        var left = textField.getText(0, textField.cursorPosition)
        var right = textField.getText(textField.cursorPosition, textField.length)

        var rxHead= /^\w+/
        var rxTail = /\w+$/

        var tokenRest = right.match(rxHead)
        _handler.tokenEnd = textField.cursorPosition + (tokenRest ? tokenRest[0].length : 0)

        var ret = left.match(rxTail)
        _handler.tokenStart = textField.cursorPosition - (ret ? ret[0].length : 0)
        if (ret === null)
            return ""
        return ret[0]
    }

}

