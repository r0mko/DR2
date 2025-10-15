TARGET = spatialite
macx {
LIBS *= /opt/local/lib/libsqlite3.dylib
DEFINES += SPATIALITE_EXTENSION=\\\"/opt/local/lib/mod_spatialite.so\\\"
}

unix: !macx {
LIBS *= -lsqlite3
DEFINES += SPATIALITE_EXTENSION=\\\"/usr/lib/mod_spatialite\\\"
}

win32 {
LIBS *= "C:/dev/sqlite/sqlite3.dll"
DEFINES += SPATIALITE_EXTENSION=\\\"C:/libspatialite.dll\\\"
}

wince*: DEFINES += HAVE_LOCALTIME_S=0

PLUGIN_CLASS_NAME = SpatialiteDriverPlugin

QT += core core-private sql-private

PLUGIN_TYPE = sqldrivers
load(qt_plugin)

#DEFINES += QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_ASCII

OTHER_FILES += \
    spatialite.json \
    .qmake.conf

SOURCES += \
    smain.cpp \
    sql_spatialite.cpp

HEADERS += \
    sql_spatialite_p.h
