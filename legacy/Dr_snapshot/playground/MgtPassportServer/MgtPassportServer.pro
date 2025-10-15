TEMPLATE = app

TARGET = mgt_server

QT += core network qml sql

CONFIG += c++11

SOURCES += \
    main.cpp \
    server.cpp \
    connection.cpp \
    settings.cpp

HEADERS += \
    server.h \
    connection.h \
    settings.h

DISTFILES += \
    settings.json

settings.path = $$OUT_PWD
settings.files = $$DISTFILES

INSTALLS += settings

DEFINES += INSTALLPATH=\\\"$$OUT_PWD\\\"
DEFINES += NONETDEBUGOUTPUT
