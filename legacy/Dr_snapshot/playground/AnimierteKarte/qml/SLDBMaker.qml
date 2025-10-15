import QtQuick 2.5
import Dr.MetaGraph 1.0
import Dr.Cartograph 1.0

ModelWeaver {
    id: i_ds
    property int pendingSize    ///< Количество создаваемых в данный момет делегатов
    onObjectAdded: pendingSize++
    
    SpatiaLiteDB {
        database: handle.data.url
        onReadyChanged: {
            var dh = handle.dataHandle
            console.log(dh)
            if(isOpen !== dh.isOpen)
                dh.isOpen = isOpen
            if(ready)
                i_ds.pendingSize--
            else
                i_ds.pendingSize++
        }
    }
    function getDB(handle) {
        if (!handle)
            return null
        return objectAt(handle.rowIndex)
    }
}
