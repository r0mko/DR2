import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import DataReactor.UI 1.0

Rectangle {
    id: root
    property alias mouse: mouseArea
    property alias cursorShape: mouseArea.cursorShape // TODO: remove
    property alias hudPaint: hud.paint // TODO: remove
    property alias hud: hud
    property ThinToolbar mainToolBar
    property list<QtObject> tools
    property alias mouseHandler: _hijack.sourceComponent

    onStateChanged: console.log("Editor state", state)

    Component.onCompleted: initTools()

    function initTools() {
        var stlist = []
        var toolbtns = []
        for (var i = 0; i < tools.length; ++i) {
            var s = tools[i].states
//            s.when = Qt.binding(function() { return tools[i].action.checked } )
            for (var j = 0; j < s.length; ++j) {
                stlist.push(tools[i].states[j])
                console.log("added state", tools[i].states[j].name)
            }
            toolbtns.push(tools[i].toolbarControl)
            tools[i].editor = root
        }
        states = stlist
        mainToolBar.buttons = toolbtns
    }


    MouseArea {
        id: mouseArea
        anchors.fill: parent
        Loader {
            id: _hijack
        }
    }

    Canvas {
        id: hud
        anchors.fill: parent
        renderStrategy: Canvas.Cooperative
        renderTarget: Canvas.FramebufferObject
        property var paint: function(ctx) {}
        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
            paint(ctx)
        }
    }
}
