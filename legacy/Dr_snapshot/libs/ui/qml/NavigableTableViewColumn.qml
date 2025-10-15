import QtQuick.Controls 1.3

TableViewColumn {

    function findDelegateItem(row, column) {
        var minWidth = 0
        var listdata = __view.__listView.children[0]
        var item = listdata.children[row] ? listdata.children[row].rowItem : undefined
        if (item) { // FocusScope { id: rowitem }
            item = item.children[1]
            if (item) { // Row { id: itemrow }
                item = item.children[column]
                if (item) { // Repeater.delegate a.k.a. __view.__itemDelegateLoader
                    return item.item
                }
            }
        }
    }
}


