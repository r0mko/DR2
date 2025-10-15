import QtQuick 2.3
import DataReactor.UI 1.0
import QtQuick.Layouts 1.1

Rectangle {
    id: root
    width: parent.width
    height: 20

    color: '#111111'

    UniLoader {
        id: ul
        target: propertyEditorElement
        value: modelData.value
        variant: modelData.inheritClassName + "." + modelData.name
        onHeightChanged: if( root.height < ul.height ) root.height = ul.height
    }
}
