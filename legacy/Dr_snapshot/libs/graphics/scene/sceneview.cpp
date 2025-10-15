#include "sceneview.h"
#include "scenemodel.h"
#include "qgraphicsscene.h"

#include <QGraphicsSceneEvent>
//#include <QDateTime>


void SceneView::setViewTransform(const QTransform &viewTransform)
{
    if (m_viewTransform == viewTransform)
        return;

    m_viewTransform = viewTransform;
    update();
}

void SceneView::replayLastMouseEvent()
{
    if (!useLastMouseEvent || !m_scene)
        return;
    mouseMoveEventHandler(&lastMouseEvent);
}

void SceneView::storeMouseEvent(QMouseEvent *event)
{
    useLastMouseEvent = true;
    lastMouseEvent = QMouseEvent(QEvent::MouseMove, event->localPos(), event->windowPos(), event->screenPos(),
                                 event->button(), event->buttons(), event->modifiers());
}

void SceneView::mouseMoveEventHandler(QMouseEvent *event)
{
    storeMouseEvent(event);
    lastMouseEvent.setAccepted(false);

    if (!sceneInteractionAllowed)
        return;
    if (!m_scene)
        return;

    QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseMove);
    mouseEvent.setWidget(0);
    mouseEvent.setButtonDownScenePos(mousePressButton, mousePressScenePoint);
    mouseEvent.setButtonDownScreenPos(mousePressButton, mousePressScreenPoint);

    mouseEvent.setScenePos(event->localPos());
    mouseEvent.setScreenPos(event->screenPos().toPoint());
    mouseEvent.setLastScenePos(lastMouseMoveScenePoint);
    mouseEvent.setLastScreenPos(lastMouseMoveScreenPoint);
    mouseEvent.setButtons(event->buttons());
    mouseEvent.setButton(event->button());
    mouseEvent.setModifiers(event->modifiers());

    lastMouseMoveScenePoint = mouseEvent.scenePos();
    lastMouseMoveScreenPoint = mouseEvent.screenPos();

    mouseEvent.setAccepted(false);
    //    if (event->spontaneous())
    //        qt_sendSpontaneousEvent(scene, &mouseEvent);
    //    else
    qApp->sendEvent(m_scene, &mouseEvent);

    // Remember whether the last event was accepted or not.
    lastMouseEvent.setAccepted(mouseEvent.isAccepted());

    if (mouseEvent.isAccepted() && mouseEvent.buttons() != 0) {
        // The event was delivered to a mouse grabber; the press is likely to
        // have set a cursor, and we must not change it.
        return;
    }
    event->ignore();


#ifndef QT_NO_CURSOR
    // QList<QGraphicsItem *> cachedItemsUnderMouse;
    // If all the items ignore hover events, we don't look-up any items
    // in QGraphicsScenePrivate::dispatchHoverEvent, hence the
    // cachedItemsUnderMouse list will be empty. We therefore do the look-up
    // for cursor items here if not all items use the default cursor.
    //    if (scene->d_func()->allItemsIgnoreHoverEvents && !scene->d_func()->allItemsUseDefaultCursor
    //        && scene->d_func()->cachedItemsUnderMouse.isEmpty()) {
    //        scene->d_func()->cachedItemsUnderMouse = scene->d_func()->itemsAtPosition(mouseEvent.screenPos(),
    //                                                                                  mouseEvent.scenePos(),
    //                                                                                  mouseEvent.widget());
    //    }
    //    // Find the topmost item under the mouse with a cursor.
    //    foreach (QGraphicsItem *item, scene->d_func()->cachedItemsUnderMouse) {
    //        if (item->hasCursor()) {
    //            _q_setViewportCursor(item->cursor());
    //            return;
    //        }
    //    }

    //    // No items with cursors found; revert to the view cursor.
    //    if (hasStoredOriginalCursor) {
    //        // Restore the original viewport cursor.
    //        hasStoredOriginalCursor = false;
    //        viewport->setCursor(originalCursor);
    //    }
#endif
}

SceneView::SceneView(QQuickItem *parent) :
    QQuickPaintedItem(parent),
    lastMouseEvent(QEvent::None, QPointF(), QPointF(), QPointF(), Qt::NoButton, 0, 0)
{
    setTransformOrigin(QQuickItem::TopLeft);
    setRenderTarget(QQuickPaintedItem::FramebufferObject);
    //    setRenderTarget(QQuickPaintedItem::Image);
    setPerformanceHint(QQuickPaintedItem::FastFBOResizing);
}

void SceneView::paint(QPainter *painter)
{
    if(!isVisible())
        return;
    QRectF target = QRectF(0, 0, width(), height());
    //    renderCount++;
    //    qint64 st = now();
    QRectF sceneRect = m_viewTransform.mapRect(target);
    m_model->m_scene->render(painter, target, sceneRect);
    //    qDebug() << "scene rendered in" << now() - st << "ms";
}


void SceneView::setModel(SceneModel *model)
{
    if(m_model == model)
        return;
    if (m_model)
        m_model->disconnect(0, this, 0);
    m_model = model;
    //    connect(m_scene, SIGNAL(changed()), this, SLOT(update()));
    //    connect(m_model, SIGNAL(changed()), this, SLOT(update()), Qt::QueuedConnection);
    emit modelChanged();
    update();
}



SceneModel *SceneView::model() const
{
    return m_model;
}

void SceneView::mouseReleaseEvent(QMouseEvent *event)
{
    storeMouseEvent(event);

    if (!sceneInteractionAllowed)
        return;

    if (!m_scene)
        return;

    QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseRelease);
    mouseEvent.setWidget(0);

    mouseEvent.setButtonDownScenePos(mousePressButton, mousePressScenePoint);
    mouseEvent.setButtonDownScreenPos(mousePressButton, mousePressScreenPoint);
    mouseEvent.setScenePos(event->pos());
    mouseEvent.setScreenPos(event->globalPos());
    mouseEvent.setLastScenePos(lastMouseMoveScenePoint);
    mouseEvent.setLastScreenPos(lastMouseMoveScreenPoint);
    mouseEvent.setButtons(event->buttons());
    mouseEvent.setButton(event->button());
    mouseEvent.setModifiers(event->modifiers());
    mouseEvent.setAccepted(false);
    qApp->sendEvent(m_scene, &mouseEvent);
    lastMouseEvent.setAccepted(mouseEvent.isAccepted());
}

void SceneView::mouseMoveEvent(QMouseEvent *event)
{
    //    if( !m_enabled ) return;
    mouseMoveEventHandler(event);
}

void SceneView::hoverMoveEvent(QHoverEvent *event)
{
    //    if( !m_enabled ) return;
    if (!sceneInteractionAllowed)
        return;

    if (!m_scene)
        return;

    QGraphicsSceneHoverEvent e(QEvent::GraphicsSceneHoverMove);
    e.setScenePos(event->posF());
    e.setScreenPos(mapToScene(event->posF()).toPoint());
    e.setLastScenePos(lastMouseMoveScenePoint);
    e.setLastScreenPos(lastMouseMoveScreenPoint);
    e.setModifiers(event->modifiers());
    e.setAccepted(false);

    qApp->sendEvent(m_scene, &e);
    event->setAccepted(e.isAccepted());
    lastMouseEvent.setAccepted(e.isAccepted());
}

void SceneView::hoverEnterEvent(QHoverEvent *event)
{
    //    if( !m_enabled ) return;
    if (!sceneInteractionAllowed)
        return;

    if (!m_scene)
        return;

    QGraphicsSceneHoverEvent e(QEvent::GraphicsSceneHoverEnter);
    e.setScenePos(event->posF());
    e.setScreenPos(mapToScene(event->pos()).toPoint());
    e.setLastScenePos(lastMouseMoveScenePoint);
    e.setLastScreenPos(lastMouseMoveScreenPoint);
    e.setModifiers(event->modifiers());
    e.setAccepted(false);

    qApp->sendEvent(m_scene, &e);
    event->setAccepted(e.isAccepted());

    lastMouseEvent.setAccepted(e.isAccepted());
}

void SceneView::hoverLeaveEvent(QHoverEvent *event)
{
    //    if( !m_enabled ) return;
    if (!sceneInteractionAllowed)
        return;

    if (!m_scene)
        return;

    QGraphicsSceneHoverEvent e(QEvent::GraphicsSceneHoverLeave);
    e.setScenePos(event->posF());
    e.setScreenPos(mapToScene(event->pos()).toPoint());
    e.setLastScenePos(lastMouseMoveScenePoint);
    e.setLastScreenPos(lastMouseMoveScreenPoint);
    e.setModifiers(event->modifiers());
    e.setAccepted(false);

    qApp->sendEvent(m_scene, &e);
    event->setAccepted(e.isAccepted());
    lastMouseEvent.setAccepted(e.isAccepted());
}

void SceneView::mousePressEvent(QMouseEvent *event)
{
    //    if( !m_enabled ) return;
    storeMouseEvent(event);
    lastMouseEvent.setAccepted(false);

    // Store some of the event's button-down data.
    mousePressViewPoint = event->pos();
    mousePressScenePoint = event->localPos();
    mousePressScreenPoint = event->screenPos().toPoint();
    lastMouseMoveScenePoint = mousePressScenePoint;
    lastMouseMoveScreenPoint = mousePressScreenPoint;
    mousePressButton = event->button();

    if (m_scene) {
        // Convert and deliver the mouse event to the scene.
        QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMousePress);
        mouseEvent.setWidget(0);

        mouseEvent.setButtonDownScenePos(mousePressButton, mousePressScenePoint);
        mouseEvent.setButtonDownScreenPos(mousePressButton, mousePressScreenPoint);
        mouseEvent.setScenePos(mousePressScenePoint);
        mouseEvent.setScreenPos(mousePressScreenPoint);
        mouseEvent.setLastScenePos(lastMouseMoveScenePoint);
        mouseEvent.setLastScreenPos(lastMouseMoveScreenPoint);
        mouseEvent.setButtons(event->buttons());
        mouseEvent.setButton(event->button());
        mouseEvent.setModifiers(event->modifiers());
        mouseEvent.setAccepted(false);
        //        if (event->spontaneous())
        //            qt_sendSpontaneousEvent(d->scene, &mouseEvent);
        //        else
        qApp->sendEvent(m_scene, &mouseEvent);

        // Update the original mouse event accepted state.
        bool isAccepted = mouseEvent.isAccepted();
        event->setAccepted(isAccepted);

        // Update the last mouse event accepted state.
        lastMouseEvent.setAccepted(isAccepted);

        if (isAccepted)
            return;
        event->ignore();
    }
}

QRectF SceneView::viewportRect() const
{
    return m_viewportRect;
}
