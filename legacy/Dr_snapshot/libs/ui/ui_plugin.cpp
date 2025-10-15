#include <QDebug>
#include <qqml.h>

#include "ui_plugin.h"
#include "ui.h"
#include "symbol.h"
#include "unidelegate.h"
#include "uniloader.h"
#include "targetdef.h"
#include "drpicture.h"

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
    // @uri Dr.UI
//    qmlRegisterType<MyItem>(uri, 1, 0, "MyItem");
    qmlRegisterSingletonType<UIHelper>(uri, 1, 0, "UI", uiSingletontypeProvider);
//    qmlRegisterSingletonType(QUrl::fromLocalFile("/Theme/Theme.qml"), uri, 1, 0, "Theme");
    qmlRegisterUncreatableType<UIHelper>(uri, 1, 0, "UI", "UI is a singleton type and can not be instantiated");
    qmlRegisterUncreatableType<Entypo>(uri, 1, 0, "Entypo", "Entypo is not creatable");
    qmlRegisterUncreatableType<FontAwesome>(uri, 1, 0, "FontAwesome", "FontAwesome is not creatable");
    qmlRegisterUncreatableType<ModernUI>(uri, 1, 0, "ModernUI", "ModernUI is not creatable");
    qmlRegisterType<Symbol>(uri, 1, 0, "Symbol");
    qmlRegisterType<TargetDef>(uri, 1, 0, "Target");
    qmlRegisterType<TypeDelegate>(uri, 1, 0, "Delegate");
    qmlRegisterType<UniLoader>(uri, 1, 0, "UniLoader");
    qmlRegisterType<UniDelegate>(uri, 1, 0, "UniDelegate");
    qmlRegisterType<DRPicture>(uri, 1, 0, "DRPicture");


}

void UIPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(uri)
    UIHelper::engine = engine;
}

void rasterizeQPicture(QPainter *painter, const QSizeF &requestedSize, const QPicture &picture, QColor colorize, bool antialiasing, Qt::AspectRatioMode aspectRatio, qreal adjustX, qreal adjustY)
{
    const QSizeF srcSize = QSizeF(picture.boundingRect().size());
    QSizeF trueSize = srcSize.scaled(requestedSize, aspectRatio);
    qreal hmargin = (requestedSize.width() - trueSize.width()) / 2;
    qreal vmargin = (requestedSize.height() - trueSize.height()) / 2;
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing, antialiasing);
    painter->translate(hmargin, vmargin);
    painter->translate(adjustX / 64, adjustY / 64);
    painter->scale(trueSize.width() / srcSize.width(), trueSize.height() / srcSize.height());
    painter->drawPicture(0, 0, picture);
    painter->restore();
    if (colorize.isValid()) {
        painter->setCompositionMode(QPainter::CompositionMode_SourceIn);
        painter->fillRect(0, 0, requestedSize.width(), requestedSize.height(), colorize);
    }
}
