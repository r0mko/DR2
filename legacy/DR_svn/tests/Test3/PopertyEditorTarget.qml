import QtQuick 2.3
import DataReactor.UI 1.0

Target {
    id: propertyEditor
    targetName: 'propertyEditor'
    Delegate{
        component: PropertyEditorDelegate{}
        typeName: 'QObject*'
    }
}
