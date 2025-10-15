TEMPLATE = app
QT += qml quick widgets network
CONFIG += c++11

copydata.commands = $(COPY_DIR) $$PWD $$OUT_PWD/..
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)

QMAKE_EXTRA_TARGETS += first copydata
DIRNAME = ROOTPATH=\\\"$$OUT_PWD\\\"
DEFINES += $$DIRNAME

SOURCES += \
    main.cpp \
    t2cmanager.cpp \
    tokenlist.cpp \
    pv.cpp

DISTFILES += \
    main.qml

HEADERS += \
    pv.h \
    pvlistener.h \
    t2cmanager.h \
    tokenlist.h
