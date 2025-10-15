TEMPLATE = lib
TARGET = rpc
QT += qml quick core core-private
CONFIG += qt plugin c++11

TARGET = $$qtLibraryTarget($$TARGET)
uri = DataReactor.RPC

# Input
include(network/network.pri)
include(plugin.pri)
include(engine.pri)

