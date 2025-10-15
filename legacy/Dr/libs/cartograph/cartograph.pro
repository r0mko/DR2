TEMPLATE = lib
TARGET = cartograph
QT += core qml gui quick quick_private qml_private gui_private core_private widgets concurrent sql
CONFIG += qt plugin c++11

TARGET = $$qtLibraryTarget($$TARGET)
uri = Dr.Cartograph

# Input
SOURCES += \
    spatialitedb.cpp \
#    style.cpp \
    defs.cpp \
    stroketriangulator.cpp \
    triangulator.cpp \
#    utm.cpp \
#    streetlayer.cpp \
#    snipfilter.cpp \
#    graphbuilder.cpp \
#    graph.cpp \
    geometrydatasource.cpp \
    geolayer.cpp \
    smartobject.cpp \
    instantiatorcollection.cpp \
    geoview.cpp \
    geombuffer.cpp \
    spatialquery.cpp \
    wkt.cpp \
    graphgenerator.cpp

HEADERS += \
    spatialitedb.h \
#    style.h \
    defs.h \
    triangulator.h \
    stroketriangulator.h \
#    streetlayer.h \
#    snipfilter.h \
#    graphbuilder.h \
#    graph.h \
    geometrydatasource.h \
    geolayer.h \
    smartobject.h \
    instantiatorcollection.h \
    geoview.h \
    geombuffer.h \
    spatialquery.h \
    wkt.h \
    graphgenerator.h


include(plugin.pri)
include(style.pri)

