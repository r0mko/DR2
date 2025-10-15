#ifndef LINEARINSTANCE_H
#define LINEARINSTANCE_H

#include "visualinstance.h"
#include "gpen.h"
#include "guidepath.h"

#include <QLinkedList>

class LinearInstance: public VisualInstance
{
    GR_INSTANCE(LinearInstance)

public:
    using VisualInstance::VisualInstance;
    LinearInstance(QGraphicsItem * parent = nullptr) : VisualInstance( parent ) {}
    virtual ~LinearInstance() {}

protected:
    GPen pen;
    QLinkedList<VectorNode> m_data;

    // QGraphicsItem interface
public:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

class LinearInstanceHandle: public VisualInstanceHandle
{
    Q_GADGET
    GR_HANDLE(LinearInstance)

    Q_PROPERTY(GPen pen READ pen WRITE setPen)
    Q_PROPERTY(qreal length READ length)


    public:
        using VisualInstanceHandle::VisualInstanceHandle;

    GPen pen() const;
    void setPen(GPen pen);


    qreal length() const;

};

DECLARE_ITEM(LinearInstance)


#endif // LINEARINSTANCE_H
