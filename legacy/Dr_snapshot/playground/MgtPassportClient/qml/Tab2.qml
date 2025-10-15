import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import Test 1.0

Tab {
    id: root
    title: 'Паспорта участков'
    ColumnLayout {
        anchors.fill: parent
        Table {
            id: view
            datasource: 'passport'
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}

