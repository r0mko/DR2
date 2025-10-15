#include "bspobject.h"
#include <QDebug>


BspObject::BspObject(QObject *parent):
    QObject(parent)
{

}

void BspObject::clear()
{
    m_tree.clear();
    emit cleared();
}

QVariantList BspObject::dumpRects() const
{
    QVariantList ret;
    for( QRectF r: m_tree.dumpRects() )
        ret.append( r );
    return ret;
}

void BspObject::insertPoint(const QPointF &point, const int &data)
{
    m_tree.insertPoint(point, data);
}

void BspObject::insertPoints(const QPolygonF &polygon, const int &data)
{
    m_tree.insertPoints(polygon, data);
}

QRectF BspObject::rect() const
{
    return m_tree.rect();
}

int BspObject::maxDepth() const
{
    return m_tree.maxDepth();
}

void BspObject::setRect(QRectF rect)
{
    if (m_tree.rect() == rect)
        return;

    m_tree.setRect(rect);
    emit rectChanged(rect);
}

void BspObject::setMaxDepth(int maxDepth)
{
    if (m_tree.maxDepth() == maxDepth)
        return;

    m_tree.setMaxDepth(maxDepth);
    emit maxDepthChanged(maxDepth);
}

