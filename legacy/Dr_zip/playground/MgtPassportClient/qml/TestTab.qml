import QtQuick 2.5
import QtQuick.Controls 1.4

Tab {
    id: root
    title: 'Test Tab'
    Table {
        anchors.fill: parent
        datasource: 'test'
    }
}
