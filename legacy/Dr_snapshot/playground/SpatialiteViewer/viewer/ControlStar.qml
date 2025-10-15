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

    Xvfb {
        Component.onCompleted: {
            processRunner.runXGS("192.168.2.11", 16);
            //processRunner.runProgram("/usr/bin/xterm");
        }

        id: frameBuffer
        anchors.fill: parent
    }
}
