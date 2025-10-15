#include "sceneitem.h"
#include "scene.h"
#include "qgraphicssceneevent.h"
#include "qgraphicsscene.h"
#include <QDebug>
#include <QQmlEngine>

SceneItem::SceneItem(SceneItem *parent) :
    QObject(parent),
    m_item(nullptr),
    m_hovered(false),
    m_pressed(false),
    m_dragging(false),
    isDestroying(false)

{
}

SceneItem::~SceneItem()
{
//    qDebug() << "Scene item destroyed" << this << isDestroying;

    if (isDestroying)
        return;
    if(m_item) {
//        qDebug() << "killing item" << m_item;
        m_item->destroy();
//        m_item->scene()->removeItem(m_item);
    }
    //        scene()->removeItem(this);
}

QRectF SceneItem::rect() const
{
    return m_rect;
}

QPointF SceneItem::position() const
{
    return m_item->pos();
}

qreal SceneItem::x() const
{
    return m_item->x();
}

qreal SceneItem::y() const
{
    return m_item->y();
}

QSizeF SceneItem::size() const
{
    return m_item->boundingRect().size();
}

qreal SceneItem::width() const
{
    return m_item->boundingRect().width();
}

qreal SceneItem::height() const
{
    return m_item->boundingRect().height();
}

qreal SceneItem::rotation() const
{
    return m_item->rotation();
}

qreal SceneItem::scale() const
{
    return m_item->scale();
}

qreal SceneItem::opacity() const
{
    return m_item->opacity();
}

QPointF SceneItem::transformOriginPoint() const
{
    return m_item->transformOriginPoint();
}

bool SceneItem::visible() const
{
    return m_item->isVisible();
}

bool SceneItem::hoverEnabled() const
{
    return m_item->acceptHoverEvents();
}

Qt::MouseButtons SceneItem::acceptedButtons() const
{
    return m_item->acceptedMouseButtons();
}

bool SceneItem::isSelected() const
{
    return m_item->isSelected();
}

bool SceneItem::isHovered() const
{
    return m_hovered;
}

bool SceneItem::isPressed() const
{
    return m_pressed;
}

bool SceneItem::dragging() const
{
    return m_dragging;
}

QVariant SceneItem::data() const
{
    return m_item->data(UserData);
}

qreal SceneItem::z() const
{
    return m_item->zValue();
}

bool SceneItem::clip() const
{
    return m_item->hasFocus();
}

QRectF SceneItem::boundingRect() const
{
    return m_item->boundingRect();
}

QPointF SceneItem::mapFromItem(const SceneItem *item, const QPointF &point) const
{
    return m_item->mapFromItem(item->m_item, point);
}

QPolygonF SceneItem::mapFromItem(const SceneItem *item, const QRectF &rect) const
{
    return m_item->mapFromItem(item->m_item, rect);
}

QPolygonF SceneItem::mapFromItem(const SceneItem *item, const QPolygonF &polygon) const
{
    return m_item->mapFromItem(item->m_item, polygon);
}

QPainterPath SceneItem::mapFromItem(const SceneItem *item, const QPainterPath &path) const
{
    return m_item->mapFromItem(item->m_item, path);
}

QPolygonF SceneItem::mapFromItem(const SceneItem *item, qreal x, qreal y, qreal w, qreal h) const
{
    return m_item->mapFromItem(item->m_item, x, y, w, h);
}

QPointF SceneItem::mapFromItem(const SceneItem *item, qreal x, qreal y) const
{
    return m_item->mapFromItem(item->m_item, x, y);
}

QPointF SceneItem::mapFromParent(const QPointF &point) const
{
    return m_item->mapFromParent(point);
}

QPolygonF SceneItem::mapFromParent(const QRectF &rect) const
{
    return m_item->mapFromParent(rect);
}

QPolygonF SceneItem::mapFromParent(const QPolygonF &polygon) const
{
    return m_item->mapFromParent(polygon);
}

QPainterPath SceneItem::mapFromParent(const QPainterPath &path) const
{
    return m_item->mapFromParent(path);
}

QPolygonF SceneItem::mapFromParent(qreal x, qreal y, qreal w, qreal h) const
{
    return m_item->mapFromParent(x, y, w, h);
}

QPointF SceneItem::mapFromParent(qreal x, qreal y) const
{
    return m_item->mapFromParent(x, y);
}

QPointF SceneItem::mapFromScene(const QPointF &point) const
{
    return m_item->mapFromScene(point);
}

QPolygonF SceneItem::mapFromScene(const QRectF &rect) const
{
    return m_item->mapFromScene(rect);
}

QPolygonF SceneItem::mapFromScene(const QPolygonF &polygon) const
{
    return m_item->mapFromScene(polygon);
}

QPainterPath SceneItem::mapFromScene(const QPainterPath &path) const
{
    return m_item->mapFromScene(path);
}

QPolygonF SceneItem::mapFromScene(qreal x, qreal y, qreal w, qreal h) const
{
    return m_item->mapFromScene(x, y, w, h);
}

QPointF SceneItem::mapFromScene(qreal x, qreal y) const {
    return m_item->mapFromScene(x, y);
}

QRectF SceneItem::mapRectFromItem(const SceneItem *item, const QRectF &rect) const
{
    return m_item->mapRectFromItem(item->m_item, rect);
}

QRectF SceneItem::mapRectFromItem(const SceneItem *item, qreal x, qreal y, qreal w, qreal h) const
{
    return m_item->mapRectFromItem(item->m_item, x, y, w, h);
}

QRectF SceneItem::mapRectFromParent(const QRectF &rect) const
{
    return m_item->mapRectFromParent(rect);
}

QRectF SceneItem::mapRectFromParent(qreal x, qreal y, qreal w, qreal h) const
{
    return m_item->mapRectFromParent(x, y, w, h);
}

QRectF SceneItem::mapRectFromScene(const QRectF &rect) const
{
    return m_item->mapRectFromScene(rect);
}

QRectF SceneItem::mapRectFromScene(qreal x, qreal y, qreal w, qreal h) const
{
    return m_item->mapRectFromScene(x, y, w, h);
}

QRectF SceneItem::mapRectToItem(const SceneItem *item, const QRectF &rect) const
{
    return m_item->mapRectToItem(item->m_item, rect);
}

QRectF SceneItem::mapRectToItem(const SceneItem *item, qreal x, qreal y, qreal w, qreal h) const
{
    return m_item->mapRectToItem(item->m_item, x, y, w, h);
}

QRectF SceneItem::mapRectToParent(const QRectF &rect) const
{
    return m_item->mapRectToParent(rect);
}

QRectF SceneItem::mapRectToParent(qreal x, qreal y, qreal w, qreal h) const
{
    return m_item->mapRectToParent(x, y, w, h);
}

QRectF SceneItem::mapRectToScene(const QRectF &rect) const
{
    return m_item->mapRectToScene(rect);
}

QRectF SceneItem::mapRectToScene(qreal x, qreal y, qreal w, qreal h) const
{
    return m_item->mapRectToScene(x, y, w, h);
}

QPointF SceneItem::mapToItem(const SceneItem *item, const QPointF &point) const
{
    return m_item->mapToItem(item->m_item, point);
}

QPolygonF SceneItem::mapToItem(const SceneItem *item, const QRectF &rect) const
{
    return m_item->mapToItem(item->m_item, rect);
}

QPolygonF SceneItem::mapToItem(const SceneItem *item, const QPolygonF &polygon) const
{
    return m_item->mapToItem(item->m_item, polygon);
}

QPainterPath SceneItem::mapToItem(const SceneItem *item, const QPainterPath &path) const
{
    return m_item->mapToItem(item->m_item, path);
}

QPolygonF SceneItem::mapToItem(const SceneItem *item, qreal x, qreal y, qreal w, qreal h) const
{
    return m_item->mapToItem(item->m_item, x, y, w, h);
}

QPointF SceneItem::mapToItem(const SceneItem *item, qreal x, qreal y) const
{
    return m_item->mapToItem(item->m_item, x, y);
}

QPointF SceneItem::mapToParent(const QPointF &point) const
{
    return m_item->mapToParent(point);
}

QPolygonF SceneItem::mapToParent(const QRectF &rect) const
{
    return m_item->mapToParent(rect);
}

QPolygonF SceneItem::mapToParent(const QPolygonF &polygon) const
{
    return m_item->mapToParent(polygon);
}

QPainterPath SceneItem::mapToParent(const QPainterPath &path) const
{
    return m_item->mapToParent(path);
}

QPolygonF SceneItem::mapToParent(qreal x, qreal y, qreal w, qreal h) const
{
    return m_item->mapToParent(x, y, w, h);
}

QPointF SceneItem::mapToParent(qreal x, qreal y) const
{
    return m_item->mapToParent(x, y);
}

QPointF SceneItem::mapToScene(const QPointF &point) const
{
    return m_item->mapToScene(point);
}

QPolygonF SceneItem::mapToScene(const QRectF &rect) const
{
    return m_item->mapToScene(rect);
}

QPolygonF SceneItem::mapToScene(const QPolygonF &polygon) const
{
    return m_item->mapToScene(polygon);
}

QPainterPath SceneItem::mapToScene(const QPainterPath &path) const
{
    return m_item->mapToScene(path);
}

QPolygonF SceneItem::mapToScene(qreal x, qreal y, qreal w, qreal h) const
{
    return m_item->mapToScene(x, y, w, h);
}

QPointF SceneItem::mapToScene(qreal x, qreal y) const
{
    return m_item->mapToScene(x, y);
}

Cursor *SceneItem::cursor() const
{
    return m_cursor;
}

Scene *SceneItem::scene() const
{
    if (item()->scene())
        return qobject_cast<Scene*>(item()->scene()->parent());
    return nullptr;
}

qreal SceneItem::gridStep() const
{
    return m_gridStep;
}

SceneItem *SceneItem::parentItem() const
{
    return m_item->parentItem()->data(QMLParent).value<SceneItem*>();
}

void SceneItem::moveBy(qreal dx, qreal dy)
{
    m_item->moveBy(dx, dy);
}

void SceneItem::moveBy(QVector2D vec)
{
    m_item->moveBy(vec.x(), vec.y());
}

void SceneItem::moveBy(QPointF pt)
{
    m_item->moveBy(pt.x(), pt.y());
}

void SceneItem::update(const QRectF &rect)
{
    m_item->update(rect);
}

void SceneItem::setFlags(SceneItemFlags arg)
{
    int f = (int)arg;
    if (m_flags == arg)
        return;
    m_item->setFlags((QGraphicsItem::GraphicsItemFlags)(f & (~QuickGraphicsFlagsMask)));
    m_flags = arg;
    emit flagsChanged(arg);
}

void SceneItem::setScene(Scene *arg)
{
    if (qobject_cast<Scene*>(item()->scene()) == arg)
        return;
    if (arg)
        initScene();
    arg->addItem(this);
}

void SceneItem::setCursor(Cursor *arg)
{
    if (m_cursor == arg)
        return;

    m_cursor = arg;
    item()->setCursor(m_cursor->cursor());
    emit cursorChanged(arg);
}

void SceneItem::setGridStep(qreal arg)
{
    if (m_gridStep == arg)
        return;

    m_gridStep = arg;
    emit gridStepChanged(arg);
}

void SceneItem::setParentItem(SceneItem *arg)
{
    if (m_item->parentItem()->data(QMLParent).value<SceneItem*>() == arg)
        return;

    m_item->setParentItem(arg->m_item);
    setParent(arg);
    emit parentItemChanged(arg);
}

SceneItem::SceneItemFlags SceneItem::flags() const
{
    return m_flags;
}

SceneItem::SceneItem(QuickGraphicsItem *item, SceneItem *parent) :
    QObject(parent),
    m_item(item),
    m_hovered(false),
    m_pressed(false),
    m_dragging(false),
    isDestroying(false)
{
    if (this->parent()) {
        m_item->setParentItem(parent->item());
    }
    m_item->setData(QMLParent, QVariant::fromValue(this));
}

void SceneItem::componentComplete()
{

}

void SceneItem::classBegin()
{

}

void SceneItem::setPosition(QPointF arg)
{
    if (m_item->pos() != arg) {
        m_item->setPos(arg);
        m_rect.moveTopLeft(arg);
    }
}

void SceneItem::setX(qreal arg)
{
    if (m_item->x() != arg)
        m_item->setX(arg);

    m_rect.moveLeft(arg);
    emit xChanged(arg);
}

void SceneItem::setY(qreal arg)
{
    if (m_item->y() != arg)
        m_item->setY(arg);
    m_rect.moveTop(arg);
    emit yChanged(arg);

}

void SceneItem::setHeight(qreal arg)
{
    if (m_rect.height() == arg)
        return;
    m_rect.setHeight(arg);
    emit heightChanged(arg);
}

void SceneItem::setRotation(qreal arg)
{
    m_item->setRotation(arg);
}

void SceneItem::setScale(qreal arg)
{
    if (m_item->scale() == arg)
        return;
    m_item->setScale(arg);
}

void SceneItem::setOpacity(qreal arg)
{
    m_item->setOpacity(arg);
}

void SceneItem::setTransformOriginPoint(QPointF arg)
{
    m_item->setTransformOriginPoint(arg);
}

void SceneItem::setVisible(bool arg)
{
    m_item->setVisible(arg);
}

void SceneItem::setHoverEnabled(bool arg)
{
    m_item->setAcceptHoverEvents(arg);
}

void SceneItem::setAcceptedButtons(Qt::MouseButtons arg)
{
    m_item->setAcceptedMouseButtons(arg);
    emit acceptedButtonsChanged(arg);
}

void SceneItem::setSelected(bool arg)
{
    m_item->setSelected(arg);
}

void SceneItem::setData(QVariant arg)
{
    m_item->setData(UserData, arg);
    emit dataChanged(arg);
}

void SceneItem::setZ(qreal arg)
{
    if (m_item->zValue() == arg)
        return;
    m_item->setZValue(arg);
}

void SceneItem::setClip(bool arg)
{
    if (!m_item->flags() & QGraphicsItem::ItemClipsChildrenToShape)
        m_item->setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
    emit clipChanged(arg);
}

void SceneItem::setRect(QRectF arg)
{
    if (m_rect == arg)
        return;

    m_rect = arg;
    emit rectChanged(arg);
}

void SceneItem::setSize(QSizeF arg)
{
    if (m_rect.size() == arg)
        return;
    m_rect.setSize(arg);
    emit sizeChanged(arg);
}

void SceneItem::setWidth(qreal arg)
{
    if (m_rect.width() == arg)
        return;
    m_rect.setWidth(arg);
    emit widthChanged(arg);
}
QGraphicsItem *SceneItem::item() const
{
    return m_item;
}


void SceneItem::checkDeleted()
{
//    delete m_item;
    m_item = nullptr;
}

void SceneItem::destroyHelper()
{
//    qDebug() << "Graphics item" << m_item << "removed from scene" << m_item->scene();
//    isDestroying = true;
//    m_item = nullptr;
//    deleteLater();

    //    delete m_item;
    //    m_item = nullptr;
    //    if (QQmlEngine::objectOwnership(this) == QQmlEngine::CppOwnership) {
    //        qDebug() << "Also deleting wrapper" << this << m_item;
    //        deleteLater();
    //    }
}








QQmlListProperty<SceneItem> SceneItem::qmlChildItems() {
    return QQmlListProperty<SceneItem>(this, 0,
                                       &SceneItem::qmlChildItemsAppend,
                                       &SceneItem::qmlChildItemsCount,
                                       &SceneItem::qmlChildItemsAt,
                                       QQmlListProperty<SceneItem>::ClearFunction());
}

void SceneItem::qmlChildItemsAppend(QQmlListProperty<SceneItem> *list, SceneItem *object)
{
    SceneItem *o = qobject_cast<SceneItem*>(list->object);
    if (o)
        object->m_item->setParentItem(o->m_item);
}

int SceneItem::qmlChildItemsCount(QQmlListProperty<SceneItem> *list)
{
    SceneItem *o = qobject_cast<SceneItem*>(list->object);
    if (o)
        return o->m_item->childItems().size();
    return -1;
}

SceneItem *SceneItem::qmlChildItemsAt(QQmlListProperty<SceneItem> *list, int index)
{
    SceneItem *o = qobject_cast<SceneItem*>(list->object);
    return o->m_item->childItems().at(index)->data(QMLParent).value<SceneItem*>();
}









