import QtQuick 2.4

QtObject {
    property Instantiator instantiator
    property int index
    property QtObject object
    property Connections __add: Connections {
        target: instantiator
        onObjectAdded: object = instantiator.objectAt(index)
        onObjectRemoved: object = instantiator.objectAt(index)
    }
    onIndexChanged: if (instantiator) { object = instantiator.objectAt(index) } else { object = null }
    onInstantiatorChanged: if (instantiator) { object = instantiator.objectAt(index) } else { object = null }
}

