#include "t2c_plugin.h"
#include <qqml.h>
#include "t2cmanager.h"
#include "t2cmanager2.h"
#include "processvariable.h"
void T2CPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<T2CManager>(uri, 1, 0, "T2CManager");
    qRegisterMetaType<ProcessVariable>();
    qmlRegisterUncreatableType<ProcessVariable>(uri, 1, 0, "ProcessVariable", QString("ProcessVariable is not a QML element"));

    qmlRegisterType<T2CManager2>(uri, 1, 1, "T2CManager");
}

void T2CPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(engine)
    Q_UNUSED(uri)
}
