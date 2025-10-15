#include "testplugin.h"
#include <QDebug>
#include <QVariant>

int TestPlugin::init(QObject *core)
{
    qDebug() << "Testplugin initialized" << core->metaObject()->className();
    Core* c = (Core*)core;
    c->log(0, this, "Called core method");
    c->addResource(QVariant("Test resource"));
    return 0;
}

QVariantHash TestPlugin::getInfo()
{
    QVariantHash info;
    info.insert("Name", "Test plugin");
    return info;
}

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
Q_EXPORT_PLUGIN2 ( TestPlugin, TestPlugin )
#endif
