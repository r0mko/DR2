#ifndef PLUGINTEMPLATE_H
#define PLUGINTEMPLATE_H
#include "../Core/plugininterface.h"
#include "../Core/core.h"
#include "PluginTemplate_global.h"

class Template : public QObject
{
    Q_OBJECT
public:
    Template() {}
    virtual ~Template() {}
};


class TemplateFactory : public Plugin
{
    Q_OBJECT
    Q_INTERFACES(Plugin)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID "org.PGE.DataReactor.T2C")
#endif
    Core* core;
public:
    TemplateFactory() {}
    virtual ~TemplateFactory() {}
    QObject* newInstance() { return new Template(); }
    int init(QObject *core);
    QVariantHash getInfo();
};

#endif // PLUGINTEMPLATE_H
