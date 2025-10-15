import QtQuick 2.5
import Dr.Cartograph 1.0
import Dr.MetaGraph 1.0
import QtQml.Models 2.2

Loader {
    id: root
    property QtObject parentObject: item
    property int pDepth: depth
    property string pChildrenRole: childrenRole
    property list<Component> pDelegates
    property Component __veaver:
        Component {
        WeaverDelegate {
        active: root.status = Loader.Ready
//        model: modelData[childrenRole]
//        delegate: (pDepth + 1) < pDelegates.length ? pDelegates[pDepth + 1] : null
//        depth: pDepth + 1
//        delegates: pDelegates
//        childrenRole: pChildrenRole
    }
    }

}
