import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.2
import Dr.T2C 1.1

Window {
    title: qsTr("T2C test")
    width: 1280
    height: 768
    visible: true

    T2CManager {
        id: manager
        host: '192.168.1.78'
        port: 47001
//        Component.onCompleted: start()
        onGotMessage: {
            console.log( message );
        }
    }
    ColumnLayout {
        Button {
            text: 'start'
            onClicked: manager.start()
        }
        Button {
            text: 'subscribe'
            onClicked: {
                [ 131689,
                143576,
                144155,
                144173,
                144191,
                143880,
                143904,
                143928,
                143940,
                143952,
                1438814].forEach(function(e){ manager.subscribe(e) })

            }
        }
    }
}
