TEMPLATE = app

QT += widgets quick qml

CONFIG += c++11

SOURCES += \
    main.cpp

DIRNAME = ROOTPATH=\\\"$$OUT_PWD\\\"
DEFINES += $$DIRNAME


DISTFILES += \
    main.qml


dist.path = $$OUT_PWD
dist.files = $$DISTFILES
INSTALLS += dist
