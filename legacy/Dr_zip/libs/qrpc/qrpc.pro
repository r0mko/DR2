TEMPLATE = lib
TARGET = qrpc
QT += qml quick core core-private
CONFIG += qt plugin c++11

TARGET = $$qtLibraryTarget($$TARGET)
uri = Dr.QRPC

# Input
include(network/network.pri)
include(plugin.pri)
include(engine.pri)

