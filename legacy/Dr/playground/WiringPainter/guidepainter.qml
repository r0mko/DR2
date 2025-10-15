import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import Dr.Graphics 1.0
import Dr.UI 1.0
import QtGraphicalEffects 1.0
import Dr.Cartograph 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Styles 1.2
import Dr.MetaGraph 1.0
import QtQuick.Window 2.2
AAWindow {
    id: mainWindow
    width: 1920
    height: 1000
    x: 0
    y: 0
    title: "Qt Wiring Editor"
    visible: true
    screenId: 0

    MenuBar {
        Menu {
            title: "Файл"
            MenuItem { action: a_openModel }
            MenuItem { action: a_saveAs }
            MenuItem { action: a_save }
            MenuItem { action: a_reloadModel }
        }
    }

    Rectangle {
        id: l_1
        color: "#222"
        anchors.fill: parent

        DrawArea {
            id: drawarea
            backgroundColor: "transparent"
            anchors.fill: parent
            scene.width: m_scene.sceneRect.width
            scene.height: m_scene.sceneRect.height
            maxScale: 512
            property var dat: []
            property rect sourceRect: Qt.rect(scrollArea.contentX / scene.scale, scrollArea.contentY / scene.scale, width / scene.scale, height / scene.scale)
            grid.visible: false

            MouseArea {
                id: scene_mouse
                acceptedButtons: Qt.AllButtons
                anchors.fill: parent
                onClicked: {
                    m_scene.clearSelection()
                    console.log("clicked scene mousearea")
                }
            }

            HandleItem {
                id: hover_handle
                visible: false
                width: 16
                height: 16
                border.color: "#ff0030"
                radius: 3
                mouseArea.drag.target: null
                mouseArea.onPressed: { mouse.accepted = false }
            }

            HandleItem {
                id: drag_handle
                visible: false
                property var runner
                property real startPos
                property real endPos
                width: 12
                height: 12
                border.color: "#88ffff"
                radius: 3
                mouseArea.drag.target: null
                mouseArea.onPressed: { mouse.accepted = false }
                function updateHandle() {
                    x = runner.point.x
                    y = runner.point.y
                    rotation = runner.angle
                }
            }

            Scene {
                id: m_scene
                enableBsp: true
                viewport: drawarea
                width: sceneRect.width
                height: sceneRect.height
                sceneRect: Qt.rect(0, 0, 1600, 1600)
                //                renderer.renderTarget: SceneRenderer.Image
                renderer.renderTarget: SceneRenderer.FramebufferObject
            }
        }

        SceneItemStroker {
            id: m_selectionOutline
            anchors.fill: drawarea
            scene: m_scene
        }
        ShaderEffectSource {
            id: m_selectionOutlineSrc
            sourceItem: m_selectionOutline
            anchors.fill: drawarea
            hideSource: true
        }

        SelectShaderEffect {
            anchors.fill: drawarea
            source: m_selectionOutlineSrc
        }
    }

    Item {
        id: l_2
        width: 240
        z: 100
        anchors.right: parent.right
        height: parent.height

        Rectangle {
            anchors.fill: parent
            opacity: 0.9
        }

        ColumnLayout {
            id: m_sidebar
            anchors.fill: parent
            anchors.margins: 8


            GroupBox {
                id: guidebuilder
                title: "Build guidepath"
                Layout.fillWidth: true
                ListModel {
                    id: handle_model
                }
                property var guide
                property var guideItem

                ColumnLayout {
                    Layout.fillWidth: true
                    RowLayout {
                        Layout.fillWidth: true
                        Switch {
                            id: drawguide
                            //                            exclusiveGroup:
                        }
                        Button {
                            text: "Add"
                            onClicked: {
                                var col = Qt.hsla(Math.random(), 1, 0.7)
                                var g = createGuide(guidebuilder.guideItem.source.clone(), col, 2)
                                //                                g.onHoverEnter = onHoverEnter
                                //                                g.onHoverMove = onHoverMove
                                //                                g.onHoverLeave = onHoverLeave
                                //                                g.onMousePress = onMousePress
                                //                                g.onMouseMove = onMouseMove
                                //                                g.onMouseRelease = onMouseRelease
                                g.acceptedButtons = Qt.LeftButton
                                g.smooth = guidebuilder.guideItem.smooth
                                g.offset = guidebuilder.guideItem.offset
                                guidebuilder.guideItem.source.clear()
                                guidebuilder.guideItem.updatePath()
                            }

                            function createGuide(path, color, width) {
                                var g = m_sceneBuilder.addItem(
                                            "GuidePathStroke", {
                                                shapeType: 1,
                                                hoverEnabled: true,
                                                source: path,
                                                flags: VisualInstance.ItemIsSelectable
                                            })
                                g.pen.color = color
                                g.pen.cosmetic = true
                                g.pen.width = width
                                g.updatePath()
                                g.onHoverEnter = onHoverEnter
                                g.onHoverMove = onHoverMove
                                g.onHoverLeave = onHoverLeave
                                g.onMousePress = onMousePress
                                g.onMouseMove = onMouseMove
                                g.onMouseRelease = onMouseRelease
                                return g
                            }

                            function onMousePress(event) {
                                if (!this.selected) {
                                    this.selected = true
                                    return
                                }
                                drag_handle.runner = this.result.findNearestPos(event.pos)
                                drag_handle.startPos = drag_handle.runner.position
                                drag_handle.visible = true
                                drag_handle.updateHandle()
                            }
                            function onMouseMove(event) {
                                drag_handle.runner = this.result.findNearestPos(event.pos)
                                drag_handle.updateHandle()
                            }

                            function onMouseRelease(event) {
                                if (!drag_handle.runner)
                                    return
                                drag_handle.runner = this.result.findNearestPos(event.pos)
                                var p1 = drag_handle.startPos
                                var p2 = drag_handle.runner.position
                                var gpath = this.result.part(p1, p2)
                                var g2 = createGuide(gpath, "#00FF00", 2)
                                g2.offset = 2
                                drag_handle.visible = false
                            }

                            function onHoverEnter(event) {
                                if (!this.selected)
                                    return
                                var r = this.result.findNearestPos(event.pos)
                                hover_handle.visible = true
                                hover_handle.x = r.point.x
                                hover_handle.y = r.point.y
                                hover_handle.rotation = r.angle
                            }

                            function onHoverMove(event) {
                                var r = this.result.findNearestPos(event.pos)
                                hover_handle.x = r.point.x
                                hover_handle.y = r.point.y
                                hover_handle.rotation = r.angle
                            }

                            function onHoverLeave(event) {
                                hover_handle.visible = false
                            }

                        }
                    }

                    TableView {
                        Layout.fillWidth: true
                        model: handle_model
                        TableViewColumn {
                            role: "xp"
                        }
                        TableViewColumn {
                            role: "yp"
                        }
                    }
                }
            }

            states: [

                State {
                    name: "createguide"
                    when: drawguide.checked
                    PropertyChanges {
                        target: scene_mouse
                        onClicked: {
                            if (!guidebuilder.guideItem) {
                                var i = m_sceneBuilder.addItem("GuidePathStroke", {
                                                                   source: creatist.create("GuidePath"),
                                                                   smooth: 50,
                                                                   precision: 5
                                                               })
                                i.pen.width = 0
                                i.pen.cosmetic = true
                                i.pen.color = "#fff"
                                i.pen.strokeStyle = Qt.DotLine
                                guidebuilder.guideItem = i
                            }
                            guidebuilder.guideItem.source.appendPoint(Qt.point(mouse.x, mouse.y))
                            guidebuilder.guideItem.updatePath()
                        }
                    }
                }
            ]

            FileDialog {
                id: importDialog
                //            id: fileselector
                modality: Qt.NonModal
                property var argument
                property var callback
                selectExisting: true
                nameFilters: ["MetaGraph model files (*.mgf)"]

                function runMultiple(callbackFn, arg) {
                    selectMultiple = true
                    callback = callbackFn
                    argument = arg
                    open()
                }
                function run(callbackFn, arg) {
                    callback = callbackFn
                    argument = arg
                    open()
                }
                onAccepted: {
                    if (selectMultiple) {
                        callback(fileUrls, argument)
                        selectMultiple = false
                    } else {
                        callback(fileUrl, argument)
                    }
                }
            }
            Item {
                Layout.fillHeight: true
            }
        }
    }

    GraphModel {
        id: gmodel
        property url file
        property string fileName: file.toString().length ? file.toString().match(/(\w+)\..+$/)[1] : "untitled"
        RootNodeFactory { }
        RootNodeFactory { id: f_RootNode }
        NodeFactory { id: f_Node }
        DataNodeFactory { id: f_DataNode }
        MetaNodeFactory { id: f_MetaNode }
        CollectionFactory { id: f_Collection }
        TypeBuilder { id: f_Type }
        BlobFactory { id: f_Blob }
        InstanceFactory { id: f_Instance }
        ElementFactory { id: f_Element }
        SqlBridgeNodeFactory { id: f_SqlBridgeNode }
        PointNodeFactory { id: f_PointNode }
        //        onLoaded: console.log("loaded model")
    }

    EdgeModel {
        id: edgeModel
        model: gmodel
        tag: 'polygon'
        onItemAdded: {
            var eh = getHandle(index)
            var phandle = m_sceneBuilder.addItem("PolygonItem", { flags: 0, shapeType: 1 })
            var col = Qt.hsla(Math.random(), 1, 0.7)
            phandle.polygon = eh.getData()
            phandle.pen.color = col.toString()
            phandle.pen.cosmetic = false
            phandle.pen.width = 1
        }
    }

    ListModel {
        id: polmodel
    }

    Action {
        id: a_openModel
        text: "Открыть..."
        shortcut: "Ctrl+O"
        onTriggered: {
            fileselector.open()
        }
    }
    Action {
        id: a_saveAs
        text: "Сохранить как..."
        shortcut: "Ctrl+Shift+S"
        onTriggered: {
            saveFileselector.open()
        }
    }
    Action {
        id: a_reloadModel
        text: "Перезагрузить модель"
        shortcut: "Ctrl+Shift+R"
        enabled: gmodel.file.toString().length > 0
        onTriggered: {
            gmodel.loadModel(gmodel.file)
        }
    }

    Action {
        id: a_save
        text: "Сохранить %1".arg(gmodel.file)
        enabled: gmodel.file.toString().length > 0
        shortcut: "Ctrl+S"
        onTriggered: gmodel.exportModel(gmodel.file, gmodel.root)
    }


    FileDialog {
        id: fileselector
        selectExisting: true
        //        modality: Qt.NonModal
        folder: shortcuts.desktop
        nameFilters: ["MetaGraph model files (*.mgf)"]
        onAccepted: {
            gmodel.file = fileUrl
            //            console.log("loading model", fileUrl.toString())
            gmodel.loadModel(fileUrl)
            //            console.log("model loaded")
        }
    }


    FileDialog {
        id: saveFileselector
        folder: shortcuts.desktop
        //        modality: Qt.NonModal
        selectExisting: false
        nameFilters: ["MetaGraph model files (*.mgf)"]
        onAccepted: {
            var st = fileUrl.toString()
            var p = st.toString().lastIndexOf(".mgf")
            if (p < st.length - 4)
                fileUrl = Qt.resolvedUrl(st + ".mgf")
            gmodel.file = fileUrl
            gmodel.exportModel(fileUrl, gmodel.rootIndex)
        }
    }

    SceneModel {
        id: m_sceneBuilder
        scene: m_scene.qGraphicsScene
        Component.onCompleted: {
            roleBindings.polygon = "edgeData"
        }
    }


    Creatist {
        id: creatist
    }
    PolygonFactory {
        id: polfactory
        model: polmodel
        xRole: "xp"
        yRole: "yp"
    }
}

