import QtQuick 2.5
import Dr.Cartograph 1.0
import Dr.MetaGraph 1.0
import QtQml.Models 2.2

Instantiator {
    id: root
    property QtObject parentObject
    property int depth: 0
    property string childrenRole
    property list<Component> delegates
    delegate: Loader {
        sourceComponent: delegates[depth]
        property DelegateModel childModel: DelegateModel {
            model: root.model
//            rootIndex:
        }
    }
}


