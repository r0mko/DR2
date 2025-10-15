#include "graphicsobject.h"

#include <QPainter>
#include <eventwrappers.h>

GraphicsObject::GraphicsObject(QGraphicsItem *parent) :
    QGraphicsObject(parent),
    m_hovered(false),
    m_pressed(false)
{

}

Scene *GraphicsObject::scene() const
{
    if (!QGraphicsItem::scene())
        return nullptr;
    QObjectUserData *d = QGraphicsItem::scene()->userData(0);
    if (d) {
        SceneData *sd = static_cast<SceneData*>(d);
        if (sd)
            return sd->scene;
    }
    return nullptr;
    //        return qobject_cast<Scene*>(QGraphicsI);
}

void GraphicsObject::setScene(Scene *scene)
{
    scene->qGraphicsScene()->addItem(this);
    emit sceneChanged();
    setupScene(scene);
}

void GraphicsObject::update(const QRectF &rect)
{
    QGraphicsItem::update(rect);
}

void GraphicsObject::grabMouse()
{
    QGraphicsObject::grabMouse();
}

void GraphicsObject::ungrabMouse()
{
    QGraphicsObject::ungrabMouse();
}

void GraphicsObject::setFlags(VisualInstanceHandle::VisualInstanceFlags flags)
{
    int f = flags & (~VisualInstanceHandle::DrGraphicsFlagsMask);
    if (QGraphicsItem::flags() != f) {
        QGraphicsItem::setFlags((QGraphicsItem::GraphicsItemFlags)f);
    }
    f =  flags & VisualInstanceHandle::DrGraphicsFlagsMask;
    if (data(VisualInstance::ExtraFlagsKey).toInt() != f) {
        setData(VisualInstance::ExtraFlagsKey, f);
        // TODO: implement smart update like in QGraphicsItem's implementation @Qt sources: Src/qtbase/src/widgets/graphicsview/qgraphicsitem.cpp:1800
        QGraphicsItem::update();
    }
}

bool GraphicsObject::bind(QGraphicsItem::GraphicsItemChange change, QJSValue callback)
{
    if (callback.isCallable()) {
        setData((VisualInstance::DataKey)(int)change, QVariant::fromValue(callback));
        return true;
    }
    return false;
}

void GraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_pressed = true;
    emit pressedChanged();
    GraphicsSceneMouseEvent e(event);
    emit mousePress(e);
    QGraphicsObject::mousePressEvent(event);

}

void GraphicsObject::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    m_pressed = false;
    emit pressedChanged();
    GraphicsSceneMouseEvent e(event);
    emit mouseRelease(e);
    if (event->buttonDownPos(event->button()) == event->lastPos())
        emit mouseClicked(e);
    QGraphicsObject::mousePressEvent(event);
}

void GraphicsObject::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    GraphicsSceneMouseEvent e(event);
    emit mouseMove(e);
    QGraphicsObject::mouseMoveEvent(event);
}

void GraphicsObject::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    m_hovered = true;
    emit hoveredChanged();
    emit hoverEnter(GraphicsSceneHoverEvent(event));
    QGraphicsObject::hoverEnterEvent(event);
}

void GraphicsObject::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    m_hovered = false;
    emit hoveredChanged();
    emit hoverLeave(GraphicsSceneHoverEvent(event));
    QGraphicsObject::hoverLeaveEvent(event);
}

void GraphicsObject::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    m_hovered = true;
    emit hoverMove(GraphicsSceneHoverEvent(event));
    QGraphicsObject::hoverMoveEvent(event);
}

bool GraphicsObject::isHovered() const
{
    return m_hovered;
}

bool GraphicsObject::isPressed() const
{
    return m_pressed;
}

bool GraphicsObject::hoverEnabled() const
{
    return acceptHoverEvents();
}

void GraphicsObject::setHoverEnabled(bool hoverEnabled)
{
    if (acceptHoverEvents() == hoverEnabled)
        return;

    setAcceptHoverEvents(hoverEnabled);
    emit hoverEnabledChanged();
}

QTransform GraphicsObject::currentViewTransform() const
{
    if (!scene())
        return QTransform();
    SceneData *d = static_cast<SceneData*>(scene()->userData(0));
    return d->viewTransform;
}

void GraphicsObject::preparePainter(QPainter *painter) const
{
    int allFlags = (int)flags() | data(VisualInstance::ExtraFlagsKey).toInt();
    if (allFlags & VisualInstanceHandle::ItemIgnoresTransformations) {
        qreal s = painter->worldTransform().m11();
        painter->scale(1 / s, 1 / s);
    }

}

QRectF GraphicsObject::boundingRect() const
{
    return QRectF();
}

void GraphicsObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    Q_UNUSED(painter);
}

void GraphicsObject::classBegin()
{
}

void GraphicsObject::componentComplete()
{

}

QVariant GraphicsObject::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    QJSValue callback = data((VisualInstance::DataKey)change).value<QJSValue>();

    //    if (!callback.isCallable())
//        return QGraphicsItem::itemChange(change, value);

//    QQmlEngine *engine = qmlEngine(this);
    QVariant retValue = QGraphicsObject::itemChange(change, value);;
//    if (engine) {
//        QJSValue thisObj = engine->newQObject(this);
//        QJSValue r;
        switch (change) {
//        case ItemParentChange:
//        case ItemParentHasChanged:
//        case ItemChildAddedChange:
//        case ItemChildRemovedChange:
//            r = handleItemChange(engine, thisObj, callback, value.value<QGraphicsItem*>());
//            retValue = r.toVariant();
//            break;
//        case ItemTransformOriginPointHasChanged:
//        case ItemPositionHasChanged:
//        case ItemTransformOriginPointChange:
//        case ItemPositionChange: {
//            QPointF in = value.value<QPointF>();
//            PointRef p_ref(&in);
//            r = handleItemChange(engine, thisObj, callback, p_ref);
////            qDebug() << "Value changed" << value << in << r.toVariant();
//            retValue = QVariant(in);
//            break;
//        }
//        case ItemZValueChange:
//        case ItemRotationChange:
//        case ItemScaleChange:
//        case ItemOpacityChange:
//        case ItemZValueHasChanged:
//        case ItemOpacityHasChanged:
//        case ItemScenePositionHasChanged:
//        case ItemRotationHasChanged:
//        case ItemScaleHasChanged:
//            r = handleItemChange(engine, thisObj, callback, value.value<qreal>());
//            retValue = r.toNumber();
//            break;
//        case ItemTransformChange:
//            retValue = handleItemChange(engine, thisObj, callback, value.value<QTransform>()).toVariant();
//            break;
//        case ItemTransformHasChanged:
//            r = handleItemChange(engine, thisObj, callback, value.value<QTransform>());
//            break;
//        case ItemSelectedChange:
//            emit selectedChanged();
//            break;
//        case ItemEnabledChange:
//            r = handleItemChange(engine, thisObj, callback, value.value<bool>());
//            retValue = r.toBool();
//        case ItemVisibleHasChanged:
//        case ItemEnabledHasChanged:
        case ItemSelectedHasChanged:
            emit selectedChanged();
            break;
//        case ItemSceneChange:
//            if (!value.value<QObject*>()) {
//                qDebug()<< "Destroyed item" << this;
//            }
//        case ItemSceneHasChanged:
//            r = handleItemChange(engine, thisObj, callback, value.value<QGraphicsScene*>());
//            retValue = QVariant::fromValue(qobject_cast<QGraphicsScene*>(r.toQObject()));
//            break;
//        case ItemToolTipHasChanged:
//        case ItemCursorHasChanged:
//        case ItemCursorChange:
//        case ItemToolTipChange:
//        case ItemFlagsChange:
        case ItemFlagsHaveChanged:
            emit flagsChanged();
            break;
        default:
            break;
        }
//        if (r.isError()) {
//            qWarning() << "error in callback:"
//                       << r.property("message").toString()
//                       << r.property("fileName").toString()
//                       << r.property("lineNumber").toString()
//                       << r.property("lineNumber").toInt()
//                       << r.toString();
//        }
//    }

    return retValue;
//    return QGraphicsItem::itemChange(change, retValue);

}

VisualInstanceHandle::VisualInstanceFlags GraphicsObject::flags() const
{
    return (VisualInstanceHandle::VisualInstanceFlags)((int)QGraphicsItem::flags() | (int)data(VisualInstance::ExtraFlagsKey).toInt());
}

QQmlListProperty<QObject> GraphicsObject::qmlChildren()
{
    return QQmlListProperty<QObject>(this, 0, &GraphicsObject::qmlAppendChild,
                                     &GraphicsObject::qmlChildCount,
                                     &GraphicsObject::qmlChildAt,
                                     QQmlListProperty<QObject>::ClearFunction());
}

void GraphicsObject::qmlAppendChild(QQmlListProperty<QObject> *list, QObject *child)
{
    GraphicsObject *f = qobject_cast<GraphicsObject *>(list->object);
    f->appendChild(child);
}

int GraphicsObject::qmlChildCount(QQmlListProperty<QObject> *list)
{
    GraphicsObject *f = qobject_cast<GraphicsObject *>(list->object);
    return f->m_children.size();

}

QObject *GraphicsObject::qmlChildAt(QQmlListProperty<QObject> *list, int index)
{
    GraphicsObject *f = qobject_cast<GraphicsObject *>(list->object);
    return f->m_children.at(index);
}

void GraphicsObject::appendChild(QObject *object)
{
    m_children.append(object);
    emit childrenChanged();
}

int GraphicsObject::removeChild(QObject *object)
{
    int ret = m_children.removeAll(object);
    if (ret > 0)
        emit childrenChanged();
    return ret;
}

