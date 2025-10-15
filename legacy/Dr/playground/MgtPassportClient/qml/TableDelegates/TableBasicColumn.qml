import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

TableViewColumn {
    id: root

//    onWidthChanged: console.log('==>>', 'width changed', width)
    /*column common*/
    role: modelData.name
    property string type: modelData.tableDelegate === undefined ? '' : modelData.tableDelegate
    title: modelData.caption
    visible: !modelData.hidden
    movable: true
    property var columnData: modelData
    /**/
    /*column specific*/
    property int margin: 5
    property int fontSize: table.__tableScale
    property string fontFamily: 'Helvetica'
    elideMode: Text.ElideMiddle
    horizontalAlignment: Text.AlignLeft
    /**/
    delegate: TableBasicColumnDelegate {}
}
