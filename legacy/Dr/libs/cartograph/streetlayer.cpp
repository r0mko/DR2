#include "streetlayer.h"
#include <QPainter>
#include "spatialitedb.h"

#include <QPen>
#include <QBrush>
#include <QDebug>
void StreetLayer::qmlSnipFilterListAppend(QQmlListProperty<SnipFilter> *list, SnipFilter *object)
{
    StreetLayer *o = qobject_cast<StreetLayer*>(list->object);

    o->m_snipFilterList.append(object);
    object->setStreetLayer(o);
    emit o->snipFilterListChanged();
}

int StreetLayer::qmlSnipFilterListCount(QQmlListProperty<SnipFilter> *list)
{
    StreetLayer *o = qobject_cast<StreetLayer*>(list->object);
    return o->m_snipFilterList.size();
}

SnipFilter *StreetLayer::qmlSnipFilterListAt(QQmlListProperty<SnipFilter> *list, int index)
{
    StreetLayer *o = qobject_cast<StreetLayer*>(list->object);
    return o->m_snipFilterList.at(index);
}

void StreetLayer::readDB()
{
    if( m_mapView == nullptr ) return;

    for( SnipFilter* sf : m_snipFilterList ){
        sf->setFont( m_font );
        auto result = m_mapView->database()->query( QString( "SELECT Transform(LineMerge(Collect(`geom`)), 3752) as geometry, `rd_name` as name FROM `trline` WHERE `obj_cod` < 999000 AND `snip_ad` IN ( '%1' ) GROUP BY name;" ).arg( sf->snipTypeList().join( "','" ) ) );
        for( const QVariant& row : result ){
            auto r = row.toMap();
            auto path = m_mapView->blobToPath( r[ "geometry" ].toByteArray() );
            auto name = r[ "name" ].toString();
            sf->addStreet( path, name );
        }
    }
}

QQmlListProperty<SnipFilter> StreetLayer::qmlSnipFilterList() {
    return QQmlListProperty<SnipFilter>(this, 0,
                                        &StreetLayer::qmlSnipFilterListAppend,
                                        &StreetLayer::qmlSnipFilterListCount,
                                        &StreetLayer::qmlSnipFilterListAt,
                                        QQmlListProperty<SnipFilter>::ClearFunction());
}

StreetLayer::StreetLayer(QQuickItem *parent) :
    QQuickPaintedItem(parent),
    m_scene( nullptr ),
    m_mapView( nullptr )
{
    setTransformOrigin(QQuickItem::TopLeft);
    setRenderTarget(QQuickPaintedItem::FramebufferObject);
    setPerformanceHint(QQuickPaintedItem::FastFBOResizing);
    setAntialiasing(true);
}

void StreetLayer::paint(QPainter *painter)
{
    if (!m_scene)
        return;
    painter->setBrush( m_color );
    painter->setPen( Qt::NoPen );
    for( SnipFilter* sf : m_snipFilterList ){
        sf->paint(painter, m_viewTransform, m_viewPortRect);
    }

}

void StreetLayer::componentComplete()
{
}

QFont StreetLayer::font() const
{
    return m_font;
}

MapView *StreetLayer::mapView() const
{
    return m_mapView;
}

void StreetLayer::setFont(QFont arg)
{
    if (m_font == arg)
        return;

    m_font = arg;
    emit fontChanged(arg);
}

void StreetLayer::setMapView(MapView *arg)
{
    if (m_mapView == arg)
        return;

    m_mapView = arg;
    emit mapViewChanged(arg);
}

void StreetLayer::setScene(QQuickItem *arg)
{
    if (m_scene == arg)
        return;
    if (m_scene) {
        m_scene->disconnect(0, this, 0);
    }
    m_scene = arg;
    connect(m_scene, SIGNAL(viewTransformChanged()), this, SLOT(updateLabels()));
    emit sceneChanged();
}

void StreetLayer::initPaths()
{
    //clean all
    for( SnipFilter* sf : m_snipFilterList )
        sf->clearData();
    readDB();
}

void StreetLayer::updateLabels()
{
    m_viewTransform = m_scene->property( "viewTransform" ).value<QTransform>();
    m_viewPortRect = m_scene->property( "viewportRect" ).value<QRectF>();
    emit layoutChanged();
    update();
}

void StreetLayer::setColor(QColor arg)
{
    if (m_color == arg)
        return;

    m_color = arg;
    emit colorChanged(arg);
}
QRectF StreetLayer::viewPortRect() const
{
    return m_viewPortRect;
}

QColor StreetLayer::color() const
{
    return m_color;
}


QQuickItem *StreetLayer::scene() const
{
    return m_scene;
}


