#include "visualinstance.h"
#include <QGraphicsScene>
#include <QJSEngine>
#include <QQmlEngine>
#include "scenemodel.h"
#include "eventwrappers.h"
#include <qgraphicssceneevent.h>
VisualInstance *VisualInstanceHandle::createItem(QGraphicsItem *parent) const
{
    qWarning() << Q_FUNC_INFO << "this should not happen. Check twice if your \"createItem\" virtual method is implemented";
    return new VisualInstance(parent);
}

VisualInstanceHandle::VisualInstanceHandle(VisualInstance *item) : m_item(item) {}

VisualInstanceHandle::~VisualInstanceHandle()
{
    m_item = nullptr;
//    qDebug() << "Handle destroyed";
}

VisualInstance *VisualInstanceHandle::item() const {
    if (!m_item) m_item = createItem();
    return m_item;
}

void VisualInstanceHandle::setItem(VisualInstance *item) { m_item = item; }

QRectF VisualInstanceHandle::boundingRect() const
{
    chkItem(QRectF());
    return m_item->boundingRect();
}

QPointF VisualInstanceHandle::position() const
{
    chkItem(QPointF());
    return m_item->pos();
}

qreal VisualInstanceHandle::x() const
{
    chkItem(0);
    return m_item->x();
}

qreal VisualInstanceHandle::y() const
{
    chkItem(0);
    return m_item->y();
}

qreal VisualInstanceHandle::rotation() const
{
    chkItem(0);
    return m_item->rotation();
}

qreal VisualInstanceHandle::scale() const
{
    chkItem(0);
    return m_item->scale();
}

QPointF VisualInstanceHandle::transformOriginPoint() const
{
    chkItem(QPointF());

    return m_item->transformOriginPoint();;
}

qreal VisualInstanceHandle::opacity() const
{
    chkItem(0);
    return m_item->opacity();;
}

bool VisualInstanceHandle::visible() const
{
    chkItem(false);
    return m_item->isVisible();
}

qreal VisualInstanceHandle::z() const
{
    chkItem(0);
    return m_item->zValue();
}

bool VisualInstanceHandle::hoverEnabled() const
{
    chkItem(false);
    return m_item->acceptHoverEvents();
}

bool VisualInstanceHandle::isSelected() const
{
    chkItem(false);
    return m_item->isSelected();
}

QVariantList VisualInstanceHandle::childItems() const
{
    QVariantList ret;
    chkItem(ret);
    //        m_item->childItems();
    return ret;
}

VisualInstanceHandle VisualInstanceHandle::parentItem() const
{
    chkItem(VisualInstanceHandle());
    return VisualInstanceHandle(); // FIXME: implement
}

Qt::MouseButtons VisualInstanceHandle::acceptedButtons() const
{
    chkItem(0);
    return m_item->acceptedMouseButtons();
}

int VisualInstanceHandle::flags() const
{
    chkItem(0);
    return (int)m_item->flags();
}

bool VisualInstanceHandle::isValid() const
{
    return m_item != nullptr;
}

void VisualInstanceHandle::setPosition(QPointF position)
{
    if (m_item->pos() != position)
        m_item->setPos(position);
}

void VisualInstanceHandle::setX(qreal x)
{
    if (m_item->x() != x)
        m_item->setX( x );
}

void VisualInstanceHandle::setY(qreal y)
{
    if (m_item->y() != y)
        m_item->setY( y );
}

void VisualInstanceHandle::setRotation(qreal rotation)
{
    if( m_item->rotation() != rotation )
        m_item->setRotation( rotation );
}

void VisualInstanceHandle::setScale(qreal scale)
{
    if (m_item->scale() != scale)
        m_item->setScale(scale);
}

void VisualInstanceHandle::setTransformOriginPoint(QPointF transformOriginPoint)
{
    if( m_item->transformOriginPoint() != transformOriginPoint )
        m_item->setTransformOriginPoint(transformOriginPoint);
}

void VisualInstanceHandle::setOpacity(qreal opacity)
{
    if( m_item->opacity() != opacity )
        m_item->setOpacity(opacity);
}

void VisualInstanceHandle::setVisible(bool visible)
{
    if( m_item->isVisible() != visible )
        m_item->setVisible(visible);
}

void VisualInstanceHandle::setZ(qreal z)
{
    if (m_item->zValue() != z)
        m_item->setZValue(z);
}

void VisualInstanceHandle::setHoverEnabled(bool hoverEnabled)
{
    if (m_item->acceptHoverEvents() != hoverEnabled)
        m_item->setAcceptHoverEvents(hoverEnabled);
}

void VisualInstanceHandle::setSelected(bool selected)
{
    if (m_item->isSelected() != selected)
        m_item->setSelected(selected);
}

void VisualInstanceHandle::setParentItem(VisualInstanceHandle parentItem)
{
    //        m_parentItem = parentItem;
}

void VisualInstanceHandle::setAcceptedButtons(Qt::MouseButtons acceptedButtons)
{
    m_item->setAcceptedMouseButtons(acceptedButtons);
}

void VisualInstanceHandle::setFlags(int flags)
{
    int f = flags & (~DrGraphicsFlagsMask);
    if (m_item->flags() != f) {
        m_item->setFlags((QGraphicsItem::GraphicsItemFlags)f);
    }
    f =  flags & DrGraphicsFlagsMask;
    if (m_item->data(VisualInstance::ExtraFlagsKey).toInt() != f) {
        m_item->setData(VisualInstance::ExtraFlagsKey, f);
        // TODO: implement smart update like in QGraphicsItem's implementation @Qt sources: Src/qtbase/src/widgets/graphicsview/qgraphicsitem.cpp:1800
        m_item->update();
    }
}

void VisualInstanceHandle::update() const
{
    if (!item())
        return;
    item()->update();
}

void VisualInstanceHandle::destroy()
{
    if (!item())
        return;
    item()->scene()->removeItem(item());
    delete item();
    setItem(nullptr);
}

bool VisualInstanceHandle::bind(VisualInstanceHandle::GraphicsItemChange change, QJSValue callback)
{
    m_item->setData((VisualInstance::DataKey)(int)change, QVariant::fromValue(callback));
    return true;

}


QJSValue VisualInstanceHandle::onMousePress() const
{
    return item()->data(VisualInstance::MousePressEventHandler).value<QJSValue>();
}

void VisualInstanceHandle::setOnMousePress(QJSValue arg)
{
    //    qDebug() << "Setting mouse press handler" << arg.isCallable() << arg.toString();
    item()->setData(VisualInstance::MousePressEventHandler, QVariant::fromValue(arg));
}

QJSValue VisualInstanceHandle::onMouseRelease() const
{
    return item()->data(VisualInstance::MouseReleaseEventHandler).value<QJSValue>();
}

void VisualInstanceHandle::setOnMouseRelease(QJSValue arg)
{
    //    qDebug() << "Setting mouse release handler" << arg.isCallable() << arg.toString();
    item()->setData(VisualInstance::MouseReleaseEventHandler, QVariant::fromValue(arg));
}

QJSValue VisualInstanceHandle::onMouseMove() const
{
    return item()->data(VisualInstance::MouseMoveEventHandler).value<QJSValue>();
}

void VisualInstanceHandle::setOnMouseMoved(QJSValue arg)
{
    //    qDebug() << "Setting mouse move handler" << arg.isCallable() << arg.toString();
    item()->setData(VisualInstance::MouseMoveEventHandler, QVariant::fromValue(arg));
}

QJSValue VisualInstanceHandle::onHoverEnter() const
{
    return item()->data(VisualInstance::HoverEnterEventHandler).value<QJSValue>();
}

void VisualInstanceHandle::setOnHoverEnter(QJSValue arg)
{
    //    qDebug() << "Setting hover enter" << arg.isCallable() << arg.toString();
    item()->setData(VisualInstance::HoverEnterEventHandler, QVariant::fromValue(arg));
}

QJSValue VisualInstanceHandle::onHoverLeave() const
{
    return item()->data(VisualInstance::HoverLeaveEventHandler).value<QJSValue>();
}

void VisualInstanceHandle::setOnHoverLeave(QJSValue arg)
{
    item()->setData(VisualInstance::HoverLeaveEventHandler, QVariant::fromValue(arg));
}

QJSValue VisualInstanceHandle::onHoverMove() const
{
    return item()->data(VisualInstance::HoverMoveEventHandler).value<QJSValue>();
}

void VisualInstanceHandle::setOnHoverMove(QJSValue arg)
{
    item()->setData(VisualInstance::HoverMoveEventHandler, QVariant::fromValue(arg));
}


void VisualInstance::handleMouseEvent(QGraphicsSceneMouseEvent *event, DataKey eventType)
{
    QJSValue handler = data(eventType).value<QJSValue>();
    if (!handler.isCallable()) {
//        event->setAccepted(false);
        return;
    }
    QQmlEngine *engine = data(QmlEngineKey).value<QQmlEngine*>();
    if (engine) {
        QVariant handle = getHandle();
        QJSValue v = engine->toScriptValue(GraphicsSceneMouseEvent(event));
        QJSValue thisObj = engine->toScriptValue(handle);
        QJSValue r = handler.callWithInstance(thisObj, { v });
        if (r.isError()) {
            qWarning() << "error in callback:"
                       << r.property("message").toString()
                       << r.property("fileName").toString()
                       << r.property("lineNumber").toString()
                       << r.property("lineNumber").toInt()
                       << r.toString();
        }
    }
}

void VisualInstance::handleMouseHoverEvent(QGraphicsSceneHoverEvent *event, DataKey eventType)
{
    QJSValue handler = data(eventType).value<QJSValue>();

    if (!handler.isCallable())
        return;

    QQmlEngine *engine = data(QmlEngineKey).value<QQmlEngine*>();
    if (engine) {
        auto handle = getHandle();
        QJSValue v = engine->toScriptValue(GraphicsSceneHoverEvent(event));
        QJSValue thisObj = engine->toScriptValue(handle);
        QJSValue r = handler.callWithInstance(thisObj, { v });
        if (r.isError()) {
            qWarning() << "error in callback:"
                       << r.property("message").toString()
                       << r.property("fileName").toString()
                       << r.property("lineNumber").toString()
                       << r.property("lineNumber").toInt()
                       << r.toString();
        }
    }
}

int VisualInstance::type() const
{
    return SELF_PTR_TYPEID;
}

QVariant VisualInstance::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    QJSValue callback = data((DataKey)change).value<QJSValue>();

    if (!callback.isCallable())
        return QGraphicsItem::itemChange(change, value);

    QQmlEngine *engine = data(QmlEngineKey).value<QQmlEngine*>();
    QVariant retValue = value;
    if (engine) {
        QJSValue thisObj = engine->toScriptValue(getHandle());
        QJSValue r;
        switch (change) {
        case ItemParentChange:
        case ItemParentHasChanged:
        case ItemChildAddedChange:
        case ItemChildRemovedChange:
            r = handleItemChange(engine, thisObj, callback, value.value<QGraphicsItem*>());
            retValue = r.toVariant();
            break;
        case ItemTransformOriginPointHasChanged:
        case ItemPositionHasChanged:
        case ItemTransformOriginPointChange:
        case ItemPositionChange: {
            QPointF in = value.value<QPointF>();
            PointRef p_ref(&in);
            r = handleItemChange(engine, thisObj, callback, p_ref);
//            qDebug() << "Value changed" << value << in << r.toVariant();
            retValue = QVariant(in);
            break;
        }
        case ItemZValueChange:
        case ItemRotationChange:
        case ItemScaleChange:
        case ItemOpacityChange:
        case ItemZValueHasChanged:
        case ItemOpacityHasChanged:
        case ItemScenePositionHasChanged:
        case ItemRotationHasChanged:
        case ItemScaleHasChanged:
            r = handleItemChange(engine, thisObj, callback, value.value<qreal>());
            retValue = r.toNumber();
            break;
        case ItemTransformChange:
        case ItemTransformHasChanged:
            r = handleItemChange(engine, thisObj, callback, value.value<QTransform>());
            break;
        case ItemVisibleChange:
        case ItemEnabledChange:
        case ItemSelectedChange:
        case ItemVisibleHasChanged:
        case ItemEnabledHasChanged:
        case ItemSelectedHasChanged:
            r = handleItemChange(engine, thisObj, callback, value.value<bool>());
            retValue = r.toBool();
            break;
        case ItemSceneChange:
            if (!value.value<QObject*>()) {
                qDebug()<< "Destroyed item" << this;
            }
        case ItemSceneHasChanged:
            r = handleItemChange(engine, thisObj, callback, value.value<QGraphicsScene*>());
            retValue = QVariant::fromValue(qobject_cast<QGraphicsScene*>(r.toQObject()));
            break;
        case ItemToolTipHasChanged:
        case ItemCursorHasChanged:
        case ItemCursorChange:
        case ItemToolTipChange:
        case ItemFlagsChange:
        case ItemFlagsHaveChanged:
        default:
            break;
        }
        if (r.isError()) {
            qWarning() << "error in callback:"
                       << r.property("message").toString()
                       << r.property("fileName").toString()
                       << r.property("lineNumber").toString()
                       << r.property("lineNumber").toInt()
                       << r.toString();
        }
    }

    return retValue;
//    return QGraphicsItem::itemChange(change, retValue);


}

void VisualInstance::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    handleMouseEvent(event, MousePressEventHandler);
    QGraphicsItem::mousePressEvent(event);
}

void VisualInstance::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    handleMouseEvent(event, MouseReleaseEventHandler);
    QGraphicsItem::mouseReleaseEvent(event);
}

void VisualInstance::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    handleMouseEvent(event, MouseMoveEventHandler);
    QGraphicsItem::mouseMoveEvent(event);
}

void VisualInstance::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    handleMouseHoverEvent(event, HoverEnterEventHandler);
    QGraphicsItem::hoverEnterEvent(event);
}

void VisualInstance::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    handleMouseHoverEvent(event, HoverLeaveEventHandler);
    QGraphicsItem::hoverLeaveEvent(event);
}

void VisualInstance::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    handleMouseHoverEvent(event, HoverMoveEventHandler);
    QGraphicsItem::hoverMoveEvent(event);
}
QVariant VisualInstance::getHandle()
{
    if (data(CachedHandleKey).isValid()) {
//        qDebug() << "cached handle" << data(CachedHandleKey);
        return data(CachedHandleKey);
    }
    int h_tid = getHandleType(type());
//    qDebug() << "Created handle" << QMetaType::typeName(h_tid);
    VisualInstanceHandle *handle = reinterpret_cast<VisualInstanceHandle*>(QMetaType::create(h_tid));
    handle->setItem(const_cast<VisualInstance*>(this));
    QVariant h(h_tid, handle);
    setData(CachedHandleKey, h);
    return h;
}

QTransform VisualInstance::currentViewTransform() const
{
    if (!scene())
        return QTransform();
    SceneData *d = static_cast<SceneData*>(scene()->userData(0));
    return d->viewTransform;
}




