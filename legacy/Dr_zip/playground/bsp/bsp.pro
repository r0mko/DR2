TEMPLATE = app

QT += core qml quick widgets

CONFIG += c++11



DEFINES += INSTALLPATH=\\\"$$OUT_PWD\\\"

SOURCES += \
    main.cpp \
    bspdata.cpp \
    bspobject.cpp

DISTFILES += \
    main.qml


dist.path = $$OUT_PWD
dist.files = $$DISTFILES
INSTALLS += dist

HEADERS += \
    bspdata.h \
    bspobject.h
