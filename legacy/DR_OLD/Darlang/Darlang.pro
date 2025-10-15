#-------------------------------------------------
#
# Project created by QtCreator 2013-02-20T11:54:44
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = Darlang
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    drobject.cpp \
    darlang_parser.cpp \
    darlangparser.cpp \
    drcode.cpp

HEADERS += \
    drobject.h \
    darlangparser_p.h \
    darlangparser.h \
    drcode.h

OTHER_FILES += \
    darlang.g \
    grammar.txt
