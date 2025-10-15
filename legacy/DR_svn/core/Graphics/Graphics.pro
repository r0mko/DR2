TEMPLATE = lib
TARGET = drgraphics
QT += qml quick
CONFIG += qt plugin c++11

TARGET = $$qtLibraryTarget($$TARGET)
uri = DataReactor.Graphics

# Input
SOURCES += \
    dgscene.cpp \
    dgitem.cpp \
    dgshapeitem.cpp \
    dgmouseevent.cpp \
    dgshape.cpp

HEADERS += \
    dgscene.h \
    dgitem.h \
    dgcursor.h \
    dgshapeitem.h \
    dgmouseevent.h \
    dgshape.h
include(plugin.pri)

