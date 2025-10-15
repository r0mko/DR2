#include "pathitem.h"
#include <QPainter>
#include <QPen>

void PathItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    Q_UNUSED(option)
    switch (shapeType) {
    case ::ShapeInstanceHandle::Polygonal:
        painter->setPen(pen);
        painter->setBrush(brush);
//        painter->fillPath(path, brush);
        painter->drawPath(path);
        break;
    case ::ShapeInstanceHandle::Linear:
        painter->setPen(pen);
        painter->drawPath(path);
        break;
    default:
        break;
    }
}

QRectF PathItem::boundingRect() const
{
    return path.boundingRect();
    //    return m_boundingRect;
}

QPainterPath PathItem::shape() const
{
    return path;

}

int PathItem::type() const
{
    return SELF_PTR_TYPEID;

}

void PathItemHandle::setBlob(QByteArray ba) {
    QPainterPath p;
    QDataStream ds( &ba, QIODevice::ReadOnly);
    ds >> p;
    setPath( p );
}

QPainterPath PathItemHandle::path() const
{
    chkItem(QPainterPath());
    return item()->path;
}

void PathItemHandle::setPath(QPainterPath path)
{
    chkItem();
    item()->path = path;
    item()->m_boundingRect = path.boundingRect();
    item()->prepareGeometryChange();
    item()->update();
}

Qt::FillRule PathItemHandle::fillRule() const
{
    chkItem(Qt::OddEvenFill);
    return item()->path.fillRule();
}

void PathItemHandle::setFillRule(Qt::FillRule fillRule)
{
    chkItem();
    item()->path.setFillRule(fillRule);
}


