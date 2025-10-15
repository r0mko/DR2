TEMPLATE = app

QT += qml quick widgets xml
CONFIG += c++11


SOURCES += main.cpp
RESOURCES += qml.qrc
INCLUDEPATH += /opt/local/include
# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

include(TransportSimulator/TransportSimulator.pri)

HEADERS +=
