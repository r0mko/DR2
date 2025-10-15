#include "linearinstance.h"

#include <QPainter>

GPen LinearInstanceHandle::pen() const
{
    chkItem(GPen());
    return item()->pen;
}

void LinearInstanceHandle::setPen(GPen pen)
{
    chkItem();
    item()->pen = pen;
    item()->update();
}

qreal LinearInstanceHandle::length() const
{
    chkItem(0);
    qreal r{0};
    for( auto it = item()->m_data.constBegin(); it != item()->m_data.constEnd(); ++it )
        r += (*it).length;
    return r;
}

VisualInstance *LinearInstanceHandle::createItem(QGraphicsItem *parent) const
{
    return new LinearInstance(parent);
}


QRectF LinearInstance::boundingRect() const
{
    QPolygonF polygon;
    for( auto it = m_data.constBegin(); it != m_data.constEnd(); ++it ) {
        if( it == m_data.constBegin() ) polygon << it->pt;
        else polygon << (it->pt * it->length);
    }
    return polygon.boundingRect();
}

void LinearInstance::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(pen);
    QPolygonF polygon;
    for( auto it = m_data.constBegin(); it != m_data.constEnd(); ++it ) {
        if( it == m_data.constBegin() ) polygon << it->pt;
        else polygon << (it->pt * it->length);
    }
    painter->drawPolyline(polygon);
}
