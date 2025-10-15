#ifndef COREINTERFACE_H
#define COREINTERFACE_H
#include <QObject>
#include "plugininterface.h"
#include <QPluginLoader>
#include "messagedefs.h"
#include "resourcedefs.h"


struct PluginInfo
{
    Plugin*   ptr;
    QPluginLoader* loader;
    QString   name;
    int       version;
};
Q_DECLARE_METATYPE(PluginInfo)
/**
 * @brief struct OUID - уникальный идентификатор объекта в распределённой системе. Содержит ID клиента (coreId) и объекта (oId)
 */
struct OUID
{
    int coreId;
    int oId;
};
Q_DECLARE_METATYPE(OUID)

/**
 * @brief The CoreInterface class
 * @class CoreInterface
 * Интерфейс ядра
 */
class CoreInterface
{
public:
    virtual int coreId() const = 0;
    virtual QScriptEngine* engine() const = 0;
    virtual Plugin* plugin(int id) = 0;
    virtual Plugin* getPluginInstance(const QString& name) = 0;
    virtual int loadPlugin(const QString &name) = 0;
    virtual int registerEvent(const QString& name) = 0;
    virtual OUID registerObject(QObject* object) = 0;
    virtual const QHash<QString, int>& getRegisteredEvents() const = 0;
    virtual const QHash<QString, int>& getRegisteredPlugins() const = 0;
    virtual void log(int loglevel, const QObject* sender, const QString& message) = 0;
    virtual RUID newRsc(int typeId, const void* copy) = 0;
    virtual void* getRsc(const RUID& id) = 0;
    virtual RUID addResource(const QVariant &data) = 0;
};




#endif // COREINTERFACE_H
