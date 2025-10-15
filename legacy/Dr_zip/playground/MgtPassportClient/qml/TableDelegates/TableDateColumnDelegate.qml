import QtQuick 2.5

TableBasicColumnDelegate {
    text: styleData.value === undefined ? '' : Qt.formatDate( styleData.value, 'dd.MM.yyyy' )
}

