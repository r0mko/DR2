#include "graphicspath.h"

GraphicsPath::GraphicsPath(QObject *parent) :
    QObject(parent),
    boundingRectDirty(true)
{
}

void GraphicsPath::setPath(QPainterPath path)
{
    m_path = path;
//    qDebug("Path updated");
    boundingRectDirty = true;
    emit pathChanged();
}

QRectF GraphicsPath::boundingRect() const
{
    if (boundingRectDirty) {
        m_boundingRect = m_path.boundingRect();
        boundingRectDirty = false;
    }
    return m_boundingRect;
}

QPointF GraphicsPath::currentPosition() const
{
    return m_path.currentPosition();
}

int GraphicsPath::elementCount() const
{
    return m_path.elementCount();
}

QPointF GraphicsPath::begin() const
{
    return m_path.elementAt(0);
}

QPointF GraphicsPath::end() const
{
    int i = m_path.elementCount() - 1;
    return m_path.elementAt(i);
}

bool GraphicsPath::contains(const QPointF &pt) const
{
    return m_path.contains(pt);
}

bool GraphicsPath::contains(const QRectF &rect) const
{
    return m_path.contains(rect);
}

bool GraphicsPath::contains(const GraphicsPath *other) const
{
    return m_path.contains( other->path() );
}

bool GraphicsPath::intersects(const QRectF &rect) const
{
    return m_path.intersects(rect);
}

bool GraphicsPath::intersects(const GraphicsPath *other) const
{
    return m_path.intersects(other->path());
}

QPointF GraphicsPath::getElementPosition(int elementIndex) const
{
    auto ele = m_path.elementAt(elementIndex);
    return QPointF(ele.x, ele.y);
}

QPointF GraphicsPath::pointAtPercent(qreal t) const
{
    return m_path.pointAtPercent(t);
}

qreal GraphicsPath::slopeAtPercent(qreal t) const
{
    return m_path.slopeAtPercent(t);
}

qreal GraphicsPath::length() const
{
    return m_path.length();
}

qreal GraphicsPath::percentAtLength(qreal t) const
{
    return m_path.percentAtLength(t);
}

void GraphicsPath::setElementPosition(int elementIndex, qreal x, qreal y)
{
    m_path.setElementPositionAt(elementIndex, x, y);
    setBoundingRectDirty();
    emit pathChanged();
}

void GraphicsPath::setElementPosition(int elementIndex, QPointF p)
{
    setElementPosition(elementIndex, p.x(), p.y());
}

void GraphicsPath::substract(GraphicsPath *other)
{
    m_path = m_path.subtracted(other->path());
    setBoundingRectDirty();
    emit pathChanged();
}

void GraphicsPath::intersect(GraphicsPath *other)
{
    m_path = m_path.intersected(other->path());
    setBoundingRectDirty();
    emit pathChanged();
}

void GraphicsPath::unite(GraphicsPath *other)
{
    m_path = m_path.united(other->path());
    setBoundingRectDirty();
    emit pathChanged();
}

void GraphicsPath::add(GraphicsPath *other)
{
    m_path.addPath(other->path());
    setBoundingRectDirty();
    emit pathChanged();
}

void GraphicsPath::simplify()
{
    m_path = m_path.simplified();
    emit pathChanged();
}

void GraphicsPath::setBoundingRectDirty()
{
    boundingRectDirty = true;
}


QPainterPath &GraphicsPath::rpath()
{
    return m_path;
}

QPainterPath *GraphicsPath::pathPtr()
{
    return &m_path;
}

QPainterPath GraphicsPath::path() const
{
    return m_path;
}
