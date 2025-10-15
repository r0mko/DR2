#ifndef UI_PLUGIN_H
#define UI_PLUGIN_H

#include <QQmlExtensionPlugin>

class UIPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
    void initializeEngine(QQmlEngine *engine, const char *uri);
};

#endif // UI_PLUGIN_H

