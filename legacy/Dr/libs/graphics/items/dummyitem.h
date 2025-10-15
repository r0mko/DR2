#ifndef DUMMYLIGHTITEM_H
#define DUMMYLIGHTITEM_H

#include "visualinstance.h"
#include "gpen.h"
class DummyItem: public VisualInstance
{
    GR_INSTANCE(DummyItem)
public:
    DummyItem(QGraphicsItem * parent = nullptr) : VisualInstance( parent ) {}
    virtual ~DummyItem() {}

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public slots:

private:
    GPen m_pen;
    QColor color;
};

class DummyItemHandle: public VisualInstanceHandle
{
    Q_GADGET
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(GPen pen READ pen WRITE setPen)


public:
    using VisualInstanceHandle::VisualInstanceHandle;
    QColor color() const;
    void setColor(QColor color);
    GPen pen() const;
    void setPen(GPen pen);



protected:
    VisualInstance *createItem(QGraphicsItem *parent) const;
};

DECLARE_ITEM(DummyItem)

#endif // DUMMYLIGHTITEM_H
