import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.3
import Dr.Cartograph 1.0
import Dr.MetaGraph 1.0
import Dr.UI 1.0

Window {
    id: dlg_settings
    modality: Qt.NonModal
    width: 400
    height: 300
    property ModelWatcher config
    ColumnLayout {
        anchors.fill: parent

        JSObjectView {
            Layout.fillHeight: true
            Layout.fillWidth: true

            id: objview
            object: config.handle.agent.datanode$data

        }
    }

}
