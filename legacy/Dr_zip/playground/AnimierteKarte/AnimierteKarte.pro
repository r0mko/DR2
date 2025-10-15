TEMPLATE = app

QT += qml quick widgets xml
CONFIG += c++11

SOURCES += main.cpp
include(TransportSimulator/TransportSimulator.pri)
# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

DISTFILES += \
    qml/animap.qml \
    qml/RectHandle.qml \
    qml/HandleItem.qml \
    qml/MarkerItem.qml \
    qml/AuxModel.qml \
    qml/LayersView.qml \
    qml/LayersTree.qml \
    qml/LayerTreeDelegate.qml \
    qml/spv_base.mgf \
    qml/ui.mgf \
    qml/GeomBufferDelegate.qml \
    qml/LayerSettings.qml \
    qml/LayersTable.qml \
    qml/PicToolButton.qml \
    qml/ModelWeaver.qml \
    qml/AddLayersDialog.qml \
    qml/GeometryIcon.qml \
    qml/AddGeometriesDialog.qml \
    qml/DataSourcesPanel.qml \
    qml/AddLayerForm.qml \
    qml/DataSourcesView.qml \
    qml/InstanceWatcher.qml \
    qml/ValueEditors/BaseEditor.qml \
    qml/ValueEditors/VE_double.qml \
    qml/ValueEditors/VE_QColor.qml \
    qml/ValueEditors/VE_slider.qml \
    qml/RoadGraph.qml \
    qml/RouteBuilder.qml \
    qml/NetsPanel.qml \
    qml/WiringSection.qml \
    qml/SegmentsPanel.qml \
    qml/BoundariesPanel.qml \
    qml/BoundsMaker.qml \
    qml/DiupWindow.qml \
    qml/GeoLayerMaker.qml \
    qml/SLDBMaker.qml \
    qml/StyleMaker.qml \
    qml/SectionSettingsWindow.qml \
    qml/SectionMaker.qml \
    pic/b2a2.png \
    pic/a2b2.png \
    pic/b2a1.png \
    pic/a2b1.png \
    pic/b2a+.png \
    pic/a2b+.png \
    pic/b2a-.png \
    pic/a2b-.png
unix {
copydata.commands = $(COPY_DIR) $$PWD/qml $$OUT_PWD
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
}
QMAKE_EXTRA_TARGETS += first copydata
DIRNAME = ROOTPATH=\\\"$$OUT_PWD\\\"
DEFINES += $$DIRNAME

dist.path = $$OUT_PWD
dist.files = $$DISTFILES

INSTALLS += dist
