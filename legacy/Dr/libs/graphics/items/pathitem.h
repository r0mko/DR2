#ifndef PATHITEM_H
#define PATHITEM_H

#include "shapeinstance.h"

class PathItem : public ShapeInstance
{
    GR_INSTANCE(PathItem)
    QPainterPath path;
    QRectF m_boundingRect;
public:
    using ShapeInstance::ShapeInstance;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
    QRectF boundingRect() const;
    QPainterPath shape() const;
    int type() const;
};


class PathItemHandle : public ShapeInstanceHandle
{
    Q_GADGET
    Q_PROPERTY(QPainterPath path READ path WRITE setPath)
    Q_PROPERTY(Qt::FillRule fillRule READ fillRule WRITE setFillRule)
    GR_HANDLE(PathItem)

public:
    using ShapeInstanceHandle::ShapeInstanceHandle;

    Q_INVOKABLE void setBlob(QByteArray ba);

    QPainterPath path() const;
    void setPath(QPainterPath path);

    Qt::FillRule fillRule() const;
    void setFillRule(Qt::FillRule fillRule);
};

DECLARE_ITEM(PathItem)

#endif // PATHITEM_H
