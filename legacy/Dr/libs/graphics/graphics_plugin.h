#ifndef DYNAGRAPH_PLUGIN_H
#define DYNAGRAPH_PLUGIN_H

#include <QQmlExtensionPlugin>


class DynagraphPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};


#endif // DYNAGRAPH_PLUGIN_H

