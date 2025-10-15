#include "pointitem.h"

#include <QPainter>

QRectF PointItem::boundingRect() const
{
    qreal s = currentViewTransform().m11();
    QRectF r(0, 0, pen.width() / s, pen.width() / s);
    r.moveCenter(pt);
    return r;
}

void PointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

    painter->setPen(pen);
    painter->drawPoint(pt);

}


QPointF PointItemHandle::point() const
{
    return item()->pt;
}

void PointItemHandle::setPoint(QPointF point)
{
    if (item()->pt == point)
        return;

    item()->pt = point;
    item()->update();
}
