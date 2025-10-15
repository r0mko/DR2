import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import Dr.MetaGraph 1.0
//import QtQuick.Extras 1.4
import Dr.UI 1.0
import "../treeview"

ColumnLayout {
    id: root
    anchors.fill: parent
    spacing: 1
    property bool dataModified: false
//    property var inEdges: objectData.inEdges()
//    property var outEdges: objectData.outEdges()
        property var inEdges: objectData.handle.inEdges()
        property var outEdges: objectData.handle.outEdges()
    property var handle: objectData.handle
    property var properties: []

    onHandleChanged: propTable.model = propTable.baseproperties.concat(properties)
    /* basic node info */
    states: [
        State {
            name: "dataicon"
            when: objectData.typeName === f_DataNode.nodeTypeName && handle["data"].hasOwnProperty("type") && handle["data"]["type"] === "picture" && handle["data"].hasOwnProperty("blob")
            PropertyChanges {
                target: iconloader
                blob: handle.data.blob
                active: true
            }
        },
        State {
            name: "blobicon"
            when: objectData.typeName === f_Blob.nodeTypeName && handle.type === "picture"
            PropertyChanges {
                target: iconloader
                blob: objectData.blob$data
                active: true
                adjustX: objectData.blob$metadata.adjustX ? objectData.blob$metadata.adjustX : 0
                adjustY: objectData.blob$metadata.adjustY ? objectData.blob$metadata.adjustY : 0
                //                adjustX: !!handle.metadata["adjustX"] ? handle.metadata.adjustX : 0
                //                adjustY: !!handle.metadata["adjustY"] ? handle.metadata.adjustY : 0
            }
        }
    ]
    Rectangle {
        height: 32
        clip: true
        Layout.fillWidth: true
        color: "#aacaec"
        RowLayout {
            anchors.fill: parent
            anchors.margins: 4
            spacing: 0
            NodeIcon {
                Layout.fillHeight: true
                Layout.minimumWidth: height
                nodeType: objectData.typeName
                iconColor: aux.delegates[nodeType].data.iconColor
                textColor: aux.delegates[nodeType].data.iconTextColor
            }

//            Label {
//                font.pixelSize: 20
//                anchors.verticalCenter: parent.verticalCenter
//                font.family: "Helvetica Neue"
//                font.weight: Font.Light
//                text: objectData.nodeId
//            }


            EditableText {
                id: node_id
                Layout.fillWidth: true
                editable: true
                font.pixelSize: 20
                font.family: "Helvetica Neue"
                font.weight: Font.Light
                delayEdit: false
                value: objectData.nodeId
                editorComponent: TextFieldEditor {
                    text: objectData.nodeId
                    onValueChanged: handle.nodeId = value
                }
            }
            Item { Layout.fillWidth: true }
        }
    }

    RowLayout {
        Layout.fillWidth: true
        spacing: 1
        JSObjectView {
            id: propTable
            property var baseproperties: [
                { "Тип": objectData.typeName },
                { "Адрес": objectData.address.join(".") },
                { "MRL": objectData.mrl }
            ]
            Layout.fillWidth: true
            JSObjectViewRow {
                text: "Родитель"
                RowLayout {
                    Label {
                        text: handle.parent.nodeId
                    }
                    ToolButton {
                        text: "..."
                        onClicked: dia_selectParent.show(handle)
                    }
                    Item { Layout.fillWidth: true }
                }
            }

        }
        Loader {
            id: iconloader
            visible: active
            active: false
            property var blob
            property real adjustX
            property real adjustY
            Layout.fillHeight: true
            sourceComponent: Rectangle {
                implicitWidth: parent.height
                DRPicture {
                    anchors.fill: parent
                    anchors.margins: 8
                    sourceData: !!iconloader.blob ? iconloader.blob : ""
                    color: "#444"
                    adjustX: iconloader.adjustX
                    adjustY: iconloader.adjustY
                    onAdjustXChanged: update()
                    onAdjustYChanged: update()
                }
            }
        }
    }

    CaptionLabel {
        Layout.fillWidth: true
        text: "Связи"
        level: 3
    }

    Button {
        text: 'Добавить связь'
        onClicked: createEdgeDialog.open()
    }

    RowLayout {
        Connections {
            target: loader
            onObjectDataChanged: {
                t1.visibleChanged()
                t2.visibleChanged()
            }
        }
        TableView {
            id: t1
            visible: objectData.inCount !== 0
            Layout.fillWidth: true
            model: inEdges
            rowDelegate: Rectangle {
                color: styleData.selected ? '#3679d7' : styleData.alternate ? '#edf3fe': '#fff'
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.RightButton
                    onClicked: {
                        if( mouse.button === Qt.RightButton ) {
                            edgescontextMenu.currentItem = graphIndex.inEdges()[styleData.row]
                            //                            edgescontextMenu.currentItem = objectData.handle.inEdges()[styleData.row]
                            edgescontextMenu.popup()
                        }
                    }
                }
            }
            TableViewColumn {
                role: 'tag'
                title: 'Ребро'
                width: 120
                delegate: Item {
                    RowLayout {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 5
                        anchors.right: parent.right
                        anchors.rightMargin: 5
                        Label {
                            color: styleData.textColor
                            elide: Text.ElideMiddle
                            font.pixelSize: 20
                            text: '⇦'
                        }
                        Label {
                            color: styleData.textColor
                            elide: Text.ElideMiddle
                            font.pixelSize: 11
                            text: styleData.value
                            Layout.fillWidth: true
                            Layout.preferredWidth: paintedWidth
                            //                                Layout.maximumWidth: paintedWidth
                        }
                    }
                }
            }
            TableViewColumn {
                role: 'startNode'
                title: 'Узел'
                width: 300
                delegate: Item {
                    RowLayout {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 5
                        anchors.right: parent.right
                        anchors.rightMargin: 5
                        Label {
                            color: styleData.textColor
                            elide: Text.ElideMiddle
                            font.pixelSize: 20
                            text: '⇦'
                        }
                        Label {
                            color: styleData.textColor
                            elide: Text.ElideMiddle
                            font.pixelSize: 11
                            text: '<b>%1</b> MRL: <a href="node://%2">%2</a>'.arg(styleData.value.typeName).arg(styleData.value.mrl)
                            Layout.fillWidth: true
                            Layout.preferredWidth: paintedWidth
                            onLinkActivated: console.log("open link", link)
                            onHoveredLinkChanged: console.log("hovered", hoveredLink)
                            //                                Layout.maximumWidth: paintedWidth
                        }
                    }
                }
            }
        }
        TableView {
            id: t2
            visible: objectData.outCount !== 0
            Layout.fillWidth: true
            model: outEdges
            rowDelegate: Rectangle {
                color: styleData.selected ? '#3679d7' : styleData.alternate ? '#edf3fe': '#fff'
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.RightButton
                    onClicked: {
                        if( mouse.button === Qt.RightButton ) {
                            edgescontextMenu.currentItem = outEdges[styleData.row]
                            edgescontextMenu.popup()
                        }
                    }
                }
            }
            TableViewColumn {
                role: 'tag'
                title: 'Ребро'
                width: 120
                delegate: Item {
                    RowLayout {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 5
                        anchors.right: parent.right
                        anchors.rightMargin: 5
                        Label {
                            color: styleData.textColor
                            elide: Text.ElideMiddle
                            font.pixelSize: 20
                            text: '⇨'
                        }
                        Label {
                            color: styleData.textColor
                            elide: Text.ElideMiddle
                            font.pixelSize: 11
                            text: styleData.value
                            Layout.fillWidth: true
                            Layout.preferredWidth: paintedWidth
                            //                                Layout.maximumWidth: paintedWidth
                        }
                    }
                }
            }
            TableViewColumn {
                role: 'endNode'
                title: 'Узел'
                width: 300
                delegate: Item {
                    RowLayout {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 5
                        anchors.right: parent.right
                        anchors.rightMargin: 5
                        Label {
                            color: styleData.textColor
                            elide: Text.ElideMiddle
                            font.pixelSize: 20
                            text: '⇨'
                        }
                        Label {
                            color: styleData.textColor
                            elide: Text.ElideMiddle
                            font.pixelSize: 11
                            text: '<b>%1</b> MRL: <a href="node://%2">%2</a>'.arg(styleData.value.typeName).arg(styleData.value.mrl)
                            Layout.fillWidth: true
                            Layout.preferredWidth: paintedWidth
                            onLinkActivated: console.log("open link", link)
                            onHoveredLinkChanged: console.log("hovered", hoveredLink)
                        }
                    }
                }
            }
        }
    }

    Menu {
        id: edgescontextMenu
        property var currentItem: null
        MenuItem {
            text: 'Удалить связь'
            action: deleteEdgeAction
        }
    }

    Action {
        id: deleteEdgeAction
        text: 'Удалить связь'
        onTriggered: if( edgescontextMenu.currentItem ) edgescontextMenu.currentItem.destroy()
    }

    Action {
        id: editEdgeAction
        text: 'Редактировать тэг'
    }

    Action {
        id: crceateEdgeAction
        text: 'Добавить связь...'
    }

    Dialog {
        id: createEdgeDialog
        visible: false
        standardButtons: StandardButton.Cancel
        width: createEdgeDialogContent.width + 24
        property var target: null
        property alias edgetag: edgetagtextfield.text
        onAccepted: dasModel.addEdge( objectData.handle, target, edgetag )

        ColumnLayout {
            id: createEdgeDialogContent
            width: 300
            height: 400
            Label { text: 'Тег:' }
            TextField {
                id: edgetagtextfield
                Layout.fillWidth: true
            }
            Label { text: 'Конец:' }
            Loader {
                Layout.fillWidth: true
                Layout.fillHeight: true
                active: createEdgeDialog.visible
                sourceComponent: GraphTreeView {
                    id: selectionTreeView
                    graphModel: dasModel
                    selectionMode: SelectionMode.SingleSelection
                    sel.onSelectionChanged: {
                        if( sel.selectedRows().length > 0  ) {
                            createEdgeDialog.target = graphModel.getHandle( sel.selectedRows()[0] )
                            createEdgeDialog.standardButtons = StandardButton.Cancel | StandardButton.Ok
                        }else{
                            createEdgeDialog.standardButtons = StandardButton.Cancel
                        }
                    }
                }
            }
            Item { Layout.minimumHeight: 25 }
        }
    }



}
