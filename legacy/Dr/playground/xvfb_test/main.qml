import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import Dr.Xvfb 1.0


ApplicationWindow {
    id: root
    title: qsTr("XvFB test")
    width: 1280
    height: 768
    visible: true

    menuBar: MenuBar {
        id: menu
        Menu {
            title: qsTr("&File")

            MenuItem {
                text: "Run program..."
                shortcut: "Ctrl+R"
                onTriggered: openDialog.open()
            }
            MenuItem {
                text: qsTr("E&xit")
                onTriggered: Qt.quit();
            }
        }
    }

    FileDialog {
        id: openDialog
        nameFilters: [ "Executable files (*)" ]
        onAccepted: {
            programName.text = openDialog.fileUrl
        }

    }

    statusBar: StatusBar {
        RowLayout {
            Label { id : statusLabel }
        }
    }

    Connections {
        target: processRunner
        onError: updateStatusBar("Error: " + error)
        onStarted: {
            updateStatusBar("Process started")
            frameBuffer.fb.update()
        }
        onFinished: updateStatusBar("Process finished")
    }

    function updateStatusBar(param) {
        statusLabel.text = param
    }

    toolBar: ToolBar {
        RowLayout {
            id: program
            anchors.fill: parent
            TextField {
                id: programName
                Layout.minimumWidth: 200
                placeholderText: "program to run"
                text: "/opt/X11/bin/xterm"
            }
            Button {
                width: 20
                text: "..."
                onClicked: openDialog.open()
            }
            Button {
                text: "Run"
                onClicked: processRunner.runProgram(programName.text)
            }
            Item { Layout.fillWidth: true }
        }
    }

    Xvfb {
        id: frameBuffer
        anchors.fill: parent
//        anchors.top: parent.toolBar.bottom
//        anchors.left: parent.left
//        anchors.right: parent.right
//        anchors.bottom: parent.statusBar.top
    }
}
