#include "t2c_plugin.h"
#include <qqml.h>
#include <t2cmanager.h>
#include <t2cmanager2.h>
//#include <t2cworker.h>
//#include <t2cworker2.h>

void T2CPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<T2CManager>(uri, 1, 0, "T2CManager");
    qmlRegisterType<T2CManager2>(uri, 1, 1, "T2CManager");
//    qmlRegisterType<T2CWorker>(uri, 1, 0, "T2CWorker");
//    qmlRegisterType<T2CWorker2>(uri, 1, 1, "T2CWorker");
}

void T2CPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(engine)
    Q_UNUSED(uri)
}
