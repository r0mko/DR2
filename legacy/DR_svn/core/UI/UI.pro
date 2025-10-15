TEMPLATE = lib
TARGET = UI
QT += qml quick qml-private quick-private
CONFIG += qt plugin c++11

TARGET = $$qtLibraryTarget($$TARGET)
uri = DataReactor.UI

include(base.pri)
include(uni.pri)
include(UI_plugin.pri)
