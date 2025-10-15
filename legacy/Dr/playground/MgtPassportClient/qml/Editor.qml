import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import Test 1.0
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

Tab {
    id: root
    title: 'Участки КС'

    ColumnLayout {
        anchors.fill: parent
        SplitView {
            anchors.fill: parent
            Rectangle{
                width: 300
                color: 'transparent'
                Table {
                    anchors.fill: parent
                    datasource: 'cn_area'
                }
            }
            Rectangle{
                color: 'transparent'
                Table {
                    anchors.fill: parent
                    datasource: 'cn_site'
                }
            }
        }
    }
}

