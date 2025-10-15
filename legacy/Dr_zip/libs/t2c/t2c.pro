TEMPLATE = lib
TARGET = t2c
QT += qml quick widgets network
CONFIG += qt plugin c++11

TARGET = $$qtLibraryTarget($$TARGET)
uri = Dr.T2C
#INCLUDEPATH += /OldItems
# Input
#include(OldItems/_old.pri)

include(base.pri)
include(main.pri)
include(t2c_plugin.pri)

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

qmldir.files = qmldir
unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}

win32 {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}








