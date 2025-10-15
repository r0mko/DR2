import QtQuick 2.3
import QtQuick.Controls 1.2
import DataReactor.UI 1.0

ToolBase {
    id: nodeEditor
    action: Action {
        checkable: true
        onCheckedChanged: console.log("state", state.name, checked)
    }

    toolbarControl: ThinToolButton {
        onClicked: action.checked ^= true
        checked: action.checked
        width: 36
        Symbol {
            anchors.fill: parent
            font: Symbol.FontAwesome
            symbol: FontAwesome.DotCircleO
            pixelSize: 20
        }
    }

    Component {
        id: handlerComp
        MouseHandler {
            onClicked: datamodel.nodes.addNode(mouse.x, mouse.y)
            onPositionChanged: console.log("hijacked mouse", mouse.x, mouse.y)
        }
    }

    states: [
        State {
            name: "editnodes"
            when: action.checked

            PropertyChanges {
                target: editor
                mouse.cursorShape: Qt.CrossCursor
                mouse.acceptedButtons: Qt.LeftButton
//                mouseHandler: handlerComp
//                mouse.onReleased: console.log("Released")

            }

            PropertyChanges {
                target: nodesView
                nodeRadius: 10
                nodeSize: 20
                clickNode: function(index) { nodesList.currentIndex = index }
                cursorShape: Qt.OpenHandCursor
                opacity: 1
            }

            PropertyChanges {
                target: nodesList
                currentIndex: -1
            }
        }

    ]


}
