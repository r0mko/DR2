import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

TableViewColumn {
    id: root
    /*column common*/
    role: modelData.name
    title: modelData.caption
    visible: !modelData.hidden
    movable: true
    property var columnData: modelData
    /**/
    /*column specific*/
    property int margin: 5
//    property int fontSize: uiStyle.fontSizeMed * 0.7
//    property string fontFamily: 'Helvetica'
//    elideMode: Text.ElideMiddle
//    horizontalAlignment: Text.AlignLeft
    /**/
    delegate: TableBoolColumnDelegate {}
}
