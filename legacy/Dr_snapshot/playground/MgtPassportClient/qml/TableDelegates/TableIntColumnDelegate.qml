import QtQuick 2.5

TableBasicColumnDelegate {
    text: Number( styleData.value ) === 0 ? '' : Number( styleData.value )
}

