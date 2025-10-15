#ifndef XVFB_PLUGIN_H
#define XVFB_PLUGIN_H

#include <QQmlExtensionPlugin>

class XvfbPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
    void initializeEngine(QQmlEngine *engine, const char *uri);
};

#endif // XVFB_PLUGIN_H
