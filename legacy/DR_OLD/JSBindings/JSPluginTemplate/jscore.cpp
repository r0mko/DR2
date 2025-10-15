#include <QtScript/QScriptExtensionPlugin>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptEngine>
#include <QtGlobal>
#include <QDebug>
//#include "../../Core/coreinterface.h"
//#include "../../Core/core.h"

class JSCore : public QScriptExtensionPlugin
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID "JSCore")
#endif

public:
    QStringList keys() const;
    void initialize(const QString &key, QScriptEngine *engine);
};

// -----------

QStringList JSCore::keys() const
{
    qDebug() << "keys() invoked!";
    QStringList list;
    list << QLatin1String("pge");
    list << QLatin1String("pge.core");
    return list;
}

void JSCore::initialize(const QString &key, QScriptEngine *engine)
{
    qDebug() << "initialize() invoked, key =" << key << "engine =" << engine;
//    engine->globalObject().setProperty("Core", engine->newQObject(Core::instance()));
}




#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
Q_EXPORT_PLUGIN2(jscore, JSCore)
//Q_EXPORT_STATIC_PLUGIN(JSCore)
#endif

