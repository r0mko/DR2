import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.2
import Dr.T2C 1.0

Window {
    title: qsTr("T2C test")
    width: 1280
    height: 768
    visible: true
    T2CManager {
        id: manager
        host: '192.168.2.11'
        port: 47001
    }
    ColumnLayout {
        Button {
            text: 'subscribe'
            onClicked: manager.subscribe(1190554)
        }
        Button {
            text: 'unsubscribe'
            onClicked: manager.unsubscribe(1190554)
        }
        Button {
            text: 'getName'
            onClicked: console.log( manager.getName(1190554) )
        }
    }
}
