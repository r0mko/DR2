#ifndef POINTITEM_H
#define POINTITEM_H

#include "shapeinstance.h"


class PointItem : public ShapeInstance
{
    GR_INSTANCE(PointItem)
    QPointF pt;
public:
    using ShapeInstance::ShapeInstance;

    // QGraphicsItem interface
public:
    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};


class PointItemHandle : public ShapeInstanceHandle
{
    Q_GADGET
    GR_HANDLE(PointItem)
    Q_PROPERTY(QPointF point READ point WRITE setPoint)
    QPointF m_point;

public:
    using ShapeInstanceHandle::ShapeInstanceHandle;

    QPointF point() const;
    void setPoint(QPointF point);
};
DECLARE_ITEM(PointItem)

#endif // POINTITEM_H
