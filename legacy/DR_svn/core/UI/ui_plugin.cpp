#include <QDebug>
#include <qqml.h>

#include "ui_plugin.h"
#include "ui.h"
#include "symbol.h"
#include "unidelegate.h"
#include "uniloader.h"
#include "targetdef.h"
#include "line.h"

static QObject *uiSingletontypeProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(scriptEngine)
    Q_UNUSED(engine)
    Q_ASSERT(UIHelper::qmlSingleton == nullptr);

    UIHelper::qmlSingleton = new UIHelper;
    return UIHelper::qmlSingleton;
}

void UIPlugin::registerTypes(const char *uri)
{
    // @uri DataReactor.UI
//    qmlRegisterType<MyItem>(uri, 1, 0, "MyItem");
    qmlRegisterSingletonType<UIHelper>(uri, 1, 0, "UI", uiSingletontypeProvider);
//    qmlRegisterSingletonType(QUrl::fromLocalFile("/Theme/Theme.qml"), uri, 1, 0, "Theme");
    qmlRegisterUncreatableType<UIHelper>(uri, 1, 0, "UI", "UI is a singleton type and can not be instantiated");
    qmlRegisterUncreatableType<Entypo>(uri, 1, 0, "Entypo", "Entypo cannot be instantiated");
    qmlRegisterUncreatableType<FontAwesome>(uri, 1, 0, "FontAwesome", "FontAwesome cannot be instantiated");
    qmlRegisterType<Symbol>(uri, 1, 0, "Symbol");
    qmlRegisterType<TargetDef>(uri, 1, 0, "Target");
    qmlRegisterType<TypeDelegate>(uri, 1, 0, "Delegate");
    qmlRegisterType<UniLoader>(uri, 1, 0, "UniLoader");
    qmlRegisterType<UniDelegate>(uri, 1, 0, "UniDelegate");
    qmlRegisterType<Line>(uri, 1, 0, "Line");
}

void UIPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(uri)
    UIHelper::engine = engine;
}


