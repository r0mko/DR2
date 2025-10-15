#include "core_plugin.h"
#include "core.h"
#include "metaobject.h"
#include "object.h"
#include <qqml.h>

void CorePlugin::registerTypes(const char *uri)
{
    // @uri Dr
    qmlRegisterType<DRObject>(uri, 1, 0, "Object");
    qmlRegisterType<MetaObject>(uri, 1, 0, "MetaObject");
    qmlRegisterUncreatableType<MetaEnum>(uri, 1, 0, "MetaEnum", "MetaEnum cannot be instantiated");
    qmlRegisterUncreatableType<MetaMethod>(uri, 1, 0, "MetaMethod", "MetaMethod cannot be instantiated");
    qmlRegisterUncreatableType<MetaProperty>(uri, 1, 0, "MetaProperty", "MetaProperty cannot be instantiated");
}


