QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TransportSimulator
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    transportsimulator.cpp \
    xmlhandler.cpp \
    renderarea.cpp \
    renderscene.cpp

HEADERS  += mainwindow.h \
    transportsimulator.h \
    xmlhandler.h \
    renderarea.h \
    renderscene.h

FORMS    += mainwindow.ui
