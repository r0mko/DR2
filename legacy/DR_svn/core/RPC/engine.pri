HEADERS += \
    rpcpeer.h \
    msgdefs.h \
    puppet.h \
    signalproxy.h \
    $$PWD/rpcattached.h \
    $$PWD/simulant.h \
    qmlpeer.h \
    $$PWD/engine.h \
    $$PWD/rpcpeerpool.h

SOURCES += \
    rpcpeer.cpp \
    puppet.cpp \
    signalproxy.cpp \
    $$PWD/rpcattached.cpp \
    $$PWD/simulant.cpp \
    qmlpeer.cpp \
    $$PWD/msg_handlers.cpp \
    $$PWD/engine.cpp \
    $$PWD/rpcpeerpool.cpp

INCLUDEPATH += $$PWD
