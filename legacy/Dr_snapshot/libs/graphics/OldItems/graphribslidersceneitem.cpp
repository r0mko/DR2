#include "graphribslidersceneitem.h"
#include <QPainter>
#include "pen.h"
#include "brush.h"
#include "../cartograph/defs.h"
#include "scene.h"

//GraphRibSlider::GraphRibSlider(QObject *parent) :
//    SceneItem(new PathGraphicsItem(this), parent)
//{
//}


GraphRibSlider::GraphRibSlider(SceneItem *parent) :
    SceneItem(new GraphRibSliderItem(this), parent)
{
    Shape *h = new Shape(this);
    h->addRect(0, 0, 12, 12);
    //    h->addEllipse(0, 0, 18, 18);
    //    h->addRect(4, 4, 10, 10);
    h->translate(-h->boundingRect().center());
    item()->setHandle(h);

    connect(this, &SceneItem::sceneChanged, this, &GraphRibSlider::updateSceneConnections);
    //    setHotPoint(h->boundingRect().center());
}


GraphRibSceneItem *GraphRibSlider::rib() const
{
    return item()->rib();
}

qreal GraphRibSlider::pathPosition() const
{
    return item()->pathPosition();
}

void GraphRibSlider::setRib(GraphRibSceneItem *arg)
{
    if (item()->rib() == arg)
        return;

    item()->setRib(arg);
    emit ribChanged(arg);
}

void GraphRibSlider::setPathPosition(qreal arg)
{
    if (item()->pathPosition() == arg)
        return;
    item()->setPathPosition(arg);
}

Pen *GraphRibSlider::pen() const
{
    return item()->pen();
}

Brush *GraphRibSlider::brush() const
{
    return item()->brush();
}

Shape *GraphRibSlider::handle() const
{
    return item()->handle();
}

bool GraphRibSlider::bindToPath() const
{
    return item()->bindToPath();
}

bool GraphRibSlider::atBeginning() const
{
    return item()->atBeginning();
}

bool GraphRibSlider::atEnd() const
{
    return item()->atEnd();
}

bool GraphRibSlider::isFrozen() const
{
    return item()->frozen();
}

void GraphRibSlider::setPen(Pen *arg)
{
    if (item()->pen() == arg)
        return;
    item()->setPen(arg);
    emit penChanged(arg);
}

void GraphRibSlider::setBrush(Brush *arg)
{
    if (item()->brush() == arg)
        return;
    item()->setBrush(arg);
    emit brushChanged(arg);
}

void GraphRibSlider::setHandle(Shape *arg)
{
    if (item()->handle() == arg)
        return;
    item()->setHandle(arg);

    emit handleChanged(arg);
}

void GraphRibSlider::setBindToPath(bool arg)
{
    if (item()->bindToPath() == arg)
        return;

    item()->setBindToPath(arg);
    emit bindToPathChanged(arg);
}

void GraphRibSlider::setFrozen(bool arg)
{
    if (item()->frozen() == arg)
        return;

    item()->setFrozen(arg);
    emit frozenChanged(arg);
}

void GraphRibSlider::setNextSlider(GraphRibSlider *arg)
{
    if (item()->nextSlider() == arg)
        return;

    item()->setNextSlider(arg);
    emit nextSliderChanged(arg);
}

void GraphRibSlider::setOriginNodeId(int arg)
{
    if (item()->originNodeId() == arg)
        return;

    item()->setOriginNodeId(arg);
    emit originNodeIdChanged(arg);
}

void GraphRibSlider::setInPosition(qreal arg)
{
    item()->setInPosition(arg);
}

void GraphRibSlider::setOutPosition(qreal arg)
{
    if (item()->outPosition() == arg)
        return;

    item()->setOutPosition(arg);
    emit outPositionChanged(arg);
}

qreal GraphRibSlider::distanceTo(qreal x, qreal y) const
{
    return distanceTo(QPointF(x, y));
}

qreal GraphRibSlider::distanceTo(const QPointF &pos) const
{
    return QLineF(position(), pos).length();
}

qreal GraphRibSlider::distanceTo(GraphRibSlider *other) const
{
    return distanceTo(other->position());
}

QVariant GraphRibSlider::buildPolygon() const
{
    QPolygonF ret;
    const GraphRibSlider *s = this;
    while (s) {
        if (!s->rib()) {
            qDebug() << "vile obscenity: no rib set for slider" << s;
            s = s->nextSlider();
            continue;
        }
        qreal stPos = s->inPosition();
        qreal endPos = s->outPosition();
        if (endPos < 0)
            endPos = s->pathPosition();
//        polyLineMerge(ret, s->rib()->guide()->getPolylinePart(stPos, endPos));
        s = s->nextSlider();
    }
//    qDebug() << "Polygon from slider:" << ret;
    return ret;
}

QVariant GraphRibSlider::buildPointList() const
{
    QPolygonF p = buildPolygon().value<QPolygonF>();
    QVariantList ret;
    for (QPointF pt:p) {
        ret.append(pt);
    }
    return ret;
}



QPointF GraphRibSlider::desiredPos() const
{
    return item()->desiredPos();
}

GraphRibSlider *GraphRibSlider::nextSlider() const
{
    return item()->nextSlider();
}

int GraphRibSlider::originNodeId() const
{
    return item()->originNodeId();
}

qreal GraphRibSlider::inPosition() const
{
    return item()->inPosition();
}

qreal GraphRibSlider::outPosition() const
{
    return item()->outPosition();
}

void GraphRibSlider::updateSceneConnections()
{
    if (scene()) {
        connect(scene(), &Scene::viewportScaleChanged, this, &GraphRibSlider::updateBounds);
        updateBounds(scene()->viewportScale());
    }
}


void GraphRibSlider::updateBounds(qreal scale)
{
    item()->setVscale(scale);
}

void GraphRibSlider::polyLineMerge(QPolygonF &dst, const QPolygonF &src) const
{
    if(!dst.isEmpty() && dst.last() == src.first())
        dst.removeLast();
    for (const QPointF &p:src) {
        dst.append(p);
    }
}


Pen *GraphRibSlider::GraphRibSliderItem::pen() const
{
    return m_pen;
}

void GraphRibSlider::GraphRibSliderItem::setPen(Pen *pen)
{
    m_pen = pen;
    update();
}

Brush *GraphRibSlider::GraphRibSliderItem::brush() const
{
    return m_brush;
}

void GraphRibSlider::GraphRibSliderItem::setBrush(Brush *brush)
{
    m_brush = brush;
    update();
}

Shape *GraphRibSlider::GraphRibSliderItem::handle() const
{
    return m_handle;
}

void GraphRibSlider::GraphRibSliderItem::setHandle(Shape *handle)
{
    m_handle = handle;
    prepareGeometryChange();
    //    qDebug() << "handle bounds" << m_handle->boundingRect() << "pos" << pos() << "visible:" << isVisible();
}

QVariant GraphRibSlider::GraphRibSliderItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (m_rib) {
        switch (change) {
        case QGraphicsItem::ItemPositionChange: {
            m_desiredPos = value.toPointF();
            emit sup->desiredPosChanged(m_desiredPos);
            if (m_frozen) {
                return pos();
            }
            qreal len;
            QPointF rpt = m_rib->guide()->vectorPath().nearestPoint(m_desiredPos, &len);
            m_pathPos = len;
//            qDebug() << "Desired pos" << m_desiredPos << rpt << QLineF(rpt, m_desiredPos).length();
            emit sup->pathPositionChanged(m_pathPos);
            if (rpt == m_rib->polygon().first()) {
                //                qDebug() << sup << "at beginning of" << m_rib->property("modelIdx").toInt() << len << m_rib->length();
                setAtBeginning(true);
                setAtEnd(false);
            } else if (rpt == m_rib->polygon().last()) {
                //                qDebug() << sup << "at end of" << m_rib->property("modelIdx").toInt() << len << m_rib->length();
                setAtBeginning(false);
                setAtEnd(true);
            } else {
                setAtBeginning(false);
                setAtEnd(false);
            }
            if (!m_bindToPath)
                break;
            return rpt;
            break;
        }

        default:
            break;
        }
    }
    return QuickGraphicsItem::itemChange(change, value);

}


qreal GraphRibSlider::GraphRibSliderItem::vscale() const
{
    return m_vscale;
}

bool GraphRibSlider::GraphRibSliderItem::atBeginning() const
{
    return m_atBeginning;
}

bool GraphRibSlider::GraphRibSliderItem::atEnd() const
{
    return m_atEnd;
}

void GraphRibSlider::GraphRibSliderItem::setVscale(const qreal &vscale)
{
    m_vscale = vscale;
    prepareGeometryChange();
}

void GraphRibSlider::GraphRibSliderItem::setAtBeginning(bool arg)
{
    if (m_atBeginning == arg)
        return;

    m_atBeginning = arg;
    emit sup->atBeginningChanged(arg);
}

void GraphRibSlider::GraphRibSliderItem::setAtEnd(bool arg)
{
    if (m_atEnd == arg)
        return;

    m_atEnd = arg;
    emit sup->atEndChanged(arg);
}

QPointF GraphRibSlider::GraphRibSliderItem::desiredPos() const
{
    return m_desiredPos;
}

void GraphRibSlider::GraphRibSliderItem::setNextSlider(GraphRibSlider *nextSlider)
{
    m_nextSlider = nextSlider;
}

int GraphRibSlider::GraphRibSliderItem::originNodeId() const
{
    return m_originNodeId;
}

void GraphRibSlider::GraphRibSliderItem::setOriginNodeId(int originNodeId)
{
    m_originNodeId = originNodeId;
}

qreal GraphRibSlider::GraphRibSliderItem::outPosition() const
{
    return m_outPosition;
}

void GraphRibSlider::GraphRibSliderItem::setOutPosition(const qreal &outPosition)
{
    m_outPosition = outPosition;
}

GraphRibSlider *GraphRibSlider::GraphRibSliderItem::nextSlider() const
{
    return m_nextSlider;
}

bool GraphRibSlider::GraphRibSliderItem::bindToPath() const
{
    return m_bindToPath;
}

void GraphRibSlider::GraphRibSliderItem::setBindToPath(bool bindToPath)
{
    m_bindToPath = bindToPath;
}

bool GraphRibSlider::GraphRibSliderItem::frozen() const
{
    return m_frozen;
}

qreal GraphRibSlider::GraphRibSliderItem::inPosition() const
{
    return m_inPosition;
}

void GraphRibSlider::GraphRibSliderItem::setInPosition(qreal arg)
{
    if (m_inPosition == arg)
        return;

    m_inPosition = arg;
    emit sup->inPositionChanged(arg);
}

void GraphRibSlider::GraphRibSliderItem::setFrozen(bool frozen)
{
    m_frozen = frozen;
}





GraphRibSlider::GraphRibSliderItem::GraphRibSliderItem(SceneItem *item, QGraphicsItem *parent)
    : QuickGraphicsShapeItem(item, parent),
      m_rib(nullptr),
      m_pathPos(0),
      m_pen(nullptr),
      m_brush(nullptr),
      m_vscale(1),
      m_atBeginning(false),
      m_atEnd(false),
      m_bindToPath(true),
      m_frozen(false),
      m_nextSlider(nullptr),
      m_originNodeId(-1),
      m_inPosition(-1),
      m_outPosition(-1)
{
    setBoundingRectDirty();
}

QRectF GraphRibSlider::GraphRibSliderItem::calculateBoundingRect() const
{
    if (m_handle) {
        qreal scale = 1/m_vscale;
        QRectF rect = m_handle->boundingRect();
        QRectF mrect(rect.x() * scale, rect.y() * scale, rect.width() * scale, rect.height() * scale);
        return mrect;
    }
    return QRectF();
}

void GraphRibSlider::GraphRibSliderItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    //    qreal scale = super()->scene()->viewportScale();

    painter->scale(1/m_vscale, 1/m_vscale);
    //    painter->translate(-sup->hotPoint());
    if (!m_handle)
        return;
    if (!m_pen && !m_brush)
        return;
    if (m_pen)
        painter->setPen(m_pen->pen());
    if (m_brush)
        painter->setBrush(m_brush->brush());
    painter->drawPath(m_handle->path());
}

GraphRibSceneItem *GraphRibSlider::GraphRibSliderItem::rib() const
{
    return m_rib;
}

void GraphRibSlider::GraphRibSliderItem::setRib(GraphRibSceneItem *rib)
{
    m_rib = rib;
}
qreal GraphRibSlider::GraphRibSliderItem::pathPosition() const
{
    return m_pathPos;
}

void GraphRibSlider::GraphRibSliderItem::setPathPosition(const qreal &position)
{
    m_pathPos = position;
    emit sup->pathPositionChanged(m_pathPos);
    int node;
    m_bindToPath = false;
    setPos(m_rib->guide()->vectorPath().pointAtLength(position, &node));
    m_bindToPath = true;
    if (node == m_rib->polygon().size() - 1) {
        // TODO: at end
    } else if (node == 0) {
        // TODO: at beginning
    }
}
