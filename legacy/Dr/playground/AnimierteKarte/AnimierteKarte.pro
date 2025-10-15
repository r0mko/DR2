TEMPLATE = app

QT += qml quick widgets

SOURCES += main.cpp

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
    qml/LayersPanel.qml \
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
    qml/ValueEditors/VE_slider.qml

copydata.commands = $(COPY_DIR) $$PWD/qml $$OUT_PWD
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)

QMAKE_EXTRA_TARGETS += first copydata
DIRNAME = ROOTPATH=\\\"$$OUT_PWD\\\"
DEFINES += $$DIRNAME
