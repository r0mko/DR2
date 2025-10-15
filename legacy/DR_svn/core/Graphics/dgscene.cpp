#include "dgscene.h"
#include <QDebug>

QQmlListProperty<DGSceneItem> DGScene::qmlItems() {
    return QQmlListProperty<DGSceneItem>(this, 0,
                                         &DGScene::qmlItemsAppend,
                                         &DGScene::qmlItemsCount,
                                         &DGScene::qmlItemsAt,
                                         QQmlListProperty<DGSceneItem>::ClearFunction());
}

void DGScene::qmlItemsAppend(QQmlListProperty<DGSceneItem> *list, DGSceneItem *object)
{
    DGScene *o = qobject_cast<DGScene*>(list->object);
    o->m_items.append(object);
    emit o->itemsChanged();
}

int DGScene::qmlItemsCount(QQmlListProperty<DGSceneItem> *list)
{
    DGScene *o = qobject_cast<DGScene*>(list->object);
    return o->m_items.size();
}

DGSceneItem *DGScene::qmlItemsAt(QQmlListProperty<DGSceneItem> *list, int index)
{
    DGScene *o = qobject_cast<DGScene*>(list->object);
    return o->m_items.at(index);
}

DGScene::DGScene(QQuickItem *parent) :
    QQuickItem(parent),
    m_zoom(1),
    m_sceneWidth(0),
    m_sceneHeight(0),
    m_viewportX(0),
    m_viewportY(0),
    m_viewportWidth(0),
    m_viewportHeight(0)
{
}

qreal DGScene::viewportX() const
{
    return m_viewportX;
}

qreal DGScene::viewportY() const
{
    return m_viewportY;
}

qreal DGScene::viewportWidth() const
{
    return m_viewportWidth;
}

qreal DGScene::viewportHeight() const
{
    return m_viewportHeight;
}

void DGScene::addItem(DGSceneItem *item)
{
    if (m_items.contains(item))
        m_items.append(item);
    item->m_scene = this;
}

void DGScene::setZoom(qreal arg)
{
    if (m_zoom != arg) {
        m_zoom = arg;
        emit zoomChanged(arg);
        setWidth(m_sceneWidth * m_zoom);
        setHeight(m_sceneHeight * m_zoom);
    }
}

void DGScene::setSceneWidth(qreal arg)
{
    if (m_sceneWidth != arg) {
        m_sceneWidth = arg;
        setWidth(arg * m_zoom);
        emit sceneWidthChanged(arg);
    }
}

void DGScene::setSceneHeight(qreal arg)
{
    if (m_sceneHeight != arg) {
        m_sceneHeight = arg;
        setHeight(arg * m_zoom);
        emit sceneHeightChanged(arg);
    }
}

void DGScene::setViewportX(qreal arg)
{
    if (m_viewportX != arg) {
        m_viewportX = arg;
        emit viewportXChanged(arg);
    }
}

void DGScene::setViewportY(qreal arg)
{
    if (m_viewportY != arg) {
        m_viewportY = arg;
        emit viewportYChanged(arg);
    }
}

void DGScene::setViewportWidth(qreal arg)
{
    if (m_viewportWidth != arg) {
        m_viewportWidth = arg;
        emit viewportWidthChanged(arg);
    }
}

void DGScene::setViewportHeight(qreal arg)
{
    if (m_viewportHeight != arg) {
        m_viewportHeight = arg;
        emit viewportHeightChanged(arg);
    }
}


void DGScene::childEvent(QChildEvent *e)
{
    DGSceneItem *item = qobject_cast<DGSceneItem *>(e->child());
    if ((e->type() == QChildEvent::ChildAdded) && item) {
        qDebug() << "Item" << e->child()->metaObject()->className() << "added";
        item->setScene(this);
    }
}

QRectF DGScene::contentsBoundingRect() const
{
    return QRectF(0, 0, width(), height());
}


DGSceneItem::DGSceneItem(QQuickItem *parent)
    : DGItem(parent),
      m_scene(0)
{

}

void DGSceneItem::addToScene(DGScene *scene)
{
    Q_UNUSED(scene)

    // FIXME: write an implementation
}


bool DGSceneItem::selected() const
{
    return m_selected;
}

DGScene *DGSceneItem::scene() const
{
    return m_scene;
}

QRectF DGSceneItem::contentsBoundingRect() const
{
    return QRectF(x(), y(), width(), height());
}

void DGSceneItem::setSelected(bool arg)
{
    if (m_selected != arg) {
        m_selected = arg;
        emit selectedChanged(arg);
    }
}

void DGSceneItem::setScene(DGScene *arg)
{
    if (m_scene != arg) {
        m_scene = arg;
        m_scene->addItem(this);
        emit sceneChanged(arg);
    }
}

void DGSceneItem::componentComplete()
{
    QQuickItem::componentComplete();
    DGScene *s = qobject_cast<DGScene *>(parentItem());
    if (s) {
        m_scene = s;

    }
}


