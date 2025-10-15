TEMPLATE = lib
TARGET = rpc
QT += qml quick core core-private
CONFIG += qt plugin c++11

TARGET = $$qtLibraryTarget($$TARGET)
uri = Dr.RPC

# Input
include(network/network.pri)
include(engine.pri)
include(rpc_plugin.pri)

