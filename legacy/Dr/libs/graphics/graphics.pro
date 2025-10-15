TEMPLATE = lib
TARGET = graphics
QT += qml quick quick-private qml-private core-private widgets
CONFIG += qt plugin c++11

TARGET = $$qtLibraryTarget($$TARGET)
uri = Dr.Graphics
INCLUDEPATH += OldItems/
INCLUDEPATH += items/
INCLUDEPATH += models/
INCLUDEPATH += scene/
INCLUDEPATH += tools/
# Input
include(OldItems/_old.pri)
include(items/items.pri)
include(models/models.pri)
include(scene/scene.pri)
include(tools/tools.pri)

include(graphics_plugin.pri)

OTHER_FILES = \
    qml/DrawArea.qml \
    qml/BackgroundGrid.qml \
    qml/SelectShaderEffect.qml \
    qml/CheckerboardShaderEffect.qml

qml.files = $$OTHER_FILES

unix {
    qml.path = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)/qml
    INSTALLS += qml
}

win32 {
    qml.path = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)/qml
    INSTALLS += qml
}
