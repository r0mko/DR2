TEMPLATE = lib
TARGET  = jscore
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../../Core/plugins/script
QT += core script
CONFIG += plugin
#TARGET=$$qtLibraryTarget($$TARGET)

SOURCES += \
    jscore.cpp

