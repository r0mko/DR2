HEADERS += \
    $$PWD/idsharedmemory.h \
    $$PWD/prunner.h \
    $$PWD/xvfb.h \
    $$PWD/xvfbevent.h \
    $$PWD/xvfbimage.h \
    $$PWD/xvfbitem.h \
    $$PWD/xvfbrenderer.h

SOURCES += \
    $$PWD/idsharedmemory.cpp \
    $$PWD/prunner.cpp \
    $$PWD/xvfb.cpp \
    $$PWD/xvfbevent.cpp \
    $$PWD/xvfbimage.cpp \
    $$PWD/xvfbitem.cpp \
    $$PWD/xvfbrenderer.cpp

DISTFILES += \
    $$PWD/qml/Xvfb.qml

qml.files = $$DISTFILES

unix {
    qml.path = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)/qml
    INSTALLS += qml
}
