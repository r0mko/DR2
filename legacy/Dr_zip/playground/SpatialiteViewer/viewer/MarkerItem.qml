import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import Dr.Graphics 1.0
import Dr.UI 1.0
import QtGraphicalEffects 1.0
import Dr.Cartograph 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.2
import Dr.MetaGraph 1.0
import QtQuick.Window 2.2
Rectangle {
    id: hover_handle
    property var runner
    property alias text: label1.text
    visible: false
    width: 16
    height: 16
    border.width: 2
    radius: 2
    color: "transparent"
    border.color: "#ff0030"
    transform: Translate {
        x: -hover_handle.width / 2
        y: -hover_handle.height / 2
    }
    scale: 1 / drawarea.scene.scale
    Text {
        id: label1
        x: 0
        y: 0
        transformOrigin: Item.TopLeft
        rotation: -hover_handle.rotation
        color: "#fff"
        style: Text.Outline
        styleColor: "#000"
    }
    
    function updateHandle(text) {
        x = runner.point.x
        y = runner.point.y
        rotation = runner.angle
        label1.text = "%1\npos: %2 ang: %3".arg(text).arg(runner.position).arg(runner.angle)
        //                        label1.text = runner.currentUserData.name
    }
}
