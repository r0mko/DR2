#include "cartograph_plugin.h"
//#include "geometry.h"
#include "cosmeticstroke.h"
#include "linestroke.h"
#include "solidcolorfill.h"
#include "spatialitedb.h"
#include "spatialquery.h"
#include "stylepreviewitem.h"
//#include "maplayer.h"
#include <QPluginLoader>
#include <QQmlEngine>
#include <qqml.h>
#include "graphbuilder.h"
#include "graph.h"
#include "geolayer.h"
#include "geoview.h"
#include "smartobject.h"
#include "geostyle.h"
#include "geombuffer.h"
#include "style.h"
template <class T>
int registerStyle()
{
//    QMetaType::registerConverter<T, StyleNode>();
    return qRegisterMetaType<T>();
}

template<class... T> QVector<int> registerStyles() {
//    QMetaType::registerConverter
    return { registerStyle<T>()...};
}



void CartographPlugin::registerTypes(const char *uri)
{
    // @uri Dr.Cartograph
    qRegisterMetaType<QTransform>();

    QVector<int> styleTypes = registerStyles<
            StyleNode,
            Style,
            SolidColorFill,
            CosmeticStroke,
            LineStroke
            >();
    for (int id : styleTypes) {
        qDebug() << "adding style type" << QMetaType::typeName(id);
        GeoView::styleTypes.append(QMetaType::typeName(id));
    }
    qRegisterMetaTypeStreamOperators<QPainterPath>();

//    qmlRegisterType<SpatialQuery>();
    qRegisterMetaType<SpatialEntity>();
    qRegisterMetaType<SpatialLayer>();

    qmlRegisterType<SpatiaLiteDB>(uri, 1, 0, "SpatiaLiteDB");

    qmlRegisterType<SpatialQuery>(uri, 1, 0, "SpatialQuery");
//    qmlRegisterUncreatableType<StyleNode>(uri, 1, 0, "StyleNode", "StyleNode is abstract base");

    qmlRegisterType<StylePreviewItem>(uri, 1, 0, "StylePreviewItem");
    qmlRegisterType<GeoView>(uri, 1, 0, "GeoView");
    qmlRegisterType<GeoLayer>(uri, 1, 0, "GeoLayer");
    qmlRegisterType<SmartObject>(uri, 1, 0, "SmartObject");
    qmlRegisterType<GeomBuffer>(uri, 1, 0, "GeomBuffer");
}

void CartographPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{

//    qDebug() << "Initializing engine" << uri << engine->importPathList();
//    QString fullPath = engine->importPathList().last().append("/Dr/Graphics/libgraphics_debug.dylib");
//    QPluginLoader loader(fullPath);
//    if (loader.load()) {
//        qDebug() << loader.instance()->metaObject()->className() << "loaded!";
//        QQmlExtensionPlugin *plugin = static_cast<QQmlExtensionPlugin*>(loader.instance());
//        plugin->registerTypes(uri);
//        plugin->initializeEngine(engine, uri);
//    }
}

