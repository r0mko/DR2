TEMPLATE = subdirs
CONFIG += ordered
QMAKE_MAC_SDK = macosx10.12
SUBDIRS += \
    spatialite/spatialite.pro \
    libs/libs.pro \
    playground/playground.pro
