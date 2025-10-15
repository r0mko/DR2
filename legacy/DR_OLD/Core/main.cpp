#include "core.h"
#include <QVariant>
#include "../TestPlugin/testplugin.h"
#include <QScriptEngine>
#include <QScriptEngineDebugger>
#include <QMetaMethod>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets/QApplication>
#else
#include <QApplication>
#endif
#include "qglobal.h"
#include <QScriptExtensionPlugin>

QScriptValue jsCallMethod(QScriptContext *ctx, QScriptEngine *e)
{
    Q_UNUSED(e)
    QScriptValue data = ctx->callee().data();
    // 0 - QObject pointer
    // 1 - Method index
    // 2 - Return value type
    // 3... - Argument types
    QObject* ptr = (QObject*)(quintptr)data.property(0).toNumber();
    int methodIndex = data.property(1).toInt32();
    void* args[11];
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    qDebug().nospace() << "Invoke " << ptr->metaObject()->className() << "." << ptr->metaObject()->method(methodIndex).name().data();
#else
    qDebug().nospace() << "Invoke " << ptr->metaObject()->className() << "." << ptr->metaObject()->method(methodIndex).signature();
#endif

    for(int i=0; i<ctx->argumentCount(); ++i)
    {
        if(ctx->argument(i).data().toInt32() == qMetaTypeId<RUID>())
        {
            args[i+1] = Core::getResource(qscriptvalue_cast<RUID>(ctx->argument(i)));
        }
        else
        {
            QVariant arg = ctx->argument(i).toVariant();
            args[i+1] = arg.data();
            qDebug() << QString("Added argument %0 type %1 value %2").arg(i).arg(arg.typeName()).arg(arg.toString());
        }
    }
    return QScriptValue(0);
}


QScriptValue RUID_JSctor(QScriptContext *, QScriptEngine *e)
{
    RUID i;

    return e->toScriptValue(i);
};

QScriptValue Resource_JSctor(QScriptContext *ctx, QScriptEngine *e)
{
    RUID i;
    QScriptValue object;
    if (ctx->isCalledAsConstructor()) {
        object = ctx->thisObject();
    } else {
        object = e->newObject();
        object.setPrototype(ctx->callee().property("prototype"));
    }
    return object;
};

QScriptValue RUIDtoSV(QScriptEngine *e, const RUID &id)
{
    QScriptValue i = e->newArray();
    QScriptValue svData = QScriptValue(qMetaTypeId<RUID>());
    i.setData(svData);
    i.setProperty(0, id.coreId);
    i.setProperty(1, id.typeId);
    i.setProperty(2, id.index);
    return i;
}

void RUIDfromSV(const QScriptValue &obj, RUID &id)
{
    id.coreId = obj.property(0).toInt32();
    id.typeId = obj.property(1).toInt32();
    id.index = obj.property(2).toInt32();
}


// создаёт привязку QObject в ScriptEngine (вызов методов с возможностью передавать указатели на ресурсы)
void createJSBinding(const QString& name, QScriptEngine* e, QObject *o)
{
    QScriptValue jsObject = e->newQObject(o);
    const QMetaObject* tmo = o->metaObject();
    for(int i=0; i<tmo->methodCount(); ++i)
    {
        QMetaMethod m = tmo->method(i);
        QScriptValue method = e->newFunction(jsCallMethod);
        QScriptValue data = e->newArray();
        int count = 0;
        data.setProperty(count++, (double)(quintptr)o);
        data.setProperty(count++, i);
        QList<QByteArray> parTypes = m.parameterTypes();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        data.setProperty(count++, m.returnType());
        int parameterCount = m.parameterCount();
        QString methodName = QString(tmo->method(i).name());
#else
        data.setProperty(count++, QMetaType::type(m.typeName()));
        int parameterCount = parTypes.size();
        QByteArray ba(m.signature());
        ba.truncate(ba.indexOf("("));
        QString methodName = ba;

#endif
        for(int j=0; j<parameterCount; ++j) {
            data.setProperty(count++, QMetaType::type(parTypes.at(j)));
        }
        method.setData(data);
        jsObject.setProperty(methodName, method);
    }
    e->globalObject().setProperty(name, jsObject);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QDir pluginDir(a.applicationDirPath());
#ifdef Q_OS_MAC
    pluginDir.cdUp();
    pluginDir.cdUp();
    pluginDir.cdUp();
#endif
    QStringList p;
    p<<pluginDir.canonicalPath()+"/plugins";
//    p<<pluginDir.canonicalPath()+"/plugins/script";
    //    a.setLibraryPaths(p);
    //    qDebug() << p;
    a.addLibraryPath(pluginDir.canonicalPath()+"/plugins");
    a.addLibraryPath("/opt/local/share/qt4/plugins");

    Core* c = Core::instance();
//    c->loadPlugin("SpectrumBand");
    c->loadPlugin("T2C");
//    c->loadPlugin("UF2Device");
    QScriptEngine *e = c->engine();
    qDebug() << e->availableExtensions();

    QScriptEngineDebugger *dbg = new QScriptEngineDebugger;
    dbg->attachTo(e);
//    qDebug() << QDir::currentPath() + "/JS/main.js";
    Core::loadFile(QDir::currentPath() + "/JS/main.js", e);
 //    e->evaluate("var r=new UF2; r.deviceId = 1811; r.openDevice(\"ttyUSB0\", 115200); var s = r.query({cmd:[15,1]}); debugger");
    return a.exec();
}
