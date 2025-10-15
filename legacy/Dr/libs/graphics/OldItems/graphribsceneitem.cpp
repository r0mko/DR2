#include "graphribsceneitem.h"
#include <QtMath>
#include <QPainter>
#include "scene.h"
#include "pen.h"
#include <QDebug>
#include "vectorpath.h"

GraphRibSceneItem::GraphRibSceneItem(SceneItem *parent) :
    ShapeSceneItem(parent),
    m_oneWay(false),
    m_showArrows(true),
    m_highlight(false),
    m_arrow(new RibArrow(this)),
    m_guide(new Groove(this)),
    m_strokePen(nullptr)
{
}

QPolygonF GraphRibSceneItem::polygon() const
{
    return m_guide->polygon();
}

void GraphRibSceneItem::setPolygon(QPolygonF arg)
{
    if (polygon() == arg)
        return;
    m_guide->setPolygon(arg);
    QPainterPath p;
    p.addPolygon(arg);
    setPath(p);
    emit polygonChanged(arg);
}

void GraphRibSceneItem::setOneWay(bool arg)
{
    if (m_oneWay == arg)
        return;

    m_oneWay = arg;
    emit oneWayChanged(arg);

}

void GraphRibSceneItem::setShowArrows(bool arg)
{
    if (m_showArrows == arg)
        return;

    m_showArrows = arg;
    emit showArrowsChanged(arg);
    update();
}

void GraphRibSceneItem::setStrokePen(Pen *arg)
{
    if (m_strokePen == arg)
        return;

    m_strokePen = arg;
    emit strokePenChanged(arg);
    update();
}

void GraphRibSceneItem::setNode1(int arg)
{
    if (m_node1 == arg)
        return;

    m_node1 = arg;
    emit node1Changed(arg);
}

void GraphRibSceneItem::setNode2(int arg)
{
    if (m_node2 == arg)
        return;

    m_node2 = arg;
    emit node2Changed(arg);
}

void GraphRibSceneItem::setHighlight(bool arg)
{
    if (m_highlight == arg)
        return;

    m_highlight = arg;
    emit highlightChanged(arg);
}

void GraphRibSceneItem::setModelIdx(int arg)
{
    if (m_modelIdx == arg)
        return;

    m_modelIdx = arg;
    emit modelIdxChanged(arg);
}

QVariantMap GraphRibSceneItem::nearestPoint(QPointF point) const
{
    qreal length;
    QPointF pt = m_guide->vectorPath().nearestPoint(point, &length);
    QVariantMap ret;
    ret["position"] = pt;
    ret["length"] = length;
    return ret;
}

Groove *GraphRibSceneItem::guide() const
{
    return const_cast<Groove*>(m_guide);
}

void GraphRibSceneItem::highlightRange(qreal startPos, qreal endPos)
{
//    m_inPosition = startPos;
//    m_outPosition = endPos;
    m_inPosition = qMin(startPos, endPos);
    m_outPosition = qMax(startPos, endPos);
    update();
}

void GraphRibSceneItem::clearRangeHighlight()
{
    m_inPosition = m_outPosition = 0;
    update();
}

qreal GraphRibSceneItem::length() const
{
    return m_guide->length();
}

void GraphRibSceneItem::afterDraw(QPainter *painter)
{
    if (m_inPosition == m_outPosition)
        return;
    if (!m_strokePen)
        return;
    painter->setPen(m_strokePen->pen());
    VectorPath p = m_guide->vectorPath().getPart(m_inPosition, m_outPosition);
//    p = p.buildRounding(40, 4, 0, m_outPosition - m_inPosition);
    QPolygonF pol = p.toPolygon();
    painter->drawPolyline(pol);

//    if (!m_showArrows || !m_arrow->pen())
//        return;

//    qreal pos = 0;
//    QVector<QLineF> lines = { QLineF(0, -m_arrow->width(), m_arrow->length(), 0), QLineF (0, m_arrow->width(), m_arrow->length(), 0), QLineF (-m_arrow->length(), 0, m_arrow->length(), 0) };
//    painter->setPen(m_arrow->pen()->pen());
//    while (pos < m_guide->length()) {
//        int i;
//        qreal s = 1/scene()->viewportScale();
//        pos += m_arrow->step() * s;
//        if (pos >= m_guide.length())
//            break;
//        QPointF pt = m_guide.pointAtLength(pos, &i);
//        //        if (i == m_polygon.size() - 2)
//        //            return;
//        QLineF seg;
//        seg.setP2(QPointF(m_guide->legs()[i].normal.y(), -m_guide->legs()[i].normal.x()));
//        qreal angle = seg.angle();
//        painter->save();
//        painter->translate(pt);
//        painter->scale(s, s);
//        painter->rotate(-angle);
//        if(!oneWay())
//            painter->translate(0, m_arrow->offset());
//        painter->drawLines(lines);
//        if (!oneWay()) {
//            painter->translate(0, -2 * m_arrow->offset());
//            painter->rotate(180);
//            painter->translate(m_arrow->length(), 0);
//            painter->drawLines(lines);
//        }
//        painter->restore();
//    }
}




bool GraphRibSceneItem::oneWay() const
{
    return m_oneWay;
}

bool GraphRibSceneItem::showArrows() const
{
    return m_showArrows;
}

Pen *GraphRibSceneItem::strokePen() const
{
    return m_strokePen;
}

RibArrow *GraphRibSceneItem::arrow() const
{
    return m_arrow;
}

int GraphRibSceneItem::node1() const
{
    return m_node1;
}

int GraphRibSceneItem::node2() const
{
    return m_node2;
}

bool GraphRibSceneItem::highlight() const
{
    return m_highlight;
}

int GraphRibSceneItem::modelIdx() const
{
    return m_modelIdx;
}

RibArrow::RibArrow(QObject *parent) :
    QObject(parent),
    m_width(2),
    m_offset(4),
    m_length(10),
    m_step(50),
    m_pen(nullptr)
{

}

void RibArrow::setWidth(qreal arg)
{
    if (m_width == arg)
        return;

    m_width = arg;
    emit widthChanged(arg);
}

void RibArrow::setOffset(qreal arg)
{
    if (m_offset == arg)
        return;

    m_offset = arg;
    emit offsetChanged(arg);
}

void RibArrow::setLength(qreal arg)
{
    if (m_length == arg)
        return;

    m_length = arg;
    emit lengthChanged(arg);
}

void RibArrow::setStep(qreal arg)
{
    if (m_step == arg)
        return;

    m_step = arg;
    emit stepChanged(arg);
}

void RibArrow::setPen(Pen *arg)
{
    if (m_pen == arg)
        return;

    m_pen = arg;
    emit penChanged(arg);
}


