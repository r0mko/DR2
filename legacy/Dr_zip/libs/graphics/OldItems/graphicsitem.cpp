#include "graphicsitem.h"
#include "pen.h"
#include "brush.h"
#include "qgraphicssceneevent.h"
#include "sceneitem.h"
#include "scene.h"
#include <QGraphicsScene>

Brush *QuickGraphicsShapeItem::brush() const
{
    return m_brush;
}

void QuickGraphicsShapeItem::setBrush(Brush *brush)
{
    m_brush = brush;
}

Pen *QuickGraphicsShapeItem::pen() const
{
    return m_pen;
}

void QuickGraphicsShapeItem::setPen(Pen *pen)
{
    m_pen = pen;
}

QPainterPath QuickGraphicsShapeItem::shape() const
{
    qreal vpScale = super()->scene()->viewportScale();
    if (shapeDirty || vpScale != lastScale) {
        if (m_pen) {
            QPainterPathStroker str(m_pen->pen());
            str.setDashPattern(Qt::SolidLine);
            str.setWidth(pen()->width() > m_minimumWidth ? pen()->width() / vpScale : m_minimumWidth / vpScale);
            m_outlinePath = str.createStroke(m_path);
        }
        if (m_brush && m_brush->style() != Qt::NoBrush) {
            m_outlinePath.addPath(m_path);
        }
        shapeDirty = false;
        lastScale = vpScale;
    }
    return m_outlinePath;
}

QRectF QuickGraphicsShapeItem::calculateBoundingRect() const
{
    return m_path.boundingRect();
}


QPainterPath QuickGraphicsShapeItem::path() const
{
    return m_path;
}

QPainterPath &QuickGraphicsShapeItem::pathRef()
{
    return m_path;
}

void QuickGraphicsShapeItem::setShapeDirty()
{
    shapeDirty = true;
    prepareGeometryChange();
}

void QuickGraphicsShapeItem::setPath(const QPainterPath &path)
{
    m_path = path;
    shapeDirty = true;
}

qreal QuickGraphicsShapeItem::minimumWidth() const
{
    return m_minimumWidth;
}

void QuickGraphicsShapeItem::setMinimumWidth(const qreal &minimumWidth)
{
    m_minimumWidth = minimumWidth;
    setShapeDirty();
}

QPainterPath QuickGraphicsShapeItem::outline() const
{
    return m_outlinePath;
}

QuickGraphicsShapeItem::QuickGraphicsShapeItem(SceneItem *item, QGraphicsItem *parent) :
    QuickGraphicsItem(item, parent),
    m_pen(nullptr),
    m_brush(nullptr),
    m_minimumWidth(0),
    lastScale(0),
    shapeDirty(true)
{
}



bool QuickGraphicsItem::isDestroyed() const
{
    return m_isDestroyed;
}
QuickGraphicsItem::~QuickGraphicsItem()
{
    if (m_isDestroyed)
        return;
    super()->checkDeleted();
    qDebug() << "GraphicsItem" << this << "was destroyed";
}

QRectF QuickGraphicsItem::boundingRect() const
{
    if (m_brectDirty || (m_item->flags() & SceneItem::ItemIgnoresTransformations)) {
        m_brect = calculateBoundingRect();
        m_brectDirty = false;
    }
    return m_brect;
}

void QuickGraphicsItem::destroy()
{
    if (m_isDestroyed)
        return;
    m_isDestroyed = true;
    scene()->removeItem(this);
    setData(SceneItem::QMLParent, QVariant(0));
}

bool QuickGraphicsItem::boundingRectDirty() const
{
    return m_brectDirty;
}

void QuickGraphicsItem::setBoundingRectDirty()
{
    m_brectDirty = true;
    prepareGeometryChange();
}

QVariant QuickGraphicsItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (super())
        switch (change) {
        case QGraphicsItem::ItemPositionChange:
            if (super()->flags() & SceneItem::ItemPositionSnapsToGrid) {
                qreal step = super()->gridStep();
                if (step == 0.0)
                    break;
                QPointF p = value.toPointF();
                qreal fx = qRound(p.x() / step) * step;
                qreal fy = qRound(p.y() / step) * step;
                return QVariant(QPointF(fx, fy));
            } else if (super()->flags() & SceneItem::ItemPixelAligned) {
                qreal step = 1 / super()->scene()->viewportScale();
                QPointF p = value.toPointF();
                qreal fx = qRound(p.x() / step) * step;
                qreal fy = qRound(p.y() / step) * step;
                return QVariant(QPointF(fx, fy));
            }
        case QGraphicsItem::ItemPositionHasChanged:
            emit super()->positionChanged();
            break;
        case QGraphicsItem::ItemTransformHasChanged:
            emit super()->transformChanged();
            break;
        case QGraphicsItem::ItemVisibleHasChanged:
            emit super()->visibleChanged();
            break;
        case QGraphicsItem::ItemEnabledHasChanged:
            emit super()->enabledChanged();
            break;
        case QGraphicsItem::ItemSelectedHasChanged:
            emit super()->selectedChanged();
            break;
        case QGraphicsItem::ItemParentHasChanged:
            emit super()->parentChanged();
            break;
        case QGraphicsItem::ItemSceneChange: {
            if (qMetaTypeId<QGraphicsScene*>() == value.userType()) {
                QObject *newScene = value.value<QObject*>();
                if (newScene == nullptr) {
                    if (m_isDestroyed) {
//                        qDebug() << "The item was totally destroyed";
                        super()->checkDeleted();
                        break;
                    }
                    setFlag(QGraphicsItem::ItemHasNoContents);
                    super()->destroyHelper();
                }
            }
            break;
        }
        case QGraphicsItem::ItemSceneHasChanged: {
            QObject *newScene = value.value<QObject*>();
//            qDebug() << "Scene changed" << newScene << super();
            if (newScene != nullptr)
                super()->initScene();
            emit super()->sceneChanged();
            break;
        }
        case QGraphicsItem::ItemCursorHasChanged:
            emit super()->cursorChanged();
            break;
        case QGraphicsItem::ItemToolTipHasChanged:
            emit super()->toolTipChanged();
            break;
        case QGraphicsItem::ItemFlagsHaveChanged:
            emit super()->flagsChanged();
            break;
        case QGraphicsItem::ItemZValueHasChanged:
            emit super()->zValueChanged();
            break;
        case QGraphicsItem::ItemOpacityHasChanged:
            emit super()->opacityChanged();
            break;
        case QGraphicsItem::ItemScenePositionHasChanged:
            emit super()->scenePositionChanged();
            break;
        case QGraphicsItem::ItemRotationHasChanged:
            emit super()->rotationChanged();
            break;
        case QGraphicsItem::ItemScaleHasChanged:
            emit super()->scaleChanged();
            break;
        case QGraphicsItem::ItemTransformOriginPointHasChanged:
            emit super()->transformOriginPointChanged();
            break;
        default:
            break;
        }
    return QGraphicsItem::itemChange(change, value);
}

void QuickGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    super()->setPressed(true);
    super()->pressed(event->buttons(), event->pos());
    QGraphicsItem::mousePressEvent(event);
}

void QuickGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    super()->setPressed(false);
    super()->setDragging(false);
    super()->released(event->pos());
    QGraphicsItem::mouseReleaseEvent(event);
}

void QuickGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    super()->setDragging(true);
    emit super()->moved(event->buttons(), event->pos());
    QGraphicsItem::mouseMoveEvent(event);
}

void QuickGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if (!super()->hoverEnabled()) {
        event->ignore();
        return;
    }
    super()->setHovered(true);
    QGraphicsItem::hoverEnterEvent(event);
}

void QuickGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if (!super()->hoverEnabled()) {
        event->ignore();
        return;
    }
    super()->setHovered(false);
    QGraphicsItem::hoverLeaveEvent(event);
}

void QuickGraphicsItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if (!super()->hoverEnabled()) {
        event->ignore();
        return;
    }
    QGraphicsItem::hoverMoveEvent(event);
    emit super()->hoverPositionChanged(event->pos());
}

void QuickGraphicsItem::updateBoundingRect(const QRectF &rect)
{
    m_brectDirty = false;
    m_brect = rect;
}

