#ifndef QRPC_PLUGIN_H
#define QRPC_PLUGIN_H

#include <QQmlExtensionPlugin>

class QRPCPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif // QRPC_PLUGIN_H

