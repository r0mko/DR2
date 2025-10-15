#ifndef GRAPHMODEL_PLUGIN_H
#define GRAPHMODEL_PLUGIN_H

#include <QQmlExtensionPlugin>

class GraphModelPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
    void initializeEngine(QQmlEngine *engine, const char *uri);
};

#endif // GRAPHMODEL_PLUGIN_H

