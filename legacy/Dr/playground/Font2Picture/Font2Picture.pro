TEMPLATE = app
CONFIG += c++11
QT += qml quick widgets

SOURCES += main.cpp \
    font2qpicture.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.

HEADERS += \
    font2qpicture.h
