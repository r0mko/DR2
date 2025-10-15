#ifndef CARTOGRAPH_PLUGIN_H
#define CARTOGRAPH_PLUGIN_H

#include <QQmlExtensionPlugin>


class CartographPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
    void initializeEngine(QQmlEngine *engine, const char *uri);

};

#endif // CARTOGRAPH_PLUGIN_H

