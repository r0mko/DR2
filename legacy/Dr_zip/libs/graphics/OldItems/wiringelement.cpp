#include "wiringelement.h"
#include <QPainter>
#include "pen.h"
#include "brush.h"
#include "scene.h"
WiringElement::WiringElement(SceneItem *parent)
    : SceneItem(new WiringElementItem(this), parent),
      m_zoomFactor(1),
      m_fixedScale(false)
{

}

WiringElement::~WiringElement()
{
//    qDebug() << "Wiring element destroyed" << this;
}

bool WiringElement::drawOutline() const
{
    return gItem->m_drawOutline;
}


void WiringElement::setDrawOutline(bool arg)
{
    if (gItem->m_drawOutline == arg)
        return;

    gItem->m_drawOutline = arg;
    update();
    emit drawOutlineChanged(arg);
}

void WiringElement::setOutlinePen(Pen *arg)
{
    if (gItem->m_outlinePen == arg)
        return;

    gItem->m_outlinePen = arg;
    emit outlinePenChanged(arg);
}

void WiringElement::setOutlineBrush(Brush *arg)
{
    if (gItem->m_outlineBrush == arg)
        return;

    gItem->m_outlineBrush = arg;
    emit outlineBrushChanged(arg);
}

void WiringElement::setOutlineMargin(qreal arg)
{
    if (gItem->m_outlineMargin == arg)
        return;

    gItem->m_outlineMargin = arg;
    emit outlineMarginChanged(arg);
}

void WiringElement::setFixedScale(bool arg)
{
    if (m_fixedScale == arg)
        return;

    m_fixedScale = arg;
    if (scene()) {
        if (m_fixedScale)
            connect(scene(), SIGNAL(viewportScaleChanged(qreal)), this, SLOT(rebuild()));
        else
            scene()->disconnect(0, this, 0);
    }
    rebuild();
    emit fixedScaleChanged(arg);
}

void WiringElement::setZoomFactor(qreal arg)
{
    if (m_zoomFactor == arg)
        return;

    m_zoomFactor = arg;
    rebuild();
    emit zoomFactorChanged(arg);
}

void WiringElement::setDebugNodes(bool arg)
{
    if (m_debugNodes == arg)
        return;

    m_debugNodes = arg;
    update();
    emit debugNodesChanged(arg);
}

QPainterPath WiringElement::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void WiringElement::rebuild()
{
    if (!scene())
        return;
    if (m_fixedScale) {
        m_zoomFactor = 1 / scene()->viewportScale();
        emit zoomFactorChanged(m_zoomFactor);
    }
    build();
}

qreal WiringElement::outlineMargin() const
{
    return gItem->m_outlineMargin;
}

bool WiringElement::fixedScale() const
{
    return m_fixedScale;
}

qreal WiringElement::zoomFactor() const
{
    return m_zoomFactor;
}

bool WiringElement::debugNodes() const
{
    return m_debugNodes;
}



void WiringElement::paint(QPainter *painter)
{
    Q_UNUSED(painter)
}

QRectF WiringElement::getBoundingRect() const
{
    return QRectF();
}

void WiringElement::initScene()
{
    if (scene() && m_fixedScale)
        connect(scene(), SIGNAL(viewportScaleChanged(qreal)), this, SLOT(rebuild()));
}

void WiringElement::drawPolygonNodes(QPainter *painter, const QPolygonF &polygon, const QPen &pen) const
{
    qreal s = 1 / scene()->viewportScale();
    QPen dotPen;
    dotPen.setWidthF(pen.widthF() * 3.5);
    dotPen.setCosmetic(true);
    dotPen.setCapStyle(Qt::RoundCap);
    dotPen.setColor(pen.color());
    painter->setPen(dotPen);
    painter->drawPoints(polygon);
    //    painter->drawLines(lines);
    QPen centerPen;
    centerPen.setCosmetic(true);
    centerPen.setColor(QColor(0,0,0));
    centerPen.setCapStyle(Qt::RoundCap);
    centerPen.setWidthF(pen.width() * 2);
//    outlinePen.setWidthF(pen()->width() * 1.5);
    painter->setPen(centerPen);
    painter->drawPoints(polygon);
    QFont f;
    f.setPointSizeF(10);
    painter->setFont(f);
    int i = 0;

    painter->setPen(Qt::white);
    painter->save();
    painter->scale(s, s);
    painter->translate(-4, 0);
    for (QPointF pt:polygon) {
        painter->drawText(pt / s, QString::number(i++));
    }
    painter->restore();
}

QPainterPath WiringElement::buildOutline(const QPainterPath &path, const QPen &pen, const QBrush &brush) const
{
    if (!scene())
        return QPainterPath();
    QPainterPathStroker str(pen);
    qreal z = 1 / scene()->viewportScale();
    qreal w = pen.width();
    if (pen.isCosmetic())
        w *= z;
    if (w < outlineMargin() * z)
        w = outlineMargin() * z;
    str.setWidth(w);
    str.setDashPattern(Qt::SolidLine);
    str.setCapStyle(Qt::FlatCap);
    return str.createStroke(path);
}

WiringElement::WiringElementItem::WiringElementItem(SceneItem *item, QGraphicsItem *parent) :
    QuickGraphicsItem(item, parent),
    m_drawOutline(false),
    m_outlinePen(nullptr),
    m_outlineBrush(nullptr),
    m_outlineMargin(8)
{

}

void WiringElement::WiringElementItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (isDestroyed())
        return;
    if (m_drawOutline && m_outlineBrush)
        painter->fillPath(shape(), m_outlineBrush->brush());
    outer<WiringElement>()->paint(painter);
    if (m_drawOutline && m_outlinePen)
        painter->strokePath(shape(), m_outlinePen->pen());
}

QRectF WiringElement::WiringElementItem::calculateBoundingRect() const
{
    if (isDestroyed())
        return QRectF();
    return  outer<WiringElement>()->getBoundingRect();
}

QPainterPath WiringElement::WiringElementItem::shape() const
{
    return outer<WiringElement>()->shape();
}


Pen *WiringElement::outlinePen() const
{
    return gItem->m_outlinePen;
}

Brush *WiringElement::outlineBrush() const
{
    return gItem->m_outlineBrush;
}
