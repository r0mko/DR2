#include "scene.h"
#include "qgraphicsitem.h"
#include "scenerenderer.h"
#include "qgraphicsscene.h"
//#include "graphicspath.h"
//#include "sceneitem.h"
#include "qdebug.h"
#include <QVariant>
#include "qgraphicssceneevent.h"
#include <QDebug>
#include <QQuickWindow>
#include <QQmlListReference>
#include <qpainter.h>
#include <graphicsobject.h>
#include <QElapsedTimer>
#include "../cartograph/defs.h"


//QQmlListProperty<SceneItem> Scene::qmlSceneItems() {
//    return QQmlListProperty<SceneItem>(this, 0,
//                                       &Scene::qmlItemsAppend,
//                                       &Scene::qmlItemsCount,
//                                       &Scene::qmlItemsAt,
//                                       QQmlListProperty<SceneItem>::ClearFunction());
//}

void Scene::replayLastMouseEvent()
{
    if (!useLastMouseEvent || !m_scene)
        return;
    mouseMoveEventHandler(&lastMouseEvent);
}

void Scene::storeMouseEvent(QMouseEvent *event)
{
    useLastMouseEvent = true;
    lastMouseEvent = QMouseEvent(QEvent::MouseMove, event->localPos(), event->windowPos(), event->screenPos(),
                                 event->button(), event->buttons(), event->modifiers());
}

//void Scene::qmlItemsAppend(QQmlListProperty<SceneItem> *list, SceneItem *object)
//{
//    Scene *o = qobject_cast<Scene*>(list->object);
//    if (!o)
//        return;
////    o->addItem(object);
//    emit o->itemAdded(object);
//}

//int Scene::qmlItemsCount(QQmlListProperty<SceneItem> *list)
//{
//    Scene *o = qobject_cast<Scene*>(list->object);
//    return o->m_scene->items().size();
//}

//SceneItem *Scene::qmlItemsAt(QQmlListProperty<SceneItem> *list, int index)
//{
//    Scene *o = qobject_cast<Scene*>(list->object);

//    return o->m_scene->items().at(index)->data(SceneItem::QMLParent).value<SceneItem*>();
//}

//QQmlListProperty<SceneItem> Scene::qmlSelectedItems() {
//    return QQmlListProperty<SceneItem>(this, 0,
//                                       &Scene::qmlSelectedItemsCount,
//                                       &Scene::qmlSelectedItemsAt);
//}

//int Scene::qmlSelectedItemsCount(QQmlListProperty<SceneItem> *list)
//{
//    Scene *o = qobject_cast<Scene*>(list->object);
//    return o->m_scene->selectedItems().size();
//}

//SceneItem *Scene::qmlSelectedItemsAt(QQmlListProperty<SceneItem> *list, int index)
//{
//    Scene *o = qobject_cast<Scene*>(list->object);
//    return o->m_scene->selectedItems().at(index)->data(SceneItem::QMLParent).value<SceneItem*>();
//}

Scene::Scene(QQuickItem *parent) :
    QQuickItem(parent),
    m_viewport(nullptr),
    sceneInteractionAllowed(true),
    sceneRectExplicitlySet(false),
    lastMouseEvent(QEvent::None, QPointF(), QPointF(), QPointF(), Qt::NoButton, 0, 0),
    m_autoUpdate(true),
    renderCount(0),
    m_notifyViewTransformUpdates(true)
  //    m_enabled( true )
{
    setTransformOrigin(QQuickItem::TopLeft);
    setAcceptedMouseButtons(Qt::AllButtons);
    m_renderer = new SceneRenderer(this);

    setAntialiasing(true);
    setAcceptHoverEvents(true);
    initSceneImpl(new MyGraphicsScene(this));
    m_renderer->setScene(this);
    connect(this, &QQuickItem::widthChanged, this, &Scene::adjustSceneRect);
    connect(this, &QQuickItem::heightChanged, this, &Scene::adjustSceneRect);
    connect( m_renderer, SIGNAL(scaleChanged()), this, SLOT(onRendererScaleShanged()));
}

Scene::~Scene()
{

}

//SceneItem *Scene::itemAt(const QPointF &pos) const
//{
//    return m_scene->itemAt(pos, QTransform())->data(SceneItem::QMLParent).value<SceneItem*>();
//}

//SceneItem *Scene::itemAt(qreal x, qreal y) const
//{
//    return m_scene->itemAt(x, y, QTransform())->data(SceneItem::QMLParent).value<SceneItem*>();

//}

//QList<SceneItem *> Scene::sceneItems() const
//{
//    QList<SceneItem *> ret;
//    for (QGraphicsItem *i:m_scene->items()) {
//        ret.append(i->data(SceneItem::QMLParent).value<SceneItem*>());
//    }
//    return ret;
//}

QRectF Scene::sceneRect() const
{
    return m_scene->sceneRect();
}

QQuickItem *Scene::viewport() const
{
    if (m_viewport)
        return m_viewport;
    return window()->contentItem();
}

QTransform Scene::viewTransform() const
{
    return m_viewTransform;
}

void Scene::componentComplete()
{
    QQuickItem::componentComplete();

    //    QQmlListReference ref(this, "data");
    //    //    qDebug() << "Scene data size" << ref.count();
    //    for (int i = 0; i < ref.count(); ++i) {
    //        SceneItem *child = qobject_cast<SceneItem*>(ref.at(i));
    //        if (child) {
    //            addItem(child);
    //        }
    //    }

//    connect(this->window(), SIGNAL(afterAnimating()), this, SLOT(adjustView())/*, Qt::QueuedConnection*/);
    connect(this->window(), &QQuickWindow::afterAnimating, this, &Scene::adjustView);
    adjustView();
}



//void Scene::removeItem(SceneItem *item)
//{
//    m_scene->removeItem(item->item());
//    emit itemRemoved(item);
//}

//void Scene::addItem(SceneItem *item)
//{
//    //    qDebug() << "Adding item" << item;
//    m_scene->addItem(item->item());
//    if (item->parent() != this)
//        item->setParent(this);
//    emit itemAdded(item);
//}


void Scene::setSceneRect(QRectF arg)
{
    if (m_scene->sceneRect() == arg)
        return;
    sceneRectExplicitlySet = true;
    disconnect(this, &QQuickItem::widthChanged, this, &Scene::adjustSceneRect);
    disconnect(this, &QQuickItem::heightChanged, this, &Scene::adjustSceneRect);
    m_scene->setSceneRect(arg);
}

void Scene::clearSelection()
{
    m_scene->clearSelection();
}

void Scene::addObject(GraphicsObject *object)
{
    object->setData(VisualInstance::QmlEngineKey, QVariant::fromValue(qmlEngine(this)));
    m_scene->addItem(object);
}

void Scene::setSelectionArea(const QRectF &rect, Qt::ItemSelectionMode mode)
{
    QPainterPath p;
    p.addRect(rect);
    m_scene->setSelectionArea(p, mode);
}

//void Scene::setSelectionArea(GraphicsPath *path, Qt::ItemSelectionMode mode)
//{
//    m_scene->setSelectionArea(path->path(), mode);
//}

void Scene::onRendererScaleShanged()
{
    emit rendererScaleChanged( m_renderer->scale() );
}

void Scene::mousePressEvent(QMouseEvent *event)
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
        //        m_scene->clearSelection();
    }





    //#ifndef QT_NO_RUBBERBAND
    //    if (dragMode == QGraphicsView::RubberBandDrag && !d->rubberBanding) {
    //        if (d->sceneInteractionAllowed) {
    //            // Rubberbanding is only allowed in interactive mode.
    //            event->accept();
    //            d->rubberBanding = true;
    //            d->rubberBandRect = QRect();
    //            if (d->scene) {
    //                // Initiating a rubber band always clears the selection.
    //                d->scene->clearSelection();
    //            }
    //        }
    //    } else
    //#endif

    //        if (d->dragMode == QGraphicsView::ScrollHandDrag && event->button() == Qt::LeftButton) {
    //            // Left-button press in scroll hand mode initiates hand scrolling.
    //            event->accept();
    //            d->handScrolling = true;
    //            d->handScrollMotions = 0;
    //#ifndef QT_NO_CURSOR
    //            viewport()->setCursor(Qt::ClosedHandCursor);
    //#endif
    //        }
}

void Scene::mouseReleaseEvent(QMouseEvent *event)
{
    //    if( !m_enabled ) return;
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

    // Update the last mouse event selected state.

    //#ifndef QT_NO_CURSOR
    //    if (mouseEvent.isAccepted() && mouseEvent.buttons() == 0 && viewport()->testAttribute(Qt::WA_SetCursor)) {
    //        // The last mouse release on the viewport will trigger clearing the cursor.
    //        d->_q_unsetViewportCursor();
    //    }
    //#endif
}

void Scene::mouseMoveEvent(QMouseEvent *event)
{
    //    if( !m_enabled ) return;
    mouseMoveEventHandler(event);
}

void Scene::hoverMoveEvent(QHoverEvent *event)
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

void Scene::hoverEnterEvent(QHoverEvent *event)
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

void Scene::hoverLeaveEvent(QHoverEvent *event)
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

bool Scene::antialiasing() const
{
    qDebug() << "antialiasing";
    return m_renderer->antialiasing();
}

//bool Scene::enabled() const
//{
//    return m_enabled;
//}

void Scene::childEvent(QChildEvent *e)
{
    //    SceneItem *child = qobject_cast<SceneItem*>(e->child());
    //    if (child) {
    ////        qDebug() << "Added:" << e->added() << "Removed:" << e->removed() << "Polished:" <<e->polished() << "Child:" << e->child();
    //        if (e->added()) {
    ////            qDebug() << "Adding SceneItem" << child << "to Scene";
    //            if (child->scene() != this)
    //                addItem(child);
    //        } else if (e->removed()) {
    ////            qDebug() << "SceneItem" << child << "was removed from Scene" << (child->scene() == nullptr);
    //        }
    //        e->accept();
    //    } else
    e->ignore();
}

//QPointF Scene::mapped(QPointF p) const
//{
//    return m_viewTransform.inverted().map(p);
//}

void Scene::setViewport(QQuickItem *arg)
{
    if (m_viewport == arg)
        return;
    if (m_viewport) {
        m_viewport->disconnect(0, this, 0);
    }
    m_viewport = arg;
    connect(m_viewport, SIGNAL(widthChanged()), this, SLOT(adjustView()));
    connect(m_viewport, SIGNAL(heightChanged()), this, SLOT(adjustView()));
    emit viewportChanged();
}

void Scene::setAntialiasing(bool arg)
{
    if (m_renderer->antialiasing() == arg)
        return;

    m_renderer->setAntialiasing(arg);
    emit antialiasingChanged(arg);
}

void Scene::setAutoUpdate(bool arg)
{
    if (m_autoUpdate == arg)
        return;

    m_autoUpdate = arg;
    emit autoUpdateChanged(arg);
}

void Scene::setEnableBsp(bool arg)
{
    if (enableBsp() == arg)
        return;
    m_scene->setItemIndexMethod(arg ? QGraphicsScene::BspTreeIndex : QGraphicsScene::NoIndex);
    emit enableBspChanged(arg);
}

void Scene::setBspTreeDepth(int arg)
{
    if (m_scene->bspTreeDepth() == arg)
        return;

    m_scene->setBspTreeDepth(arg);
    emit bspTreeDepthChanged(arg);
}

bool isExactScale(qreal scale)
{
    return (fmod(scale, 1) == 0 || fmod(1/scale, 1) == 0 || fmod(scale/1.5, 1) == 0 || fmod(1.5/scale, 1) == 0);
}


void Scene::adjustView()
{
    if (!isVisible() || !m_autoUpdate) {
        return;
    }
    if (QDateTime::currentMSecsSinceEpoch() - lastUpdate < 33) {
        return;
    }
//    el.start();
    QTransform newTr;
    //    m_viewTransform.reset();
    QPointF o(0, 0);
    QPolygonF dst(sceneRect());
    dst.removeLast();

    QPolygonF src;
    src << mapToScene(sceneRect().topLeft()) << mapToScene(sceneRect().topRight()) << mapToScene(sceneRect().bottomRight()) << mapToScene(sceneRect().bottomLeft());

    if (!QTransform::quadToQuad(dst, src, newTr))
        qWarning() << "Illegal geometry!" << src << dst;
    if (newTr == m_viewTransform && !m_renderer->boundingRect().isEmpty()) {
        return;
    }
    lastUpdate = QDateTime::currentMSecsSinceEpoch();
//        if (!isExactScale(newTr.m11())) {
//            qreal r = newTr.m11() / m_viewTransform.m11();
//            if (r < 1)
//                r = 1/r;
//            if (r < 1.5)
//                return;
//        }
    QRectF viewportRect;
    if (m_viewport) {
        QPointF vpUL = o;
        QPointF vpBR = QPointF(viewport()->width(), viewport()->height());
        QRectF vprect = QRectF(mapFromItem(viewport(), vpUL), mapFromItem(viewport(), vpBR));
        viewportRect = mapRectToScene(vprect);
    } else {
        viewportRect = QRectF(o, QSizeF(window()->geometry().size()));
    }
    QRectF sceneVpRect = src.boundingRect();
    QRectF visibleRect = viewportRect.intersected(sceneVpRect);
    QRectF painterRect = mapRectFromScene(visibleRect);

    //    QSignalBlocker block(m_renderer);

//    if (viewportRect.contains(visibleRect.adjusted(-1, -1, 1, 1)))
//        return;
    m_renderer->setSize(visibleRect.size());
//    m_renderer->setHeight(visibleRect.height());
    m_renderer->setPosition(painterRect.topLeft());
    if (m_viewTransform.m11() != newTr.m11()) {
        m_renderer->setScale(1/newTr.m11());
        if (m_notifyViewTransformUpdates) {
            sceneInfo()->viewTransform = m_viewTransform;
            emit viewportScaleChanged(newTr.m11());
        }
    }
    m_renderer->setSceneRect(painterRect);

//    m_renderer->setX(-newTr.m31());
//    m_renderer->setY(-newTr.m32());

    m_viewTransform = newTr;
    emit viewportRectChanged( m_viewTransform.inverted().mapRect( viewport()->boundingRect() ) );
    emit viewTransformChanged(m_viewTransform);

}

void Scene::adjustSceneRect()
{
    m_scene->setSceneRect(0, 0, width(), height());
    //    qDebug() << "Adjusted scene rect" << m_scene->sceneRect();
}

void Scene::initSceneImpl(MyGraphicsScene *scene)
{
    m_scene = scene;
    connect(m_scene, &QGraphicsScene::sceneRectChanged, this, &Scene::sceneRectChanged);
    connect(m_scene, &QGraphicsScene::selectionChanged, this, &Scene::selectionChanged);
    connect(m_scene, &QGraphicsScene::changed, this, &Scene::changed);
    auto sd = new SceneData;
    sd->scene = this;
    m_scene->setUserData(0, sd);
}

void Scene::mouseMoveEventHandler(QMouseEvent *event)
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




//void Scene::setQGraphicsScene(QGraphicsScene *scene)
//{
//    m_scene->disconnect(0, this, 0);
//    m_scene->deleteLater();
//    initSceneImpl(scene);
//}

SceneRenderer *Scene::renderer() const
{
    return m_renderer;
}

bool Scene::autoUpdate() const
{
    return m_autoUpdate;
}

QRectF Scene::viewportRect() const
{
    return m_viewTransform.inverted().mapRect( viewport()->boundingRect() );
}

bool Scene::enableBsp() const
{
    return m_scene->itemIndexMethod() == QGraphicsScene::BspTreeIndex;
}

int Scene::bspTreeDepth() const
{
    return m_scene->bspTreeDepth();
}

qreal Scene::viewportScale() const
{
    return m_viewTransform.m11();
}

bool Scene::contains(const QPointF &point) const
{
    if (!sceneInteractionAllowed)
        return false;
    if (m_scene)
        return m_scene->itemAt(point, QTransform()) != nullptr;
    return false;
}

SceneData *Scene::sceneInfo() const
{
    return static_cast<SceneData*>(m_scene->userData(0));
}

void Scene::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    if (!newGeometry.isEmpty()) {
//        qDebug() << "Adjusting scene view";
        adjustView();
    }
}

bool Scene::notifyViewTransformUpdates() const
{
    return m_notifyViewTransformUpdates;
}

void Scene::setNotifyViewTransformUpdates(bool notifyViewTransformUpdates)
{
    if (m_notifyViewTransformUpdates == notifyViewTransformUpdates)
        return;

    m_notifyViewTransformUpdates = notifyViewTransformUpdates;
    emit notifyViewTransformUpdatesChanged(notifyViewTransformUpdates);
}



QGraphicsScene *Scene::qGraphicsScene() const
{
    return m_scene;
}


//const QTransform &SceneData::currentViewTranform(const QGraphicsScene *scene)
//{
//    QObjectUserData *d = scene->userData(dataId);
//    if (!d)
//        return QTransform();

//    SceneData *sd = static_cast<SceneData*>(d);
//    return sd->viewTransform;
//}


void MyGraphicsScene::renderItems(QPainter *painter, const QRectF &sourceRect, const QRectF &targetRect, QVector<QGraphicsItem *> &itemVector)
{
    int numItems = itemVector.size();
    qreal xratio = targetRect.width() / sourceRect.width();
    qreal yratio = targetRect.height() / sourceRect.height();
    painter->save();
    QTransform painterTransform;
    painterTransform *= QTransform()
                        .scale(xratio, yratio)
                        .translate(-sourceRect.left(), -sourceRect.top());
    painter->setWorldTransform(painterTransform, true);
    drawItems(painter, numItems, itemVector.data(), nullptr);
    painter->restore();
}
