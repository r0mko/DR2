TEMPLATE = lib
TARGET = ui
QT += qml quick qml-private quick-private
CONFIG += qt plugin c++11

TARGET = $$qtLibraryTarget($$TARGET)
uri = Dr.UI

include(base.pri)
include(uni.pri)
include(ui_plugin.pri)

