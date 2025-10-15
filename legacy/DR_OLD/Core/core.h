#ifndef CORE_H
#define CORE_H
#include <QtCore>
#include "coreinterface.h"
#include "messagedefs.h"
#include "resourcemanager.h"
#include <QMetaType>
/**
 * @brief Ядро DataReactor.
 */

class Core : public QObject, public CoreInterface
{
    Q_OBJECT
    struct CorePrivate
    {
        uint coreid;
        QVarLengthArray<PluginInfo> plugins;
        QHash<QString, int> registeredPlugins;
        QHash<QString, int> registeredEvents;
    } p;
    ResourceManager r;
    QObjectList objects;
    QScriptEngine* jsengine;
    static Core* self;
public:
    Core();
    QScriptEngine* engine() const { return jsengine; }
    Q_INVOKABLE int coreId() const { return p.coreid; }
    Q_INVOKABLE int loadPlugin(const QString &name);
    Q_INVOKABLE qint64 currentMsec();
    Q_INVOKABLE bool event(QEvent *e);
    Q_INVOKABLE Plugin* plugin(int id) { return p.plugins[id].ptr; }
    Q_INVOKABLE Plugin* getPluginInstance(const QString& name);
    Q_INVOKABLE int registerEvent(const QString &name);
    Q_INVOKABLE OUID registerObject(QObject* object);
    Q_INVOKABLE const QHash<QString, int>& getRegisteredEvents() const { return p.registeredEvents; }
    Q_INVOKABLE const QHash<QString, int>& getRegisteredPlugins() const { return p.registeredPlugins; }
    Q_INVOKABLE RUID createResource(int type, void* copy = 0);
    Q_INVOKABLE RUID addResource(const QVariant &data);
    Q_INVOKABLE static Core* instance() { if(self) return self; else { self=new Core(); return self; }; }
    Q_INVOKABLE static RUID newResource(int type, void* copy = 0);
    Q_INVOKABLE void log(int loglevel, const QObject* sender, const QString& message);
    Q_INVOKABLE RUID newRsc(int typeId, const void* copy = 0) { return r.allocateResource(typeId, copy); }
    Q_INVOKABLE void* getRsc(const RUID &id) { return r.getResource(id); }
    static void* getResource(const RUID &id) { return instance()->getRsc(id); }
    static void initJavaScript(QScriptEngine* engine);
    static QScriptValue consoleLog(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue importExtension(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue includeScript(QScriptContext *context, QScriptEngine *engine);
    static bool loadFile(QString fileName, QScriptEngine *engine);
private:
};

#endif // CORE_H
