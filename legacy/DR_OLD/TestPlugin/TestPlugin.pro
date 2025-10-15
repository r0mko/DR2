#-------------------------------------------------
#
# Project created by QtCreator 2013-01-22T23:48:06
#
#-------------------------------------------------

QT       -= gui

TARGET = TestPlugin
TEMPLATE = lib
CONFIG += plugin
DESTDIR = ../Core/plugins

DEFINES += TESTPLUGIN_LIBRARY

SOURCES += testplugin.cpp

HEADERS += testplugin.h\
        TestPlugin_global.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
