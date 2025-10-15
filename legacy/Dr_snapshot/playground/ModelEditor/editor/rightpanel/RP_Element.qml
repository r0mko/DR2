import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
//import Dr.MetaGraph 1.0
import Dr.UI 1.0
RP_Node {
    id: root

    property var agent: objectData.element$i

    CaptionLabel {
        Layout.fillWidth: true
        text: 'Element data'// + objectData.typeName
        level: 3
    }


    JSObjectView {
        id: propTable
        Layout.fillWidth: true
        Repeater {
//            model: objectData.handle.properties
            model: Graph.extractProperties(root.agent, false)
            onModelChanged:  console.log( "length", Graph.extractProperties(root.agent, false).length )
            JSObjectViewRow {
                text: modelData.name
                RowLayout {
                    Label {
                        Layout.leftMargin: 5
                        Layout.minimumWidth: 100
                        text: modelData.typeName
                        font.pixelSize: 10
                    }
                    Loader {
                        property var md: modelData

                        sourceComponent: delegates[modelData.typeName] === undefined ? labelDelegate : delegates[modelData.typeName]
                    }

                    Item { Layout.fillWidth: true }
                }
            }
        }
    }
    property var delegates: {
        'QString':  stringDelegate,
        'int':      intDelegate,
        'float':    floatDelegate,
        'double':   floatDelegate,
        'QVector3D':vector3DDelegate
    }

    Component {
        id: labelDelegate
        Label { text: root.agent[md.name] }
    }
    Component {
        id: stringDelegate
        TextField {
            text: root.agent[md.name]
            onAccepted: {
                root.agent[md.name] = text
                focus = false
            }
        }
    }
    Component {
        id: intDelegate
        SpinBox {
            value: root.agent[md.name]
            onValueChanged: root.agent[md.name] = value
            onEditingFinished: focus = false
            minimumValue: -2147483648
            maximumValue: 2147483647
        }
    }
    Component {
        id: floatDelegate
        SpinBox {
            value: root.agent[md.name]
            decimals: 3
            onValueChanged: root.agent[md.name] = value
            onEditingFinished: focus = false
            minimumValue: -1e127
            maximumValue: 1e127
        }
    }
    Component {
        id: vector3DDelegate
        RowLayout {
            SpinBox {
                prefix: 'x: '
                value: root.agent[md.name].x
                decimals: 3
                onValueChanged: root.agent[md.name].x = value
                onEditingFinished: focus = false
                minimumValue: -1e127
                maximumValue: 1e127
            }
            SpinBox {
                prefix: 'y: '
                value: root.agent[md.name].y
                decimals: 3
                onValueChanged: root.agent[md.name].y = value
                onEditingFinished: focus = false
                minimumValue: -1e127
                maximumValue: 1e127
            }
            SpinBox {
                prefix: 'z: '
                value: root.agent[md.name].z
                decimals: 3
                onValueChanged: root.agent[md.name].z = value
                onEditingFinished: focus = false
                minimumValue: -1e127
                maximumValue: 1e127
            }
        }
    }

}

