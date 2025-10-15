import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Extras 1.4
import Font2Pic 1.0
import Dr.UI 1.0
import QtQuick.Controls.Styles 1.2
import QtQml.Models 2.2
import Dr.MetaGraph 1.0

ApplicationWindow {
    title: qsTr("Font2QImage")
    width: 800
    height: 800
    visible: true

    menuBar: MenuBar {
        Menu {
            title: "Файл"
            MenuItem { action: a_openModel }
            MenuItem { action: a_saveAs }
            MenuItem { action: a_save }
            MenuItem { action: a_importFont }
        }
    }

    Action {
        id: a_openModel
        text: "Open..."
        shortcut: "Ctrl+O"
        onTriggered: {
            fileselector.open()
        }
    }
    Action {
        id: a_saveAs
        text: "Save As..."
        shortcut: "Ctrl+Shift+S"
        onTriggered: {
            saveFileselector.open()
        }
    }
    Action {
        id: a_save
        text: "Save %1".arg(main_model.file)
        enabled: main_model.file.toString().length > 0
        shortcut: "Ctrl+S"
        onTriggered: {
            main_model.exportModel(main_model.file, main_model.root)
        }
    }
    Action {
        id: a_importFont
        text: "Import font..."
        shortcut: "Ctrl+Shift+I"
        onTriggered: {
            fontPicker.open()
        }
    }

    FileDialog {
        id: fileselector
        selectExisting: true
        //        folder: shortcuts.desktop
        nameFilters: ["MetaGraph model files (*.mgf)"]
        onAccepted: {
            main_model.file = fileUrl
            main_model.loadModel(fileUrl)
        }
    }
    FileDialog {
        id: saveFileselector
        //        folder: shortcuts.desktop
        //        modality: Qt.ApplicationModal
        selectExisting: false
        nameFilters: ["MetaGraph model files (*.mgf)"]

        onAccepted: {
            var st = fileUrl.toString()
            //            var p = st.toString().lastIndexOf(".mgf")
            //            if (p < st.length - 4)
            //                fileUrl = Qt.resolvedUrl(st + ".mgf")
            main_model.file = fileUrl
            main_model.exportModel(fileUrl, main_model.rootIndex)//, iconModel.rootIndex)
        }
    }

    //    Action {
    //        id: a_export
    //        text: enabled ? "Export %1 glyphs".arg(rawfont.glyphCount) : "Export glyphs"
    //        enabled: rawfont.isValid
    //        onTriggered: outDirPicker.open()
    //    }

    Component {
        id: outFolder_comp
        RowLayout {
            Label {
                text: exportDirPicker.folder
            }
            Button {
                id: btn_export
                action: a_export
            }
        }
    }

    SystemPalette { id: palette; }

    GraphModel {
        id: main_model
        property url file
        RootNodeFactory { }
        DataNodeFactory { id: fDataNode }
        NodeFactory { id: fNode }
        MetaNodeFactory { id: fMetaNode }
        BlobFactory { id: fBlob }
        CollectionFactory { id: fCollection }
        TypeBuilder { id: builder }

        function init() {
            var root = fNode.create("font2pic")
            fNode.parentHandle = root
            fNode.create("fonts")
            var i = fNode.create("icons")
            fNode.parentHandle = i
            fNode.create("iconPictures")
            fNode.parentHandle = i
            fNode.create("iconPaths")
        }
    }
    NodeChildrenModel {
        id: fonts_model
        model: main_model
        mrl: "/font2pic/fonts"
    }


    NodeChildrenModel {
        id: iconModel
        model: main_model
        mrl: "/font2pic/icons/iconPictures"
    }
    NodeChildrenModel {
        id: iconPathModel
        model: main_model
        mrl: "/font2pic/icons/iconPaths"
    }

    Component.onCompleted: main_model.init()

    Instantiator {
        id: fonts_inst
        model: fonts_model
        Font2Picture {
            id: rawfont
            fontFile: handle.data ? handle.data.url : ""
            pixelSize: 32
            onIsValidChanged: if (isValid) scan()
            onIconBlobAdded: {
                fBlob.setParent(iconModel)
                fBlob.setData(blob)
                fBlob.metadata = { name: name, glyphIndex: glyphIndex, adjustX: 0, adjustY: 0 }
                fBlob.type = "picture"
                var ico = fBlob.create(name)
                //                var ico = fBlob.create(handle.nodeId + "%1%2".arg(name).arg(iconModel.count))

                fBlob.setParent(iconPathModel)
                fBlob.setData(blobpath)
                fBlob.metadata = { name: name, glyphIndex: glyphIndex, adjustX: 0, adjustY: 0 }
                fBlob.type = "painterpath"
                var path = fBlob.create(name)
            }

        }
    }

    SplitView {

        anchors.fill: parent
        anchors.margins: 8


        ColumnLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            RowLayout {
                //            height: 30
                //            Layout.preferredHeight: 30
                Layout.fillWidth: true
                Item {
                    //                Layout.fillHeight: true
                    height: 32
                    width: height
                    ColorPicker {
                        id: bkColor
                        width: 22
                        height: 22
                        color: "#eee"
                    }
                    ColorPicker {
                        id: fgColor
                        x: 8
                        y: 8
                        width: 22
                        height: 22
                        color: "#000"
                    }
                }
                ColumnLayout {
                    Text {
                        text: "Antialias"
                        renderType: Text.QtRendering
                        font.family: "Helvetica Neue"
                        font.weight: Font.Light

                        //                        width: showAll_label.width
                    }
                    Switch {
                        id: aaSwitch
                        Layout.alignment: Qt.AlignHCenter
                    }
                }
                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Text {
                        renderType: Text.QtRendering
                        Layout.alignment: Qt.AlignHCenter
                        text: "Icon size (px)"
                        font.family: "Helvetica Neue"
                        font.weight: Font.Light

                    }

                    Slider {
                        id: iconSize
                        Layout.fillWidth: true
                        value: 32
                        stepSize: 0.5
                        minimumValue: 8
                        maximumValue: 80
                        style: SliderStyle {
                            handle: Rectangle {
                                width: 30
                                height: 18
                                radius: 4
                                color: "#444"
                                Text {
                                    font.pixelSize: 10
                                    anchors.centerIn: parent
                                    color: "#eee"
                                    text: control.value
                                }
                            }
                        }
                    }
                }
            }



            Rectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true
                color: bkColor.color

                MessageDialog {
                    id: deleteIconAlert
                    modality: Qt.ApplicationModal
                    text: "Delete current icon?"
                    standardButtons: StandardButton.Ok | StandardButton.Cancel
                    onAccepted: {
                        console.log("remove icon", icons.currentIndex)
                        iconModel.remove(icons.currentIndex)
                    }
                }

                ScrollView {
                    clip: true
                    anchors.fill: parent
                    frameVisible: true
                    Keys.onPressed: {
                        if(event.key === Qt.Key_Backspace || event.key === Qt.Key_Delete)
                            deleteIconAlert.open()
                    }
                    //                Keys.onSpacePressed: {
                    //                    var h
                    //                    if (filterTemp.checked) {
                    //                        h = tempIconModel.getHandle(icons.currentIndex)
                    //                        var d = h.data
                    //                        if (h.data.choosen) {
                    //                            var sn = h.inNodes("ref")
                    //                            console.log("source node", sn[0])
                    //                            sn[0].destroy()
                    //                            d.choosen = false
                    //                            h.data = d
                    //                        } else {
                    //                            fDataNode.setParent(iconModel)
                    //                            fDataNode.data = h.data
                    //                            var nh = fDataNode.create(h.nodeId)
                    //                            main_model.addEdge(nh, h, "ref")
                    //                            d.choosen = true
                    //                            h.data = d
                    //                        }
                    //                    } else {
                    //                        h = iconModel.getHandle(icons.currentIndex)
                    //                        var dn = h.outNodes("ref")[0]
                    //                        dn.data.choosen = false
                    //                        h.destroy()
                    //                    }
                    //                }
                    focus: true
                    GridView {
                        id: icons
                        anchors.fill: parent
                        anchors.margins: 2
                        clip: false
                        remove: Transition { NumberAnimation { property: "scale"; to: 0; duration: 200 } }
                        removeDisplaced: Transition { NumberAnimation { properties: "x,y"; duration: 200; easing.type: Easing.OutQuint } }
                        cellWidth: Math.max(iconSize.value * 3, 96)
                        cellHeight: Math.max(iconSize.value * 2, 64)
                        highlight: Item {
                            x: icons.currentItem.x
                            y: icons.currentItem.y
                            width: icons.cellWidth
                            height: icons.cellHeight
                            Rectangle {
                                anchors.fill: parent
                                anchors.margins: -2
                                radius: 4
                                color: "transparent"
                                border.width: 2
                                border.color: Qt.tint(fgColor.color, "#8040abff")
                            }
                        }

                        highlightFollowsCurrentItem: false
                        model: iconModel
                        delegate: Item {
                            id: root
                            property alias iconPic: iconPicture
                            width: icons.cellWidth
                            height: icons.cellHeight
                            clip: true
                            //                        anchors.fill: parent
                            //                        implicitHeight: childrenRect.height
                            //                        implicitWidth: childrenRect.width
                            //                        Rectangle {
                            //                            anchors.fill: parent
                            //                            color: "transparent"
                            //                            border.width: 2
                            //                            radius: 2
                            //                            border.color: "#00ff60"
                            //                            visible: handle.data.choosen && filterTemp.checked
                            //                        }

                            ColumnLayout {

                                id: icon_view
                                anchors.fill: parent
                                width: icons.cellWidth
                                Item {
                                    //                                Layout.fillHeight: true
                                    implicitHeight: iconRect.height
                                    Layout.fillWidth: true
                                    //                                    clip: true

                                    Item {
                                        id: iconRect
                                        anchors.centerIn: parent
                                        width: iconSize.value
                                        height: iconSize.value

                                        DRPicture {
                                            id: iconPicture
                                            //                                            smooth: true
                                            antialiasing: aaSwitch.checked
                                            //                                            onAntialiasingChanged: update()
                                            aspectRatio: Qt.IgnoreAspectRatio
                                            //                                        renderTarget: DRPicture.FramebufferObject
                                            anchors.centerIn: parent
                                            width: Math.ceil(iconPicture.implicitWidth * iconSize.value / 32)
                                            height: Math.ceil(iconPicture.implicitHeight * iconSize.value / 32)
                                            sourceData: handle.data
                                            adjustX: handle.metadata.adjustX
                                            adjustY: handle.metadata.adjustY
                                            color: fgColor.color
                                        }
                                    }
                                }

                                EditableText {
                                    id: node_id
                                    editable: root.GridView.isCurrentItem
                                    Layout.fillWidth: true
                                    font.pixelSize: 10
                                    value: nodeId
                                    horizontalAlignment: Text.AlignHCenter
                                    onValueUpdated: handle.nodeId = newValue
                                    textColor: fgColor.color
                                    font.family: "Helvetica Neue"
                                    font.weight: Font.Light
                                }
                            }
                            Menu {
                                id: ctxMenu
                                MenuItem {
                                    text: 'export as QPainterPath'
                                }
                                MenuItem {
                                    text: 'export as QPicture'
                                }
                            }

                            MouseArea {
                                id: mouseArea
                                enabled: !root.GridView.isCurrentItem
                                hoverEnabled: true
                                anchors.fill: parent
                                onClicked: {
                                    if (mouse.button == Qt.LeftButton) {
                                        root.GridView.view.currentIndex = index
                                    }
                                    else if(mouse.button == Qt.RightButton) {

                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        Column {
            Layout.fillHeight: true
            Layout.minimumWidth: 280
            Rectangle {
                //                Layout.fillWidth: true
                color: bkColor.color
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.rightMargin: 8
                anchors.leftMargin: 8
                implicitHeight: icons.currentItem.iconPic.implicitHeight * (width / icons.currentItem.iconPic.implicitWidth)
                border.width: 1

                ShaderEffectSource {
                    id: icoRaster
                    anchors.fill: parent
                    smooth: false
                    sourceItem: icons.currentItem.iconPic
                }
            }

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.rightMargin: 8
                anchors.leftMargin: 8
                implicitHeight: width
                border.width: 1
                clip: true
                Rectangle {
                    property QtObject obj: iconModel.get(icons.currentIndex)
                    id: rect
                    color: "#ffcc80"
                    border.width: 2
                    width: 20
                    height: 20
                    x: icons.currentItem.iconPic.adjustX + parent.width / 2
                    y: icons.currentItem.iconPic.adjustY + parent.height / 2
                    radius: 5
                    transform: Translate {
                        x: -rect.width / 2
                        y: -rect.height / 2
                    }

                    MouseArea {
                        anchors.fill: parent
                        drag.target: parent
                        onPositionChanged: {
                            if (!pressed)
                                return
                            var h = rect.obj.handle
                            var hm = rect.obj.handle.metadata
                            //                        console.log(JSON.stringify(hm))
                            hm.adjustX = rect.x - rect.parent.width / 2
                            hm.adjustY = rect.y - rect.parent.height / 2
                            h.metadata = hm
                            rect.obj.handle = h

                        }
                    }
                }
            }

        }
    }

    FileDialog {
        id: fontPicker
        nameFilters: [ "Font files (*.ttf *.otf)" ]
        //        folder: shortcuts.desktop
        selectExisting: true
        onAccepted: {
            fDataNode.setParent(fonts_model)
            var name = fileUrl.toString().split("/")
            fDataNode.data = { url: fileUrl.toString() }
            fDataNode.create(name[name.length - 2])
        }

    }
    FileDialog {
        id: exportDirPicker
        folder: fontPicker.folder
        selectFolder: true
        selectExisting: true
        onAccepted: {
            console.log("exporting %1 glyphs into %2".arg(rawfont.glyphCount).arg(folder), fileUrl)
            rawfont.outputFolder = folder
            for (var i = 1; i < iconModel.count; ++i) {
                var d = iconModel.getHandle(i)
            }

            for (var i = 1; i < rawfont.glyphCount + 1; ++i)
                var res = rawfont.createPicForGlyph(i);
        }
    }

    //    ListModel {
    //        id: iconModel
    //    }

}
