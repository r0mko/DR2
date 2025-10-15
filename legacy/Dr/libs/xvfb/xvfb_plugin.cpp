#include "xvfb_plugin.h"
#include <qqml.h>
#include "xvfbitem.h"
#include "xvfbrenderer.h"

void XvfbPlugin::registerTypes(const char *uri)
{
//    qmlRegisterType<XvfbItem>(uri, 1, 0, "XvfbItem");
    qmlRegisterType<XvfbRenderer>(uri, 1, 0, "XvfbRenderer");
}

void XvfbPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(engine)
    Q_UNUSED(uri)
}
