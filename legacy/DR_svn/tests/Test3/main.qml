import QtQuick 2.0
import QtQml.Models 2.1
import QtQuick.Window 2.2
import DataReactor.UI 1.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2

Window {
    id: mainWindow
    width: 1500
    height: 800
    title: 'Window title'
    readonly property bool test: true
    property bool test2: true
    property var w: mainWindow
    Component.onCompleted: show()
    color: 'green'

    Exo2Font { id: main_font }
    Target {
        id: propertyEditor
        targetName: 'propertyEditor'
        Delegate {
            component: PropertyEditorDelegate2{}
            typeName: 'QObject*'
        }

    }
    Target {
        id: listView
        targetName: 'listView'
//        Delegate {
//            component: ListViewDelegate{}
//            typeName: 'QObject*'
//        }
        Delegate {
            component: ListViewDelegate{}
            typeName: 'QVariantList'
        }
        Delegate {
            component: Rectangle{ width: 100; height: 100 }// PropertyEditorDelegate{}
            typeName: 'QQmlListProperty<?>'
        }
    }
    Target {
        id: propertyEditorElement
        targetName: 'propertyEditorElement'
        Delegate {
            component: PropertyDelegateInt{}
            typeName: 'int'
        }
        Delegate {
            component: PropertyDelegateDouble{}
            typeName: 'double'
        }
        Delegate {
            component: PropertyDelegateSlider{}
            typeName: 'double'
            variantName: 'QQuickItem.rotation'
            extraData: { 'minValue': -180, 'maxValue': 180, 'stepSize': 0.1 }
        }
        Delegate {
            component: PropertyDelegateSlider{}
            typeName: 'double'
            variantName: 'QQuickItem.scale'
            extraData: { 'minValue': 0, 'maxValue': 10, 'stepSize': 0.01 }
        }
        Delegate {
            component: PropertyDelegateSlider{}
            typeName: 'double'
            variantNames: ['QQuickItem.opacity', 'QWindow.opacity']
            extraData: { 'minValue': 0, 'maxValue': 1, 'stepSize': 0.01 }
        }
        Delegate {
            component: PropertyDelegateDouble{}
            typeName: 'double'
            variantNames: [ 'QQuickItem.x', 'QQuickItem.y', 'QQuickItem.z']
            extraData: { 'minValue': -99999, 'maxValue': 99999, 'stepSize': 1 }
        }
        Delegate {
            component: PropertyDelegateDouble{}
            typeName: 'double'
            variantNames: [ 'QQuickItem.width', 'QQuickItem.height']
            extraData: { 'minValue': 0, 'maxValue': 99999, 'stepSize': 1 }
        }

        Delegate {
            component: PropertyDelegateDouble{}
            typeName: 'int'
            variantNames: ['QWindow.x', 'QWindow.y']
            extraData: { 'minValue': -99999, 'maxValue': 99999, 'stepSize': 1 }
        }

        Delegate {
            component: PropertyDelegateDouble{}
            typeName: 'int'
            variantNames: ['QWindow.width', 'QWindow.height']
            extraData: { 'minValue': 0, 'maxValue': 99999, 'stepSize': 1 }
        }


        Delegate {
            component: PropertyDelegateString{}
            typeName: 'QString'
        }
        Delegate {
            component: PropertyDelegateBool{}
            typeName: 'bool'
        }
        Delegate {
            component: PropertyDelegateColor{}
            typeName: 'QColor'
        }
        Delegate {
            component: PropertyDelegateCommon{}
            typeName: 'QPointF'
        }
        Delegate {
            component: PropertyDelegateCommon{}
            typeName: 'QRectF'
        }
        Delegate {
            component: PropertyDelegateFont{}
            typeName: 'QFont'
        }
        Delegate {
            component: PropertyDelegateString{}
            typeName: 'QUrl'
        }
        Delegate {
            component: PropertyDelegateInvalid{}
            typeName: 'QQuickAnchorLine'
        }
        Delegate {
            component: PropertyDelegateInvalid{}
            typeName: 'QQuickGradient*'
        }
        Delegate {
            component: PropertyDelegateObject{}
            typeName: 'QObject*'
        }
        Delegate {
            component: PropertyDelegateList{}
            typeName: 'QQmlListProperty<?>'
        }
        Delegate {
            component: PropertyDelegateVariantList{}
            typeName: 'QVariantList'
        }
        Delegate {
            component: PropertyDelegateInvalid{}
            typeName: 'Invalid'
        }
    }

    SplitView {
        antialiasing: true
        anchors.fill: parent
        Rectangle{
            width: 501
            color: '#222'
            antialiasing: true
            property var window: mainWindow
            Text{
                id: item
                anchors.fill: parent

                property var someList: [ "foo", true, item, {"the": "property"} ]
                font.family: main_font.regular.name
                text: 'Quick brown fox text, алсо, эй, жлоб! Где туз? Прячь юных съёмщиц в шкаф.'
                color: Theme.palette.lightSeq[ 4 ]
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
//                onWidthChanged: visible = true
                states: [
                    State {
                        name: "thestate1"
                        PropertyChanges {
                            target: item
                            color: Theme.palette.lightSeq[1]
                        }
                    },
                    State {
                        name: "thestate2"
                        PropertyChanges {
                            target: item
                            color: Theme.palette.lightSeq[2]
                        }
                    },
                    State {
                        name: "thestate3"
                        PropertyChanges {
                            target: item
                            color: Theme.palette.lightSeq[3]
                        }
                    },
                    State {
                        name: "thestate4"
                        PropertyChanges {
                            target: item
                            color: Theme.palette.lightSeq[5]
                        }
                    },
                    State {
                        name: "thestate5"
                        PropertyChanges {
                            target: item
                            color: Theme.palette.lightSeq[6]
                        }
                    }
                ]
            }
        }

        Rectangle{
            color: '#333'
            antialiasing: true
            UniLoader {
                id: uload
                antialiasing: true
                anchors.fill: parent
                value: item
                target: propertyEditor
            }
        }
    }

}
