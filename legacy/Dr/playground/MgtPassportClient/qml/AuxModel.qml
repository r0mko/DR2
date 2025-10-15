import QtQuick 2.4
import Dr.MetaGraph 1.0
import Dr.UI 1.0

GraphModel {
    id: aux
    property alias icons: _icons.namedChildren
    property alias style: _style.namedChildren
//    property url uiModel: "ui.mgf"
    property url uiModel: "file:///Users/Tox/Dropbox/programming/!Roman/Dr/playground/SpatialiteViewer/viewer/ui.mgf"

    property alias __icons: _icons
//    RootNodeFactory { }
    DataNodeFactory { id: fDataNode }
    NodeFactory { id: fNode }
    BlobFactory { id: fBlob }
    RootNodeFactory { }

    Component.onCompleted: {
        var ui = importFile(uiModel, null, "ui")
//        var ui =
        loadModel(uiModel)
        console.log("created aux model", ui.mrl)
    }

    ModelWatcher {
        id: _icons
        mrl: "/icons"
        onLinkedChanged: console.log("icons linked", linked)
    }

    ModelWatcher {
        id: _style
        mrl: "/style"
    }

}
