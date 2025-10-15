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
import Playground 1.0

ApplicationWindow {
    id: mainWindow
    width: 1920
    height: 1000
    x: 0
    y: 0
    title: "Wiring Editor"

    Component.onCompleted: show()

    Window {
        flags: Qt.Tool
        title: "Напряжение контактной сети"
        width: 400
        height: 260
        visible: true
    }


}
