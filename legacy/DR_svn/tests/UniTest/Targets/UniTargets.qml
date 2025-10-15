pragma Singleton
import QtQuick 2.3
import DataReactor.UI 1.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Window 2.2

Object {
    id: targets
    property Target propertyEditor: PropertyEditor {}
    property Target propertyEditorElement: PropertyEditorElement {}

}
