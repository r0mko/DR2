import QtQuick 2.3
import QtQuick.Controls 1.2
import DataReactor.UI 1.0
import DataReactor 1.0

StateGroup {
    id: _b
    property Action action
    property ThinToolButton toolbarControl
//    State state
    property Drawboard editor: null
    property Component mouseHandler: null

    onEditorChanged: {
        state.when = Qt.binding(function() { return action.checked })
    }
    property QtObject datamodel: null
    default property alias data: _b._d

    property list<QtObject> _d

}
