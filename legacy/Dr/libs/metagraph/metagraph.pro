TEMPLATE = lib
TARGET = metagraph
QT += qml quick quick-private qml-private core-private
CONFIG += qt plugin c++11

TARGET = $$qtLibraryTarget($$TARGET)
uri = Dr.MetaGraph

# Input
INCLUDEPATH += $$PWD/modules
INCLUDEPATH += $$PWD/base
INCLUDEPATH += $$PWD/typed

include(base/base.pri)
include(typed/typed.pri)
include(modules/modules.pri)
include(model.pri)
include(metagraph_plugin.pri)
include(graphics.pri)

DISTFILES = qmldir

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


OTHER_FILES += \
    ../index.qdoc

