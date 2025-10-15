import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Window 2.2

import Dr.UI 1.0
import Dr.MetaGraph 1.0
import "ValueEditors"

Window {
    id: sectionSettingsWindow
    width: 300
    height: 400
    visible: false
    color: "#eee"
    property var handle
    property WiringSection currentSection: handle ? i_sections.instance(handle) : null
    onVisibleChanged: updateOutliner()
    onActiveChanged: updateOutliner()



    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        CaptionLabel {
            Layout.fillWidth: true
            text: "Участок %1".arg(handle.nodeId)
        }
        JSObjectView {
            id: secProp
            Layout.fillWidth: true
    //        anchors.fill: parent
//            onImplicitHeightChanged: { guide_panel.contentHeight = implicitHeight + 4 }
            property QtObject ndata: sectionSettingsWindow.handle.dataHandle

            JSObjectViewRow {
                text: "Цвет цепи «+»"
                valueDelegate: VE_QColor {
                    property color modelValue: secProp.ndata.posColor
                    function valueUpdated(color) {
                        secProp.ndata.posColor = color
                    }
                }
            }

            JSObjectViewRow {
                text: "Номер кабеля «+»"
                valueDelegate: TextField {
                    text: secProp.ndata.posCableId
                    onEditingFinished: secProp.ndata.posCableId = text
                    validator: IntValidator {}
                }
            }

            JSObjectViewRow {
                text: "Цвет цепи «-»"
                visible: sectionSettingsWindow.handle.data.type == "TrolleySection"
                valueDelegate: VE_QColor {
                    property color modelValue: secProp.ndata.negColor
                    function valueUpdated(color) {
                        secProp.ndata.negColor = color
                    }
                }
            }

            JSObjectViewRow {
                text: "Номер кабеля «-»"
                visible: sectionSettingsWindow.handle.data.type == "TrolleySection"
                valueDelegate: TextField {
                    text: Number(secProp.ndata.posCableId) + 50
                    onEditingFinished: secProp.ndata.negCableId = text
                    validator: IntValidator {}
                }
            }
            //                JSObjectViewRow {
            //                    text: "type"
            //                    valueDelegate: TextField {
            ////                        text: sectionSettingsWindow.sectionAgent.datanode$data.type
            ////                        onEditingFinished: sectionSettingsWindow.sectionAgent.datanode$data.type = text
            //                    }
            //                }
        }

        TableView {
            id: t_segs
            Layout.fillHeight: true
            Layout.fillWidth: true
            model: i_sections.instance(handle).segmentsModel

            onClicked: {
                currentSection.clearHighlight()
                currentSection.highlightSegment(currentSection.segmentsModel.getHandle(currentRow).uid)
            }
            onRowCountChanged: resizeColumnsToContents()

            TableViewColumn {
                role: "uid"
                title: "Отрезок"
            }
            TableViewColumn {
                role: "edgeData"
                title: "Смещение"
                delegate: Slider {
                    maximumValue: 20
                    stepSize: 0.5
                    value: styleData.value.offset
                    onValueChanged: {
                        var d = styleData.value
                        var h = t_segs.model.getHandle(styleData.row)
                        d.offset = value
                        h.setData(d)
                    }
                }
            }

        }
    }

    function updateOutliner() {
        if (active && visible) {
            outliner.color0 = "#0000ff"
            outliner.opacity = 0.5
            outliner.animate = false
        } else {
            outliner.color0 = "#000"
            outliner.opacity = 1
            outliner.animate = true
        }
    }


}
