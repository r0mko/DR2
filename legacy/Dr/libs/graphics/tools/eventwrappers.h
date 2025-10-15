#ifndef GRAPHICSSCENEMOUSEEVENT_H
#define GRAPHICSSCENEMOUSEEVENT_H

#include <QObject>
#include "qgraphicssceneevent.h"

class GraphicsSceneMouseEvent
{
    Q_GADGET

    Q_PROPERTY(Qt::KeyboardModifiers modifiers READ modifiers CONSTANT)
    Q_PROPERTY(Qt::MouseButtons buttons READ buttons CONSTANT)
    Q_PROPERTY(Qt::MouseButton button READ button CONSTANT)
    Q_PROPERTY(QPointF pos READ pos CONSTANT)
    Q_PROPERTY(QPointF scenePos READ scenePos CONSTANT)
    Q_PROPERTY(QPointF screenPos READ screenPos CONSTANT)
    Q_PROPERTY(QPointF lastPos READ lastPos CONSTANT)
    Q_PROPERTY(QPointF lastScenePos READ lastScenePos CONSTANT)
    Q_PROPERTY(QPointF lastScreenPos READ lastScreenPos CONSTANT)
    Q_PROPERTY(bool accepted READ accepted WRITE setAccepted)

    QGraphicsSceneMouseEvent *e = nullptr;

public:
    GraphicsSceneMouseEvent(QGraphicsSceneMouseEvent *event = nullptr) : e(event) {}

    Qt::MouseButton button() const { return e->button(); }
    Qt::MouseButtons buttons() const { return e->buttons(); }
    Qt::KeyboardModifiers modifiers() const { return e->modifiers(); }

    QPointF pos() const { return e->pos(); }
    QPointF scenePos() const { return e->screenPos(); }
    QPointF screenPos() const { return e->scenePos(); }

    QPointF lastPos() const { return e->lastPos(); }
    QPointF lastScenePos() const { return e->lastScenePos(); }
    QPointF lastScreenPos() const { return e->lastScreenPos(); }

    bool accepted() const { return e->isAccepted(); }
    void setAccepted(bool accepted) { e->setAccepted(accepted); }
};

class GraphicsSceneHoverEvent
{
    Q_GADGET

    Q_PROPERTY(Qt::KeyboardModifiers modifiers READ modifiers CONSTANT)
    Q_PROPERTY(QPointF pos READ pos CONSTANT)
    Q_PROPERTY(QPointF scenePos READ scenePos CONSTANT)
    Q_PROPERTY(QPointF screenPos READ screenPos CONSTANT)
    Q_PROPERTY(QPointF lastPos READ lastPos CONSTANT)
    Q_PROPERTY(QPointF lastScenePos READ lastScenePos CONSTANT)
    Q_PROPERTY(QPointF lastScreenPos READ lastScreenPos CONSTANT)

    QGraphicsSceneHoverEvent *e = nullptr;
public:
    GraphicsSceneHoverEvent(QGraphicsSceneHoverEvent *event = nullptr) : e(event) {}

    Qt::KeyboardModifiers modifiers() const { return e->modifiers(); }

    QPointF pos() const { return e->pos(); }
    QPointF scenePos() const { return e->screenPos(); }
    QPointF screenPos() const { return e->scenePos(); }

    QPointF lastPos() const { return e->lastPos(); }
    QPointF lastScenePos() const { return e->lastScenePos(); }
    QPointF lastScreenPos() const { return e->lastScreenPos(); }
};

class GraphicsSceneWheelEvent
{
    Q_GADGET

    Q_PROPERTY(Qt::KeyboardModifiers modifiers READ modifiers CONSTANT)
    Q_PROPERTY(QPointF pos READ pos CONSTANT)
    Q_PROPERTY(QPointF scenePos READ scenePos CONSTANT)
    Q_PROPERTY(QPointF screenPos READ screenPos CONSTANT)
    Q_PROPERTY(Qt::MouseButtons buttons READ buttons CONSTANT)
    Q_PROPERTY(int delta READ delta CONSTANT)
    Q_PROPERTY(Qt::Orientation orientation READ orientation CONSTANT)

    QGraphicsSceneWheelEvent *e = nullptr;
public:
    GraphicsSceneWheelEvent(QGraphicsSceneWheelEvent *event = nullptr) : e(event) {}

    Qt::KeyboardModifiers modifiers() const { return e->modifiers(); }

    QPointF pos() const { return e->pos(); }
    QPointF scenePos() const { return e->screenPos(); }
    QPointF screenPos() const { return e->scenePos(); }

    Qt::MouseButtons buttons() const { return e->buttons(); }

    int delta() const { return e->delta(); }
    Qt::Orientation orientation() const { return e->orientation(); }
};

class PointRef
{
    Q_GADGET
    Q_PROPERTY(qreal x READ x WRITE setX)
    Q_PROPERTY(qreal y READ y WRITE setY)
    Q_PROPERTY(QPointF point READ point WRITE setPoint)

    QPointF *p = nullptr;

public:
    PointRef(QPointF *point = nullptr) : p(point) {}

    inline qreal x() const { return p->x(); }
    inline void setX(qreal x) { p->rx() = x; }

    inline qreal y() const { return p->y(); }
    inline void setY(qreal y) { p->ry() = y; }

    inline QPointF point() const { return *p; }
    inline void setPoint(QPointF point) { *p = point; }
};

Q_DECLARE_METATYPE(Qt::MouseButton)
Q_DECLARE_METATYPE(GraphicsSceneMouseEvent)
Q_DECLARE_METATYPE(GraphicsSceneHoverEvent)
Q_DECLARE_METATYPE(GraphicsSceneWheelEvent)
Q_DECLARE_METATYPE(PointRef)

#endif // GRAPHICSSCENEMOUSEEVENT_H
