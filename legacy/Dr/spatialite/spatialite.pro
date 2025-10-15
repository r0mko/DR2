TARGET = spatialite
macx {
LIBS *= /usr/local/opt/sqlite/lib/libsqlite3.dylib
DEFINES += SPATIALITE_EXTENSION=\\\"/usr/local/lib/mod_spatialite\\\"
}

unix: !macx {
LIBS *= -lsqlite3
DEFINES += SPATIALITE_EXTENSION=\\\"/usr/lib/mod_spatialite\\\"
}

win32 {
LIBS *= C:\usr\lib\sqlite3.lib
DEFINES += SPATIALITE_EXTENSION=\\\"C:\spatialite.dll\\\"
}

wince*: DEFINES += HAVE_LOCALTIME_S=0

PLUGIN_CLASS_NAME = SpatialiteDriverPlugin

QT  = core core-private sql-private

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
