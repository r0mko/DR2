#include "offsetpolylinesceneitem.h"

OffsetPolylineSceneItem::OffsetPolylineSceneItem(ShapeSceneItem *parent) :
    ShapeSceneItem(parent),
    m_offset(0),
    m_radius(10),
    m_simple(true),
    m_precision(12)
{

}

qreal OffsetPolylineSceneItem::offset() const
{
    return m_offset;
}

qreal OffsetPolylineSceneItem::radius() const
{
    return m_radius;
}

QPolygonF OffsetPolylineSceneItem::polygon() const
{
    return PolyLineRange::polygon();
}

bool OffsetPolylineSceneItem::simple() const
{
    return m_simple;
}

int OffsetPolylineSceneItem::precision() const
{
    return m_precision;
}

void OffsetPolylineSceneItem::setOffset(qreal arg)
{
    if (m_offset == arg)
        return;
    m_offset = arg;
    emit offsetChanged(arg);
    updatePath();
}

void OffsetPolylineSceneItem::setRadius(qreal arg)
{
    if (m_radius == arg)
        return;

    m_radius = arg;
    emit radiusChanged(arg);
    updateWire();
    updatePath();
}

void OffsetPolylineSceneItem::setPolygon(QPolygonF arg)
{
    qDebug() << "Setting polygon" << arg;
    if (polygon() == arg)
        return;
    PolyLineRange::setPolygon(arg);
    init();
    emit polygonChanged(arg);
    updateWire();
    updatePath();
}

void OffsetPolylineSceneItem::setSimple(bool arg)
{
    if (m_simple == arg)
        return;
    m_simple = arg;
    emit simpleChanged(arg);
    updateWire();
    updatePath();
}

void OffsetPolylineSceneItem::setPrecision(int arg)
{
    if (m_precision == arg)
        return;
    m_precision = arg;
    emit precisionChanged(arg);
    updateWire();
    updatePath();
}

void OffsetPolylineSceneItem::initPolygon(QVariant pol)
{
    QPolygonF p = pol.value<QPolygonF>();
    if (!p.isEmpty())
        setPolygon(p);
}

void OffsetPolylineSceneItem::initPointList(QVariant pol)
{
    QVariantList pts = pol.toList();
    QPolygonF ret;
    for (QVariant pt:pts) {
        ret.append(pt.toPointF());
    }
    setPolygon(ret);
}

void OffsetPolylineSceneItem::updateWire()
{
    if (m_simple) {
        calculateSimple();
    } else {
//        calculateCommonRounded(m_radius, m_precision);
        calculateRounded(m_radius, m_precision);
    }
}

void OffsetPolylineSceneItem::updatePath()
{
//    calculateSimple();

//    QPolygonF line = calculateOffset(wire(), m_offset);
    QPolygonF line = wire().toPolygonF();
    QPainterPath p;
    p.addPolygon(line);
    setPath(p);
    update(boundingRect());
}
