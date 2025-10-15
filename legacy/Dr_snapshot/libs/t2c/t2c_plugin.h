#ifndef T2C_PLUGIN_H
#define T2C_PLUGIN_H

#include <QQmlExtensionPlugin>

class T2CPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
    void initializeEngine(QQmlEngine *engine, const char *uri);
};

#endif // T2C_PLUGIN_H
