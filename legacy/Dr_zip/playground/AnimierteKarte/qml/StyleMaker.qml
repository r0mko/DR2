import QtQuick 2.5
import Dr.UI 1.0
import Dr.MetaGraph 1.0
import Dr.Cartograph 1.0

Weaver {
    id: i_styles
    onObjectAdded: if (parent) parent.insertChild(object, index)
    onObjectRemoved: object.deleted = true
    onValueChanged: {
        var handle = main_model.getHandle(index)
        var l
        if( handle.type != 'Style' )
            l = i_layers.objectAt(handle.parent.inNodes()[0].parent.rowIndex)
        else
            l = i_layers.objectAt(handle.inNodes()[0].parent.rowIndex)
        l.update()
    }
}
