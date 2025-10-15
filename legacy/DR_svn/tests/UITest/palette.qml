import QtQuick 2.3
import QtQuick.Window 2.2
import DataReactor.UI 1.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2
import "Delegates/formelement" as Formelement
import "Delegates"
Window {
    id: palettewindow
    visible: true
    color: "#333"
    width: 500
    height: 300
//    property var pal: Theme.palette.lightSeq

    property var pal: Theme.palette.midSeq
    property alias luminanceMode: lumMode.checked
    Component.onCompleted: {
        paletteModel.clear()
        pal.forEach(function(i) {
            paletteModel.append({ colorValue: i, hueLocked: false, satLocked: false, lightnessLocked: false,  });
        })
        palette.currentIndex = -1
        //        paletteGrid.model = paletteModel
    }


    onClosing: {
        console.log("closing")
        var p = []
        for (var i = 0; i < palette.children.length; ++i) {
            if (palette.children[i].color)
                p.push(palette.children[i].color)
        }
        console.log(p)
    }

    ColumnLayout {
        id: layout
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10
        Column {
            anchors.left: parent.left
            anchors.right: parent.right
            RowLayout {
                anchors.left: parent.left
                anchors.right: parent.right
                Label {
                    width: palette.labelWidth
                    color: "white"
                    text: "S"
                }
                Slider {
                    id: sat_all
                    Layout.fillWidth: true
                    value: 1.0
                }
            }
            RowLayout {
                anchors.left: parent.left
                anchors.right: parent.right
                Label {
                    width: palette.labelWidth
                    color: "white"
                    text: "L"
                }
                Slider {
                    id: lum_all
                    Layout.fillWidth: true
                    value: 0.5
                }
            }
            RowLayout {
                anchors.left: parent.left
                anchors.right: parent.right
                Label {
                    width: palette.labelWidth
                    color: "white"
                    text: "A"
                }
                Slider {
                    id: alpha_all
                    Layout.fillWidth: true
                    value: 1.0
                }
            }
        }

        CheckBox {
            id: lumMode
            text: "luminance mode"
            style: CheckBoxStyle {
                label: Text {
                    color: "white"
                    text: control.text

                }
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            GridView {
                id: palette
                property int labelWidth: 20
                anchors.fill: parent
                cellHeight: 70
                cellWidth: 70

                model: paletteModel
                delegate: cellComp
                highlight: highlightComp
                highlightFollowsCurrentItem: false
                focus: true
                onCurrentItemChanged: {
                    console.log(currentIndex)
                    var apos = palettewindow.contentItem.mapFromItem(palette, currentItem.x, currentItem.y, currentItem.width, currentItem.height)
                    toolwin.xPos = apos.x + apos.width / 2 + palettewindow.x
                    toolwin.yPos = apos.y + palettewindow.y
                    toolwin.content = currentItem.editor
                    toolwin.visible = true
                }
            }
        }

        PopupWindow {
            id: toolwin
            visible: false
        }

        Component {
            id: highlightComp
            Rectangle {
                property int margin: 4
                width: palette.currentItem.width + margin * 2
                height: palette.currentItem.width + margin * 2
                radius: 14
                border.width: 2
                border.color: "#d01c86ff"
                color: "transparent"
                x: palette.currentItem.x - margin
                y: palette.currentItem.y - margin
            }
        }

        Component {
            id: cellComp
            ColorCell {
                color: colorValue
                allSat: sat_all.value
                allLum: lum_all.value

                border.color: GridView.isCurrentItem ? "#80ffffff" : "#80000000"


            }
        }



        //        Button {
        //            width: parent.width
        //            height: 40
        //            text: "new..."
        //            onClicked:  {
        //                var p = cellComp.createObject(palette)
        //            }
        //        }
    }
    ListModel {
        id: paletteModel
        ListElement {
            colorValue: "#000"
            hueLocked: false
            satLocked: false
            lightnessLocked: false
        }
    }



    //    Window {
    //        id: listWin
    //        width: 500
    //        height: 400
    //        color: "#333"
    //        flags: Qt.Tool
    //        visible: true





    //        //        x: window.x + window.width
    //        //        y: window.y
    ////        TableView {
    ////            id: table
    ////            rowDelegate: Item {
    ////                Rectangle {
    ////                    anchors.fill: parent
    ////                    visible: styleData.selected
    ////                    color: "#A0305080"
    ////                }
    ////                Rectangle {
    ////                    anchors.fill: parent
    ////                    opacity: modelData.inheritLevel / 5
    ////                    color: "#888"
    ////                }
    ////            }

    ////            anchors.fill: parent
    //////            anchors.margins: 5
    ////            model: info.properties
    ////            alternatingRowColors: false
    ////            backgroundVisible: false
    ////            TableViewColumn {
    ////                id: label
    ////                role: "name"
    ////                title: "Property"
    ////                width: 120
    ////            }
    ////            TableViewColumn {
    ////                id: typename
    ////                role: "typeName"
    ////                title: "Type"
    ////                width: 120
    ////            }
    ////            TableViewColumn {
    ////                role: "inheritLevel"
    ////                title: "Type"
    ////                width: 120
    ////            }
    ////            TableViewColumn {
    ////                role: "value"
    ////                title: "Value"
    ////                width: table.viewport.width - (label.width + typename.width)
    ////                //                width: table.viewport.width - label.width
    ////                delegate: UniDelegate {
    ////                    target: "formelement"
    ////                    value: modelData.value
    ////                    anchors.fill: parent
    ////                    anchors.margins: 3
    ////                }
    ////            }
    ////        }

    ////        ListView {
    ////            id: list
    ////            anchors.fill: parent
    ////            model: info.properties
    //////            delegate: MetaPropertyDelegate {
    //////                label: name
    //////                val: value
    //////            }
    ////            delegate: UniDelegate {
    ////                target: "qobjectinfo"
    ////                value: modelData
    //////                onAttachedChanged: console.log(Object.keys(attached))
    ////            }
    ////        }
    //        Component.onCompleted: listWin.show()


    //    }

    //    MetaObject {
    //        id: info
    //        object: rect
    //    }

    Target {
        targetName: "qobjectinfo"
        Delegate {
            component: MetaPropertyDelegate{}
            typeName: "MetaProperty"
        }
    }
    Target {
        targetName: "formelement"
        Delegate {
            component: Formelement.Integer{}
            typeName: "int"
        }
        Delegate {
            component: Formelement.Integer{}
            typeName: "uint"
        }
        Delegate {
            component: Formelement.Integer{}
            typeName: "quint64"
        }
        Delegate {
            component: Formelement.Integer{}
            typeName: "qint64"
        }
        Delegate {
            component: Formelement.Float{}
            typeName: "qreal"
        }
        Delegate {
            component: Formelement.Float{}
            typeName: "float"
        }
        Delegate {
            component: Formelement.Boolean{}
            typeName: "bool"
        }
        Delegate {
            component: Formelement.QString{}
            typeName: "QString"
        }
        Delegate {
            component: Formelement.QColor{}
            typeName: "QColor"
        }
        Delegate {
            component: Formelement.ListInfo{}
            typeName: "QQmlListProperty<QObject>"
        }
        Delegate {
            component: Formelement.ListInfo{}
            typeName: "QQmlListProperty<QQuickItem>"
        }
        Delegate {
            component: Formelement.ListInfo{}
            typeName: "QQmlListProperty<QQuickTransform>"
        }
    }

}
