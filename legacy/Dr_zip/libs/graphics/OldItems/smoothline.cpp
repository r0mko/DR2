#include "smoothline.h"
#include <QPainter>
#include "scene.h"
#include "groove.h"
void SmoothLine::SmoothLineItem::build(qreal startPos, qreal endPos)
{
    if (!pen())
        return;
    if (m_guide->vectorPath().isNull())
        return;
    VectorPath result = m_guide->vectorPath();
    if (m_radius > 0)
        result = result.buildRounding(m_radius * qSqrt(m_zoomFactor), m_radius * qSqrt(m_zoomFactor) / m_precision, startPos, endPos);

    if (m_offset > 0)
        result = result.buildOffset(pen()->width() * m_offset * m_zoomFactor);

    m_result->setVectorPath(result);
    polygon = result.toPolygon();
    QRectF b = polygon.boundingRect();
    QPainterPath p;
    p.addPolygon(polygon);
    setPath(p);
    setBoundingRectDirty();
    updateBoundingRect(b);
    emit outer<SmoothLine>()->resultReady();
    update(b);
}





SmoothLine::SmoothLineItem::SmoothLineItem(SceneItem *item, QGraphicsItem *parent) :
    QuickGraphicsShapeItem(item, parent),
    m_offset(0),
    m_zoomFactor(1),
    m_radius(10),
    m_precision(18),
    m_startPos(0),
    m_endPos(HUGE)
{

}

qreal SmoothLine::SmoothLineItem::offset() const
{
    return m_offset;
}

void SmoothLine::SmoothLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    if (!pen())
        return;
    QPen outlinePen;
    outlinePen.setCosmetic(true);
    outlinePen.setColor(QColor(0,0,0));
    outlinePen.setCapStyle(Qt::RoundCap);
    outlinePen.setWidthF(pen()->width() * 2);
    //    painter->setPen(outlinePen);
    //    painter->drawPolyline(polygon);
    painter->setPen(pen()->pen());
    painter->drawPolyline(polygon);

    if (!m_displayDebugNodes)
        return;

    qreal s = 1 / super()->scene()->viewportScale();
    QPen dotPen;
    dotPen.setWidthF(pen()->width() * 3.5);
    dotPen.setCosmetic(true);
    dotPen.setCapStyle(Qt::RoundCap);
    dotPen.setColor(pen()->color());
    painter->setPen(dotPen);
    painter->drawPoints(polygon);
    //    painter->drawLines(lines);

    outlinePen.setWidthF(pen()->width() * 1.5);
    painter->setPen(outlinePen);
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

void SmoothLine::SmoothLineItem::setGuide(Groove *guide)
{
    m_guide = guide;
}

Groove *SmoothLine::SmoothLineItem::guide() const
{
    return m_guide;
}

void SmoothLine::SmoothLineItem::setOffset(const qreal &offset)
{
    m_offset = offset;
    build(m_startPos, m_endPos);

}

void SmoothLine::SmoothLineItem::setZoomFactor(qreal arg)
{
    m_zoomFactor = arg;
    build(m_startPos, m_endPos);
}

qreal SmoothLine::SmoothLineItem::zoomFactor() const
{
    return m_zoomFactor;
}

qreal SmoothLine::SmoothLineItem::precision() const
{
    return m_precision;
}

void SmoothLine::SmoothLineItem::setPrecision(qreal arg)
{
    m_precision = arg;
    build(m_startPos, m_endPos);
}

qreal SmoothLine::SmoothLineItem::startPos() const
{
    return m_startPos;
}

Groove *SmoothLine::SmoothLineItem::result() const
{
    return m_result;
}

void SmoothLine::SmoothLineItem::setResult(Groove *result)
{
    m_result = result;
}

SmoothLine::SmoothLine(SceneItem *parent) :
    SceneItem(new SmoothLineItem(this), parent)
{
    gItem->setGuide(new Groove(this));
    gItem->setResult(new Groove(this));
}

SmoothLine::~SmoothLine()
{

}

void SmoothLine::setPen(Pen *arg)
{
    if (item()->pen() == arg)
        return;
    if (gItem->pen())
        gItem->pen()->disconnect(0, this, 0);

    item()->setPen(arg);
    connect(gItem->pen(), SIGNAL(penChanged()), this, SLOT(update()));
    emit penChanged(arg);
}

void SmoothLine::setOffset(qreal arg)
{
    if (item()->offset() == arg)
        return;
    item()->setOffset(arg);
    emit offsetChanged(arg);
}

void SmoothLine::setZoomFactor(qreal arg)
{
    if (gItem->zoomFactor() == arg)
        return;

    gItem->setZoomFactor(arg);
    emit zoomFactorChanged(arg);
}

void SmoothLine::setRadius(qreal arg)
{
    if (gItem->radius() == arg)
        return;

    gItem->setRadius(arg);
    emit radiusChanged(arg);
}

void SmoothLine::setPrecision(qreal arg)
{
    if (gItem->precision() == arg)
        return;
    gItem->setPrecision(arg);
    emit precisionChanged(arg);
}

void SmoothLine::setStartPos(qreal arg)
{
    if (startPos() == arg)
        return;

    gItem->setStartPos(arg);
    emit startPosChanged(arg);
}

void SmoothLine::SmoothLineItem::setEndPos(qreal arg)
{
    m_endPos = arg;
    build(m_startPos, m_endPos);
}

void SmoothLine::setEndPos(qreal arg)
{
    if (endPos() == arg)
        return;

    gItem->setEndPos(arg);
    emit endPosChanged(arg);
}

void SmoothLine::setMinimumWidth(qreal arg)
{
    if (gItem->minimumWidth() == arg)
        return;

    gItem->setMinimumWidth(arg);
    emit minimumWidthChanged(arg);
}

const VectorPath &SmoothLine::vectorPath() const
{
    return item()->guide()->vectorPath();
}

Pen *SmoothLine::pen() const
{
    return item()->pen();
}

qreal SmoothLine::offset() const
{
    return item()->offset();
}

qreal SmoothLine::zoomFactor() const
{
    return item()->zoomFactor();
}

qreal SmoothLine::radius() const
{
    return gItem->radius();
}

qreal SmoothLine::precision() const
{
    return gItem->precision();
}

bool SmoothLine::displayDebugNodes() const
{
    return gItem->displayDebugNodes();
}

qreal SmoothLine::length() const
{
    return gItem->guide()->vectorPath().length();
}

Groove *SmoothLine::guide() const
{
    return gItem->guide();
}

Groove *SmoothLine::result() const
{
    return gItem->result();
}

qreal SmoothLine::minimumWidth() const
{
    return gItem->minimumWidth();
}

void SmoothLine::appendRib(Groove *guide, qreal inPosition, qreal outPosition, QVariant data)
{
    item()->guide()->vectorPath().addVectorPath(guide->vectorPath().getPart(inPosition, outPosition), data);
}

qreal SmoothLine::startPos() const
{
    return gItem->startPos();
}

qreal SmoothLine::SmoothLineItem::endPos() const
{
    return m_endPos;
}

qreal SmoothLine::endPos() const
{
    return gItem->endPos();
}

void SmoothLine::SmoothLineItem::setStartPos(qreal arg)
{
    m_startPos = arg;
    build(m_startPos, m_endPos);
}



void SmoothLine::clear()
{
    item()->guide()->vectorPath().clear();
}

void SmoothLine::init()
{
    setStartPos(0);
    setEndPos(vectorPath().length());
    emit lengthChanged(vectorPath().length());
//    vectorPath().dumpNodes();
    emit ready();
    update();
}

void SmoothLine::setDisplayDebugNodes(bool arg)
{
    if (gItem->displayDebugNodes() == arg)
        return;

    gItem->setDisplayDebugNodes(arg);
    emit displayDebugNodesChanged(arg);
}

qreal SmoothLine::SmoothLineItem::radius() const
{
    return m_radius;
}


void SmoothLine::SmoothLineItem::setRadius(qreal arg)
{
    m_radius = arg;
    build(m_startPos, m_endPos);
}


bool SmoothLine::SmoothLineItem::displayDebugNodes() const
{
    return m_displayDebugNodes;
}

void SmoothLine::SmoothLineItem::setDisplayDebugNodes(bool arg)
{
    m_displayDebugNodes = arg;
    update();
}
