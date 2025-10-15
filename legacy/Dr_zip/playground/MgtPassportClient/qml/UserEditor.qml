import QtQuick 2.5
import QtQuick.Controls 1.4

Tab {
    id: root
    title: 'Управление пользователями'
    Table{
        anchors.fill: parent
        datasource: 'users'
    }
}
