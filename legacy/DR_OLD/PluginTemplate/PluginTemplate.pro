#-------------------------------------------------
#
# Project created by QtCreator 2013-02-12T14:48:32
#
#-------------------------------------------------
QT       += script
QT       -= gui
DESTDIR = ../Core/plugins
TARGET = PluginTemplate
TEMPLATE = lib

DEFINES += PLUGINTEMPLATE_LIBRARY

SOURCES += plugintemplate.cpp

HEADERS += plugintemplate.h\
        PluginTemplate_global.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
