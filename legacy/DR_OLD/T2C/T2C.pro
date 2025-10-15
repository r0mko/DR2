#-------------------------------------------------
#
# Project created by QtCreator 2013-01-23T21:17:51
#
#-------------------------------------------------

QT       += network script

QT       -= gui
DESTDIR = ../Core/plugins
CONFIG += plugin
TARGET = T2C
TEMPLATE = lib

DEFINES += T2C_LIBRARY

SOURCES += t2c.cpp

HEADERS += t2c.h\
        T2C_global.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib`
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
