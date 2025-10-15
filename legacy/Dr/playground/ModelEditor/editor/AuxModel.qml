import Dr.MetaGraph 1.0
import QtQuick 2.3
GraphModel {
    id: aux
    property alias icons: icons.namedChildren
    property alias delegates: delegates.namedChildren
//    property alias icons: iconModel
    NodeFactory { }
    BlobFactory { }
    RootNodeFactory { }
    DataNodeFactory { }
    function init() {
        clear()
        var ui = importFile("ui.mgf", null)
    }

    ModelWatcher {
        id: icons
        mrl: "/ui/icons"
        onLinkedChanged: console.log("icons:", linked)
    }

    ModelWatcher {
        id: delegates
        mrl: "/ui/delegates"
        onLinkedChanged: console.log("delegates:", linked)
    }

    Component.onCompleted: init()
}
