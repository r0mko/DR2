HEADERS += \
    rpcpeer.h \
    msgdefs.h \
    puppet.h \
    qdrobject.h \
    signalproxy.h \
    $$PWD/rpcattached.h \
    $$PWD/simulant.h \
    qmlpeer.h \
    $$PWD/engine.h \
    $$PWD/rpcprofile.h

SOURCES += \
    rpcpeer.cpp \
    puppet.cpp \
    qdrobject.cpp \
    signalproxy.cpp \
    $$PWD/rpcattached.cpp \
    $$PWD/simulant.cpp \
    qmlpeer.cpp \
    $$PWD/msg_handlers.cpp \
    $$PWD/engine.cpp \
    $$PWD/rpcprofile.cpp

INCLUDEPATH += $$PWD
