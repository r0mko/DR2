TEMPLATE = app

QT += qml quick widgets qml_private core_private
#QT += qml quick widgets
CONFIG += c++11

# Default rules for deployment.
SOURCES += \
    $$PWD/main.cpp


DISTFILES += \
    viewer/AddLayerForm.qml \
    viewer/AddLayersDialog.qml \
    viewer/AutoListObject.qml \
    viewer/AuxModel.qml \
    viewer/DataSourcesPanel.qml \
    viewer/DataSourcesView.qml \
    viewer/DockableWindow.qml \
    viewer/GeomBufferDelegate.qml \
    viewer/GeometryTreeDelegate.qml \
    viewer/InstanceWatcher.qml \
    viewer/LayerSettings.qml \
    viewer/LayersPanel.qml \
    viewer/LayersTable.qml \
    viewer/LayersTree.qml \
    viewer/LayersView.qml \
    viewer/LayerTreeDelegate.qml \
    viewer/main.qml \
    viewer/ModelWeaver.qml \
    viewer/Sidebar.qml \
    viewer/SidebarPanel.qml \
    viewer/StyleEditor.qml \
    viewer/TableCellQColor.qml \
    viewer/TinyButton.qml \
    viewer/VectorIconButton.qml \
    viewer/Weaver.qml \
    viewer/WeaverDelegate.qml \
    viewer/spv_base.mgf \
    viewer/icons.mgf \
    viewer/TableCelldouble.qml \
    viewer/GeometryIcon.qml \
    viewer/PicToolButton.qml \
    viewer/StyleListDelegate.qml \
    viewer/test.qml \
    viewer/ValueEditors/BaseEditor.qml \
    viewer/ValueEditors/VE_QColor.qml \
    viewer/AddGeometriesDialog.qml \
    viewer/ValueEditors/VE_slider.qml \
    viewer/ValueEditors/VE_double.qml \
    viewer/MarkerItem.qml \
    viewer/ControlStar.qml \
    viewer/Marker.qml

HEADERS +=
unix {
copydata.commands = $(COPY_DIR) $$PWD/viewer $$OUT_PWD
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
}
QMAKE_EXTRA_TARGETS += first copydata
DIRNAME = ROOTPATH=\\\"$$OUT_PWD\\\"
DEFINES += $$DIRNAME
