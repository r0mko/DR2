#-------------------------------------------------
#
# Project created by QtCreator 2013-02-12T14:48:32
#
#-------------------------------------------------
QT       += script gui
DESTDIR = ../Core/plugins
TARGET = SpectrumBand
CONFIG += plugin
TEMPLATE = lib

DEFINES += SPECTRUMBANDFACTORY_LIBRARY

SOURCES += \
    polyband.cpp

HEADERS +=\
    spectrumbandplugin.h \
    spectrumbandplugin_global.h
