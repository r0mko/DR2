import QtQuick 2.0
import QtQuick.Controls 1.1

QtObject {
    id: root
    property int maxSuggestions: 10
    property Item suggestionList: null
    property TextArea textArea
    property int suggestKey: Qt.Key_Space
    property int suggestKeyModifier: Qt.MetaModifier
    property var suggestions: []
    property var completionModel: []
    signal requestScopeUpdate
    signal suggestRequested(string prefix)
    signal suggestionAccepted(string suggestion)
    property string textProperty
    property string selectedSuggestion
    property string prefix: extractToken()

    signal keyPressed(var event)

    function extractToken() {
        var left = textArea.getText(0, textArea.cursorPosition)
        var right = textArea.getText(textArea.cursorPosition, textArea.length)

        var rxHead= /^\w+/
        var rxTail = /\w+$/

        var tokenRest = right.match(rxHead)
        _handler.tokenEnd = textArea.cursorPosition + (tokenRest ? tokenRest[0].length : 0)

        var ret = left.match(rxTail)
        _handler.tokenStart = textArea.cursorPosition - (ret ? ret[0].length : 0)
        if (ret === null)
            return ""
        return ret[0]
    }

    Component.onCompleted: { textArea.Keys.pressed.connect(keyPressed) }

    property Item __handler: Item {
        id: _handler

        property int tokenStart
        property int tokenEnd
//        onTokenStartChanged: console.log("start", tokenStart, textArea.getFormattedText(tokenStart, tokenEnd))
//        onTokenEndChanged: console.log("end", tokenEnd, textArea.getText(tokenStart, tokenEnd))
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
            y: textArea.cursorRectangle.y + textArea.cursorRectangle.height
            x: textArea.cursorRectangle.x
            Behavior on x { NumberAnimation { duration: 100; easing.type: Easing.OutCubic } }
            Behavior on y { NumberAnimation { duration: 100; easing.type: Easing.OutCubic } }
            frameVisible: true
            property alias view: m_list
            property alias currentIndex: m_list.currentIndex

            ListView {
                id: m_list
                anchors.fill: parent
                highlightFollowsCurrentItem: false
                clip: true
                model: suggestions
                delegate: suggestionDelegate
                Rectangle {
                    z: -1
                    anchors.fill: parent
                }
                highlight: Rectangle {
                    width: ListView.view ? ListView.view.currentItem.width : 0
                    height: ListView.view ? ListView.view.currentItem.height : 0
                    color: "#6420aaff"
                    y: ListView.view ? ListView.view.currentItem.y : 0
                }
            }
        }
    }

    property Component suggestionDelegate: Component {
        Text {
            text: modelData
            width: parent.width
            horizontalAlignment: Text.AlignLeft
            MouseArea {
                anchors.fill: parent
                onPressed: parent.ListView.view.currentIndex = index
                onClicked: acceptSuggestion(modelData)
            }
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
                    target: textArea
                    Keys.forwardTo: _handler
                }
            },

            State {
                name: "active"
                when: textArea.activeFocus
                PropertyChanges {
                    target: root
                    onKeyPressed: {
                        if ((event.key === suggestKey) && (event.modifiers & suggestKeyModifier)) {
                            event.accepted = true
                            suggestionList = suggestionListComponent.createObject(root.textArea, { currentIndex: 0 })
                            applyFilter()
                        } else
                            event.accepted = false
                    }
                }
            }
        ]
    }


    function applyFilter() {
        selectedSuggestion = ""
        var slist = []
        completionModel.forEach(function(token) {
            var txt = textProperty.length ? token[textProperty] : token
            if (txt.indexOf(prefix) === 0)
                slist.push(token)
        })
        suggestions = slist
    }


    function acceptSuggestion(suggestion) {
        textArea.remove(_handler.tokenStart, _handler.tokenEnd)
        textArea.insert(_handler.tokenStart, suggestion)
        suggestionList.destroy()
        suggestionList = null
        suggestionAccepted(suggestion)
    }
}

