import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1

Rectangle {
    id: root
    color: '#e0f1f1f1'
    width: 250
    height: 65
    border.color: '#e0d1d1d1'
    border.width: 1
    radius: 5
    ColumnLayout {
        Label {
            text: 'В источнике данных: "' + dataSource + '"'
        }
        Label {
            text: action == 'ADD' ? 'Новая запись' : action == 'UPD' ? 'Изменена запись' : action == 'DEL' ? 'Удалена запись' : ''
        }
        Label {
            text: 'Клиентским приложением №' + initiatorId
        }
    }
    MouseArea {
        anchors.fill: parent
        onClicked: noticeModel.remove(index)//console.log( index )
    }
}

