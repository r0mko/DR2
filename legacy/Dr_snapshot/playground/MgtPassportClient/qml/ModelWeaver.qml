import QtQuick 2.4

Instantiator {
    id: root
    property bool ready: false

    onObjectAdded: ready = model && (model.count === count)
    onObjectRemoved: ready = model && (model.count === count)
    onModelChanged: ready = false

    property var dependentModel: null
    Binding on dependentModel {
        when: ready
        value: root.model
    }
}

