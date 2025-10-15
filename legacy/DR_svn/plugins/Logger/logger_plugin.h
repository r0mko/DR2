#ifndef LOGGER_PLUGIN_H
#define LOGGER_PLUGIN_H

#include <QQmlExtensionPlugin>

class LoggerPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif // LOGGER_PLUGIN_H

