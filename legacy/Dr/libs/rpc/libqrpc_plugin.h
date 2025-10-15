#ifndef LIBQRPC_PLUGIN_H
#define LIBQRPC_PLUGIN_H

#include <QQmlExtensionPlugin>

class QRPCPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif // LIBQRPC_PLUGIN_H

