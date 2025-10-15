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

    property int runState: 0

    menuBar: MenuBar {
        id: menu
        Menu {
            title: qsTr("&File")

            MenuItem {
                id: openMenu
                text: "Choose program..."
                shortcut: "Ctrl+O"
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
            runState = 2
            updateStatusBar("Process started")
            frameBuffer.fb.update()
        }
        onFinished: {
            runState = 0
            updateStatusBar("Process finished")
        }
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
                text: "xterm"
            }
            Button {
                id: openButton
                width: 20
                text: "..."
                onClicked: openDialog.open()
            }
            Button {
                id: runButton
                text: "Run"
                onClicked: {
                    if (!runState) {
                        runState = 1
                        processRunner.runProgram(programName.text)
                    } else {
                        processRunner.stopProgram();
                    }
                }
            }
            Item { Layout.fillWidth: true }
        }

        states: [
            State {
                name: "clickToRun"
                when: ( runState == 1 )
                PropertyChanges { target: runButton; text: "Stop" }
                PropertyChanges { target: openButton; enabled: false }
                PropertyChanges { target: programName; enabled: false }
                PropertyChanges { target: openMenu; enabled: false }
            },
            State {
                name: "running"
                when: ( runState == 2 )
                PropertyChanges { target: runButton; text: "Stop" }
                PropertyChanges { target: openButton; enabled: false }
                PropertyChanges { target: programName; enabled: false }
                PropertyChanges { target: openMenu; enabled: false }
            }
        ]
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
