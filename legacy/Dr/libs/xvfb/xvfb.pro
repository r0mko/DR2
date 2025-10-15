TEMPLATE = lib
TARGET = xvfb
QT += qml quick widgets opengl # quick-private qml-private core-private
CONFIG += qt plugin c++11

TARGET = $$qtLibraryTarget($$TARGET)
uri = Dr.Xvfb
#INCLUDEPATH += /OldItems
# Input
#include(OldItems/_old.pri)

include(base.pri)
include(xvfb_plugin.pri)

#OTHER_FILES = \
#    qml/DrawArea.qml \
#    qml/BackgroundGrid.qml \
#    qml/SelectShaderEffect.qml \
#    qml/CheckerboardShaderEffect.qml

#qml.files = $$OTHER_FILES

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}


macx: {
    INCLUDEPATH += /opt/X11/include/
    LIBS += -L/opt/X11/lib
}
qmldir.files = qmldir
unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}


LIBS += -lX11 -lXtst


