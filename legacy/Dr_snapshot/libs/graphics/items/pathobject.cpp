#include "pathobject.h"

#include <QPainter>

PathObject::PathObject(QGraphicsItem *parent) : GraphicsObject(parent)
{

}

QPainterPath PathObject::path() const
{
    return m_path;
}

PathObject::ShapeType PathObject::shapeType() const
{
    return m_shapeType;
}

GPen PathObject::pen() const
{
    return m_pen;
}

GBrush PathObject::brush() const
{
    return m_brush;
}

Qt::FillRule PathObject::fillRule() const
{
    return m_fillRule;
}

QPainterPath PathObject::shape() const
{return m_path;

}

QRectF PathObject::boundingRect() const
{
    return m_path.boundingRect().marginsAdded(QMarginsF(m_pen.width() / 2, m_pen.width() / 2 , m_pen.width() / 2 , m_pen.width() / 2));
}

void PathObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    preparePainter(painter);
    switch (m_shapeType) {
    case Polygonal:
        painter->setPen(m_pen);
        painter->setBrush(m_brush);
//        painter->fillPath(path, brush);
        painter->drawPath(m_path);
        break;
    case Linear:
        painter->setPen(m_pen);
        painter->drawPath(m_path);
        break;
    default:
        break;
    }

}

void PathObject::setPath(QPainterPath path)
{
    if (m_path == path)
        return;

    m_path = path;
    update();
    emit pathChanged(path);
}

void PathObject::setShapeType(PathObject::ShapeType shapeType)
{
    if (m_shapeType == shapeType)
        return;

    m_shapeType = shapeType;
    update();
    emit shapeTypeChanged(shapeType);
}

void PathObject::setPen(GPen pen)
{
    if (m_pen == pen)
        return;

    m_pen = pen;
    update();
    emit penChanged(pen);
}

void PathObject::setBrush(GBrush brush)
{
    if (m_brush == brush)
        return;

    m_brush = brush;
    update();
    emit brushChanged(brush);
}

void PathObject::setFillRule(Qt::FillRule fillRule)
{
    if (m_fillRule == fillRule)
        return;

    m_fillRule = fillRule;
    update();
    emit fillRuleChanged(fillRule);
}

void PathObject::setBlob(QByteArray ba) {
    QPainterPath p;
    QDataStream ds( &ba, QIODevice::ReadOnly);
    ds >> p;
    setPath( p );
}

