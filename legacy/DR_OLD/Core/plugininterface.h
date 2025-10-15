#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H
#include <QObject>
#include <QtPlugin>
#include <QVariantHash>
#include <QtScript/QScriptEngine>

class Plugin : public QObject
{
public:
    virtual int init(QObject* core) = 0;
    virtual QVariantHash getInfo() = 0;
    virtual QObject* newInstance() = 0;
//    virtual QScriptValue bindToScriptEngine(QScriptEngine *e, const QString &name = QString());
};
Q_DECLARE_INTERFACE(Plugin, "com.r0mko.testlab.CorePlugin/1.0")

#endif // PLUGININTERFACE_H
