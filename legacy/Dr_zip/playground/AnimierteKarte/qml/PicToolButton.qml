import QtQuick 2.5
import Dr.UI 1.0

VectorIconButton {
    width: 30
    height: 28
    property string iconName
    flat: true
    iconColor: "#444"
    hoverColor: "#4080eeff"
    iconData: aux.icons[iconName].data
}
