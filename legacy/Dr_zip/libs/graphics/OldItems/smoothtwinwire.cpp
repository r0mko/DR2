#include "smoothtwinwire.h"
#include "scene.h"
#include "pen.h"
#include <QPainter>

SmoothTwinWire::SmoothTwinWire(SceneItem *parent) :
    SmoothWire(parent)
{

}

SmoothTwinWire::~SmoothTwinWire()
{
}

void SmoothTwinWire::paint(QPainter *painter)
{
    if (!pen())
        return;
    painter->setPen(pen()->pen());
    painter->drawPolyline(wire1);
    QPen pen2 = pen()->pen();
    pen2.setColor(m_auxColor);
    painter->drawPolyline(wire2);

}

QRectF SmoothTwinWire::getBoundingRect() const
{
    return wire1.boundingRect().united(wire2.boundingRect());
}

void SmoothTwinWire::build()
{
    if (!pen())
        return;
    qreal m_zoomFactor = 1 / scene()->viewportScale();
    if (guide()->vectorPath().isNull())
        return;
    if (radius() > 0)
        m_roundedPath = guide()->vectorPath().buildRounding(radius() * qSqrt(m_zoomFactor), precision(), beginMargin(), guide()->vectorPath().length() - endMargin());
    else
        m_roundedPath = guide()->vectorPath().getPart(beginMargin(), guide()->vectorPath().length() - endMargin());
    if (offset() > 0) {
        m_offsetPath = m_roundedPath.buildOffset(pen()->width() * offset() * m_zoomFactor);
    } else {
        m_offsetPath = m_roundedPath.buildOffset(pen()->width() * offset() * m_zoomFactor);
    }
    wire1 = m_offsetPath.toPolygon();
    m_offsetPath = m_roundedPath.buildOffset(pen()->width() * (offset() + m_spacing) * m_zoomFactor);

    item()->setBoundingRectDirty();
    update();
}

