#-------------------------------------------------
#
# Project created by QtCreator 2013-01-22T21:15:44
#
#-------------------------------------------------

QT       += core script gui scripttools widgets network

TARGET = Core
CONFIG   += console
CONFIG   += app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    core.cpp \
    resourcemanager.cpp \
    networkmanager.cpp

HEADERS += \
    plugininterface.h \
    coreinterface.h \
    core.h \
    messagedefs.h \
    resourcedefs.h \
    resourcemanager.h \
    networkmanager.h

OTHER_FILES += \
    JS/main.js \
    JS/parsewiki.js
