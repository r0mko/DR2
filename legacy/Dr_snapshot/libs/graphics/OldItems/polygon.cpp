#include "polygon.h"
#include "graphicspath.h"
#include <QDebug>

QPolygonF GPolygon::polygon() const
{
    return m_polygon;
}

void GPolygon::setPolygon(const QPolygonF &polygon)
{
    if (!batch)
        startBatch();
    m_polygon = polygon;
    if (!batch)
        finishBatch();
    emit polygonChanged();
}

GraphicsPath *GPolygon::toPath() const
{
    GraphicsPath *path = new GraphicsPath;
    QPainterPath p;
    p.addPolygon(m_polygon);
    path->setPath(p);
    return path;
}

qreal GPolygon::length() const
{
    qreal ret = 0;
    if(m_polygon.size() < 2)
        return ret;
    QPointF prev = m_polygon.first();
    for (int i = 1; i < m_polygon.size(); ++i) {
        QPointF pt = m_polygon[i];
        ret += QLineF(pt, prev).length();
        prev = pt;
    }
    return ret;

}

bool GPolygon::containsPoint(const QPointF &point, Qt::FillRule rule)
{
    return m_polygon.containsPoint(point, rule);
}

QPointF GPolygon::node(int index)
{
    return m_polygon.at(index);
}

GPolygon::GPolygon(QObject *parent) :
    QObject(parent),
    batch(false)
{
}

bool GPolygon::closed() const
{
    return m_polygon.isClosed();
}

int GPolygon::nodeCount() const
{
    return m_polygon.size();
}

QRectF GPolygon::boundingRect() const
{
    return m_polygon.boundingRect();
}

void GPolygon::begin()
{
    startBatch();
    batch = true;
}

void GPolygon::end()
{
    finishBatch();
    batch = false;
}

void GPolygon::clear()
{
    m_polygon.clear();
    emit nodeCountChanged();
    finishBatch();
}

void GPolygon::append(const QPointF &pt)
{
    if (!batch)
        startBatch();
    m_polygon.append(pt);
    if (!batch)
        finishBatch();
}

void GPolygon::append(qreal x, qreal y)
{
    append(QPointF(x, y));
}

void GPolygon::insert(int index, const QPointF &pt)
{
    if (!batch)
        startBatch();
    m_polygon.insert(index, pt);
    if (!batch)
        finishBatch();
}

void GPolygon::removeAt(int index)
{
    if (!batch)
        startBatch();
    m_polygon.removeAt(index);
    if (!batch)
        finishBatch();
}

void GPolygon::setPointPosition(int index, const QPointF &pt)
{
    if (!batch)
        startBatch();
    m_polygon[index] = pt;
    if (!batch)
        finishBatch();
}

void GPolygon::setPointPosition(int index, qreal x, qreal y)
{
    setPointPosition(index, QPointF(x, y));
}

void GPolygon::setXAt(int index, qreal x)
{
    if (!batch)
        startBatch();
    if (index < m_polygon.size())
        m_polygon[index].setX(x);
    if (!batch)
        finishBatch();
}

void GPolygon::setYAt(int index, qreal y)
{
    if (!batch)
        startBatch();
    if (index < m_polygon.size())
        m_polygon[index].setY(y);
    if (!batch)
        finishBatch();
}

void GPolygon::unite(GPolygon *other)
{
    if (!batch)
        startBatch();
    m_polygon = m_polygon.united(other->m_polygon);
    if (!batch)
        finishBatch();
}

void GPolygon::substract(GPolygon *other)
{
    if (!batch)
        startBatch();
    m_polygon = m_polygon.subtracted(other->m_polygon);
    if (!batch)
        finishBatch();
}

void GPolygon::intersect(GPolygon *other)
{
    if (!batch)
        startBatch();
    m_polygon = m_polygon.intersected(other->m_polygon);
    if (!batch)
        finishBatch();
}

void GPolygon::translate(const QPointF &offset)
{
    if (!batch)
        startBatch();
    m_polygon.translate(offset);
    if (!batch)
        finishBatch();
}

void GPolygon::translate(qreal dx, qreal dy)
{
    if (!batch)
        startBatch();
    m_polygon.translate(dx, dy);
    if (!batch)
        finishBatch();
}
void GPolygon::fromQPolygonF(QVariant pol)
{
    QPolygonF p = pol.value<QPolygonF>();
    if (!p.isEmpty())
        setPolygon(p);
}

void GPolygon::startBatch()
{
    oldBoundingRect = m_polygon.boundingRect();
    wasClosed = m_polygon.isClosed();
    oldSize = m_polygon.size();
}

void GPolygon::finishBatch()
{
    if (oldSize != m_polygon.size()) {
        emit nodeCountChanged();
        oldSize = m_polygon.size();
    }
    QRectF newBr = m_polygon.boundingRect();
    if (newBr != oldBoundingRect) {
        emit boundingRectChanged();
        oldBoundingRect = newBr;
    }
    if (wasClosed != m_polygon.isClosed()) {
        emit closedChanged();
        wasClosed = m_polygon.isClosed();
    }
}
