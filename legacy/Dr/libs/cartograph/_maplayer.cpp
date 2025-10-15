#include "maplayer.h"
#include "mapview.h"
#include "geometry.h"
#include <QDebug>

#include "qsgnode.h"
MapLayer::MapLayer(QQuickItem *parent) :
    QQuickItem(parent)
{
    setFlag(QQuickItem::ItemHasContents);
}

QSGNode *MapLayer::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *data)
{
    Q_UNUSED(data)
    if (!node) {
        node = new QSGNode;
    }
//    qDebug() << "update paint node MapLayer";
    node->removeAllChildNodes();
    for (Geometry *n:m_nodes) {
        if (n->buffersComplete() && !n->hidden()) {
            QSGNode *nd = n->node();
//            qDebug() << "add node" << nd;
            node->appendChildNode(nd);
        }
    }
    return node;
}

void MapLayer::componentComplete()
{
    QQuickItem::componentComplete();
}

MapView *MapLayer::view() const
{
    return m_view;
}

void MapLayer::setView(MapView *arg)
{
    if (m_view == arg)
        return;
    if (m_view)
        m_view->disconnect(0, this, 0);
    m_view = arg;
    connect(m_view, &MapView::viewReadyChanged, this, &MapLayer::update);
//    connect(m_view, &MapView::viewReady, this, &MapLayer::onViewReady);
    emit viewChanged();

}

uint MapLayer::targetSRID() const
{
    if (!m_view)
        return 0;
    return m_view->targetSRID();
}

SpatiaLiteDB *MapLayer::database() const
{
    if (!m_view)
        return nullptr;
    return m_view->database();
}

int MapLayer::sourceSRID() const
{
    return m_sourceSRID;
}

void MapLayer::setSourceSRID(int sourceSRID)
{
    if (m_sourceSRID == sourceSRID)
        return;

    m_sourceSRID = sourceSRID;
    emit sourceSRIDChanged(sourceSRID);
}

//QQuickShaderEffectSource *MapLayer::shaderEffectSource() const
//{
//    return m_shaderEffectSource;
//}


//void MapLayer::checkGeometry(Geometry* geom)
//{
//    if (geom->complete()){
//        update();
//    }else {
//        connect(geom, SIGNAL(completed()), this, SLOT(update()));
//    }
//}

//void MapLayer::onViewPortRectChanged(QRectF r)
//{
//    m_shaderEffectSource->setSourceRect( r );
//    for( QQuickItem* i : m_effects ){
//        i->setX( r.x() );
//        i->setY( r.y() );
//        i->setWidth( r.width() / m_shaderEffectSource->scale() );
//        i->setHeight( r.height() / m_shaderEffectSource->scale() );
//    }
//}

//void MapLayer::onViewPortScaleChanged(qreal s)
//{
//    m_shaderEffectSource->setScale( s );
//    for( QQuickItem* i : m_effects ){
//        i->setScale( s );
//    }
//}

//void MapLayer::onViewReady()
//{
    // TODO: consider changing that
//    if( m_view->scene() != nullptr ){
//        uint si = m_view->scene()->metaObject()->indexOfSignal( "viewportRectChanged(QRectF)" );
//        uint sl = this->metaObject()->indexOfSlot( "onViewPortRectChanged(QRectF)" );
//        QMetaObject::connect( m_view->scene(), si, this, sl );
////        m_shaderEffectSource->setSourceRect( m_view->scene()->metaObject()->property( m_view->scene()->metaObject()->indexOfProperty( "viewportRect" ) ).read( m_view->scene() ).toRectF() );

//        si = m_view->scene()->metaObject()->indexOfSignal( "rendererScaleChanged(qreal)" );
//        sl = this->metaObject()->indexOfSlot( "onViewPortScaleChanged(qreal)" );
//        QMetaObject::connect( m_view->scene(), si, this, sl );
//    }
//    uint re = m_view->scene()->metaObject()->indexOfProperty( "viewportRect" );
//    QRectF rect = m_view->scene()->metaObject()->property( re ).read( m_view->scene() ).toRectF();
//    m_view->update();
//    update();
//    m_shaderEffectSource->update();

//    for( QQuickItem* i: m_effects ){
//        i->setParentItem( m_view );
//        i->setScale( m_shaderEffectSource->scale() );
//        i->setX( rect.x() );
//        i->setY( rect.y() );
//        i->setWidth( rect.width() / m_shaderEffectSource->scale() );
//        i->setHeight( rect.height() / m_shaderEffectSource->scale() );
//        qDebug() << "effect's parent item" << i->parentItem();
//        i->update();
//        qDebug() << "view ready | item size" << i->width() << i->height();
//    }
//}

void MapLayer::addGeometry(Geometry *g)
{
    g->setLayer(this);
    m_nodes.append(g);
    update();
    connect(g, SIGNAL(completed()), this, SLOT(update()));
    emit dataChanged();
}

