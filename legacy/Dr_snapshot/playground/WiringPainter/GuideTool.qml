import QtQuick 2.3
import Dr.Graphics 1.0
QtObject {
    id: root
    property Item handle
    property SceneItem visual
    property Groove guide
    property real position
    property var resources: []
    default property alias children: root.resources
}

