#-------------------------------------------------
#
# Project created by QtCreator 2013-02-05T13:52:25
#
#-------------------------------------------------

QT       += script serialport

QT       -= gui
DESTDIR = ../Core/plugins
CONFIG += plugin serialport
TARGET = UF2Device
TEMPLATE = lib

DEFINES += UF2DEVICE_LIBRARY

SOURCES += uf2device.cpp

HEADERS += uf2device.h\
        UF2Device_global.h
