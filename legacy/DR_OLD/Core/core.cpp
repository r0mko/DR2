#include "core.h"
#include <QEvent>
#include <QDebug>
#include "plugininterface.h"
#include "resourcemanager.h"
#include <iostream>

Core* Core::self = 0;
//ResourceManager Core::r = ResourceManager(Core::instance());

bool Core::loadFile(QString fileName, QScriptEngine *engine)
{
    // avoid loading files more than once
    static QSet<QString> loadedFiles;
    QFileInfo fileInfo(fileName);
    QString absoluteFileName = fileInfo.absoluteFilePath();
    QString absolutePath = fileInfo.absolutePath();
    QString canonicalFileName = fileInfo.canonicalFilePath();
    if (loadedFiles.contains(canonicalFileName)) {
        return true;
    }
    loadedFiles.insert(canonicalFileName);
    QString path = fileInfo.path();

    // load the file
    QFile file(fileName);
    if (file.open(QFile::ReadOnly)) {
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        QString contents = stream.readAll();
        file.close();

        int endlineIndex = contents.indexOf('\n');
        QString line = contents.left(endlineIndex);
        int lineNumber = 1;

        // strip off #!/usr/bin/env qscript line
        if (line.startsWith("#!")) {
            contents.remove(0, endlineIndex+1);
            ++lineNumber;
        }

        // set qt.script.absoluteFilePath
        QScriptValue script = engine->globalObject().property("core").property("js");
        QScriptValue oldFilePathValue = script.property("absoluteFilePath");
        QScriptValue oldPathValue = script.property("absolutePath");
        script.setProperty("absoluteFilePath", engine->toScriptValue(absoluteFileName));
        script.setProperty("absolutePath", engine->toScriptValue(absolutePath));

        QScriptValue r = engine->evaluate(contents, fileName, lineNumber);
        if (engine->hasUncaughtException()) {
            QStringList backtrace = engine->uncaughtExceptionBacktrace();
            qDebug() << QString("    %1\n%2\n\n").arg(r.toString()).arg(backtrace.join("\n"));
            return true;
        }
        script.setProperty("absoluteFilePath", oldFilePathValue); // if we come from includeScript(), or whereever
        script.setProperty("absolutePath", oldPathValue); // if we come from includeScript(), or whereever
    } else {
        return false;
    }
    return true;
}

QScriptValue Core::includeScript(QScriptContext *context, QScriptEngine *engine)
{
    QString currentFileName = engine->globalObject().property("core").property("js").property("absoluteFilePath").toString();
    QFileInfo currentFileInfo(currentFileName);
    QString path = currentFileInfo.path();
    QString importFile = context->argument(0).toString();
    QFileInfo importInfo(importFile);
    if (importInfo.isRelative()) {
        importFile =  path + "/" + importInfo.filePath();
    }
    if (!Core::loadFile(importFile, engine)) {
        return context->throwError(QString("Failed to resolve include: %1").arg(importFile));
    }
    return engine->toScriptValue(true);
}

QScriptValue Core::importExtension(QScriptContext *context, QScriptEngine *engine)
{
    return engine->importExtension(context->argument(0).toString());
}

QScriptValue Core::consoleLog(QScriptContext *context, QScriptEngine *engine)
{
    Q_UNUSED(engine)
    QByteArray codec = context->thisObject().property("encoding").toString().toAscii();
    QTextCodec* c = QTextCodec::codecForName(codec);
    for(int i=0; i<context->argumentCount(); ++i) {
        QByteArray message = c->fromUnicode(context->argument(i).toString());
        std::cout << message.data() << " " << std::flush;
    }
    std::cout << std::endl;
    return QScriptValue::NullValue;
}



Core::Core() : r(this)
{
    p.coreid = qrand();
    jsengine = new QScriptEngine(this);
    initJavaScript(jsengine);
    qRegisterMetaType<RUID>();
    if(self) {
        qDebug() << "Several instances of Core not allowed";
    } else {
        self = this;
    }
}

int Core::loadPlugin(const QString &name)
{
    QDir pluginsDir(QCoreApplication::applicationDirPath());
    QString fileName;
#if defined(Q_OS_WIN)
    fileName = name + ".dll"
        #elif defined(Q_OS_MAC)

    fileName = "lib"+name+".dylib";
    if(pluginsDir.dirName().contains("MacOS"))
    {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }

#elif defined(Q_OS_LINUX)
    fileName = "lib"+name+".so";
#endif

    pluginsDir.cd("plugins");
    QPluginLoader *pluginLoader = new QPluginLoader(pluginsDir.absoluteFilePath(fileName));
    QObject *plugin = pluginLoader->instance();
    //    qDebug() << pluginsDir.absoluteFilePath(fileName);
    if(plugin) {
        Plugin* plugin = qobject_cast<Plugin*>(pluginLoader->instance());
        PluginInfo info;
        info.ptr = plugin;
        info.loader = pluginLoader;
        plugin->init(this);
        p.registeredPlugins.insert(name, p.plugins.size());
        p.plugins.append(info);
        return p.plugins.size()-1;
    }
    else {
        qDebug() <<"Error loading" << name << ":" << pluginLoader->errorString();
        return -1;
    }
}

qint64 Core::currentMsec()
{
    return QDateTime::currentMSecsSinceEpoch();
}

bool Core::event(QEvent *e)
{
    qDebug() << (int)e->type();
    return true;
}

Plugin *Core::getPluginInstance(const QString &name)
{
    Plugin* ptr = 0;
    if(p.registeredPlugins.contains(name)) {
        ptr = p.plugins[p.registeredPlugins.value(name)].ptr;
    }
    return ptr;
}


int Core::registerEvent(const QString &name)
{
    if(p.registeredEvents.contains(name))
        return p.registeredEvents[name];
    else
    {
        int t = QEvent::registerEventType();
        qDebug() << "Registered event" << name << "as" << t;
        p.registeredEvents.insert(name, t);
        return t;
    }
}

OUID Core::registerObject(QObject *object)
{
    OUID id;
    id.coreId = p.coreid;
    id.oId = objects.size();
    objects.append(object);
    return id;
}

RUID Core::createResource(int type, void *copy)
{
    return r.allocateResource(type, copy);
}

RUID Core::newResource(int type, void *copy)
{
    return instance()->createResource(type, copy);
}

void Core::log(int loglevel, const QObject *sender, const QString &message)
{
    Q_UNUSED(loglevel);
    QString name;
    if(sender->objectName().isEmpty())
        name = sender->metaObject()->className();
    else
        name = sender->objectName();
    qDebug() << name << message;


}

void Core::initJavaScript(QScriptEngine *engine)
{

    QScriptValue global = engine->globalObject();
    // add the qt object
    global.setProperty("console", engine->newObject());
    QScriptValue console = global.property("console");
    console.setProperty("encoding", "UTF-8");
    console.setProperty("log", engine->newFunction(Core::consoleLog));

    global.setProperty("core", engine->newObject());
    // add a 'script' object
    QScriptValue script = engine->newObject();
    script.setProperty("availableExtensions", engine->toScriptValue(engine->availableExtensions()));
    global.property("core").setProperty("js", script);
    // add a 'system' object
    QScriptValue system = engine->newObject();
    global.property("core").setProperty("system", system);

    // add os information to core.system.os
#ifdef Q_OS_WIN32
    QScriptValue osName = engine->toScriptValue(QString("Windows"));
#elif defined(Q_OS_LINUX)
    QScriptValue osName = engine->toScriptValue(QString("Linux"));
#elif defined(Q_OS_MAC)
    QScriptValue osName = engine->toScriptValue(QString("Mac OS X"));
#elif defined(Q_OS_UNIX)
    QScriptValue osName = engine->toScriptValue(QString("UNIX"));
#endif
    system.setProperty("os", osName);
    // add environment variables to core.system.env
    QMap<QString,QVariant> envMap;
    QStringList envList = QProcess::systemEnvironment();
    foreach (const QString &entry, envList) {
        QStringList keyVal = entry.split('=');
        if (keyVal.size() == 1)
            envMap.insert(keyVal.at(0), QString());
        else
            envMap.insert(keyVal.at(0), keyVal.at(1));
    }
    system.setProperty("env", engine->toScriptValue(envMap));

    // add the include functionality to qt.script.include
    script.setProperty("include", engine->newFunction(Core::includeScript));
    // add the importExtension functionality to qt.script.importExtension
    script.setProperty("importExtension", engine->newFunction(Core::importExtension));

    QStringList args = QCoreApplication::arguments();
    args.takeFirst();
    bool customfile = false;
    QString customFileName;
    QScriptValue argsQs = engine->newArray();
    for(int i=0; i<args.size(); ++i) {
        argsQs.setProperty(i, QScriptValue(args.at(i)));
    }
    global.property("core").setProperty("argv", argsQs);
}

RUID Core::addResource(const QVariant &res)
{
    RUID id = newRsc(res.userType(), res.data());
    qDebug() << "Resource type" << res.userType() << "added" << res;
    return id;
}


//QScriptValue Plugin::bindToScriptEngine(QScriptEngine *e, const QString &name)
//{
//    QScriptValue jsObject = e->newQObject(this);
//    const QMetaObject* tmo = this->metaObject();
//    for(int i=0; i<tmo->methodCount(); ++i)
//    {
//        QMetaMethod m = tmo->method(i);
//        QScriptValue method = e->newFunction(jsCallMethod);
//        QScriptValue data = e->newArray();
//        int count = 0;
//        data.setProperty(count++, (double)(quintptr)o);
//        data.setProperty(count++, i);
//        QList<QByteArray> parTypes = m.parameterTypes();
//#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
//        data.setProperty(count++, m.returnType());
//        int parameterCount = m.parameterCount();
//        QString methodName = QString(tmo->method(i).name());
//#else
//        data.setProperty(count++, QMetaType::type(m.typeName()));
//        int parameterCount = parTypes.size();
//        QByteArray ba(m.signature());
//        ba.truncate(ba.indexOf("("));
//        QString methodName = ba;

//#endif
//        for(int j=0; j<parameterCount; ++j) {
//            data.setProperty(count++, QMetaType::type(parTypes.at(j)));
//        }
//        method.setData(data);
//        jsObject.setProperty(methodName, method);
//    }
//    e->globalObject().setProperty(name, jsObject);
//}

// создаёт привязку QObject в ScriptEngine (вызов методов с возможностью передавать указатели на ресурсы)
//void createJSBinding(const QString& name, QScriptEngine* e, QObject *o)
//{
//    QScriptValue jsObject = e->newQObject(o);
//    const QMetaObject* tmo = o->metaObject();
//    for(int i=0; i<tmo->methodCount(); ++i)
//    {
//        QMetaMethod m = tmo->method(i);
//        QScriptValue method = e->newFunction(jsCallMethod);
//        QScriptValue data = e->newArray();
//        int count = 0;
//        data.setProperty(count++, (double)(quintptr)o);
//        data.setProperty(count++, i);
//        QList<QByteArray> parTypes = m.parameterTypes();
//#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
//        data.setProperty(count++, m.returnType());
//        int parameterCount = m.parameterCount();
//        QString methodName = QString(tmo->method(i).name());
//#else
//        data.setProperty(count++, QMetaType::type(m.typeName()));
//        int parameterCount = parTypes.size();
//        QByteArray ba(m.signature());
//        ba.truncate(ba.indexOf("("));
//        QString methodName = ba;

//#endif
//        for(int j=0; j<parameterCount; ++j) {
//            data.setProperty(count++, QMetaType::type(parTypes.at(j)));
//        }
//        method.setData(data);
//        jsObject.setProperty(methodName, method);
//    }
//    e->globalObject().setProperty(name, jsObject);
//}
