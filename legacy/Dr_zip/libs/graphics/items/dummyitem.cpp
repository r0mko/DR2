#include "dummyitem.h"

#include <QPainter>

QRectF DummyItem::boundingRect() const
{
    return QRect(-50, -50, 100, 100 );
}

void DummyItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setBrush( color );
    painter->setPen( m_pen.pen() );
    painter->drawRect( -50, -50, 100, 100 );
}

QColor DummyItemHandle::color() const
{
    if (!item())
        return QColor();
    return static_cast<DummyItem*>(item())->color;
}

void DummyItemHandle::setColor(QColor color)
{
    if (!item())
        return;
    static_cast<DummyItem*>(item())->color = color;
    item()->update();
}

GPen DummyItemHandle::pen() const
{
    if (!item())
        return GPen();
    return static_cast<DummyItem*>(item())->m_pen;
}

void DummyItemHandle::setPen(GPen pen)
{
    if (!item())
        return;
    static_cast<DummyItem*>(item())->m_pen = pen;
    item()->update();
}




VisualInstance *DummyItemHandle::createItem(QGraphicsItem *parent) const {
    return new DummyItem(parent);
}
