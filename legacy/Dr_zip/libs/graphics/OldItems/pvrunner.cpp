#include "pvrunner.h"
#include <QtMath>
#include <QVector2D>
#include <QLineF>
#include "graph.h"



PVRunner::PVRunner() :
    m_position(0),
    m_backwards(false)
{

}

PVRunner::~PVRunner()
{

}

void PVRunner::setPosition(qreal arg)
{
    if (m_edge.isNull())
        return;
    if (m_position == arg)
        return;
    m_position = 0;
    m_node = m_edge.first();
    nodePoint = m_edge.node1().point();
    while (!m_node.isNull()) {
        if (m_position + m_node.length() >= arg)
            break;
        m_position += m_node.length();
        nodePoint += m_node.vector();
        m_node = m_node.next();
    }
    if (m_node.isNull()) {
        m_atEnd = true;
        m_atVertex = true;
        m_node = m_edge.last();
        m_position = m_edge.length();
        nodePos = m_node.length();
        nodePoint = m_edge.node2().point();
    } else {
        nodePos = arg - m_position;
        if (nodePos == 0 || nodePos == m_node.length())
            m_atVertex = true;
        else
            m_atVertex = false;
        m_atEnd = false;
        m_atStart = false;
        m_position = arg;
    }
    //    notifyChanges();
}

qreal PVRunner::moveBy(qreal len)
{
    if (len > 0) {
        return moveForward(len);
    } else {
        return -moveBack(-len);
    }
}

qreal PVRunner::moveForward(qreal len)
{
    qreal cpos = m_position;
    while (len > lengthAhead()) {
        len -= hopForward();
        if (m_atEnd)
            break;
    }
    if (!m_atEnd)
        nearMove(len);
    return qAbs(m_position - cpos);
}

qreal PVRunner::moveBack(qreal len)
{
    qreal cpos = m_position;
    while (len > lengthBehind()) {
        len -= hopBack();
        if (m_atStart)
            break;
    }
    if (!m_atStart)
        nearMove(len);
    return qAbs(m_position - cpos);
}

QPointF PVRunner::backVector() const
{

}

QPointF PVRunner::forwardVector() const
{
}

QLineF PVRunner::segment() const
{
    return QLineF(nodePoint, nodePoint + m_node.vector());
}

qreal PVRunner::hopForward()
{
    qreal cpos = m_position;
    if (m_backwards) {
        if (m_atVertex && !m_node.hasPrev())
            m_atEnd = true;
        else
            _toPrev();
    } else {
        if (m_node.hasNext())
            _toNext();
        else
            m_atStart = true;
    }
    return qAbs(m_position - cpos);
}

qreal PVRunner::hopBack()
{
    qreal cpos = m_position;
    if (!m_backwards) {
        if (m_atVertex && !m_node.hasPrev())
            m_atStart= true;
        else
            _toPrev();
    } else {
        if (m_node.hasNext())
            _toNext();
        else
            m_atEnd = true;
    }
    return qAbs(m_position - cpos);
}



QPointF PVRunner::toClosestPosition(const QPointF &pos)
{
    _toBegin();
    nodePoint = m_edge.node1().point();
    m_atVertex = true;
    QPointF ret;
    qreal len = 0;
    qreal mindst = 1e+127;
    QVector2D pv(pos);
    qreal length;
    while (!m_node.isNull()) {
        qreal dst = pv.distanceToLine(QVector2D(nodePoint), QVector2D(m_node.unitVector()));
        if (dst < mindst) {
            QPointF endP = (m_node.normal() * (dst + 1));
            QLineF l_perpendicular(pos - endP, pos + endP);
            QPointF iPoint;
            QLineF::IntersectType sect = segment().intersect(l_perpendicular, &iPoint);
            if (sect == QLineF::BoundedIntersection) {
                mindst = dst;
                ret = iPoint;
                qreal seg_pos = QLineF(nodePoint, iPoint).length();
                length = m_position + seg_pos;
            }
        }
        QLineF st(pos, nodePoint);
        qreal stlen = st.length();
        if (stlen < mindst) {
            mindst = stlen;
            length = len;
            ret = nodePoint;
        }
        _toNext();
    }
    QLineF e(pos, m_edge.node2().point());
    qreal elen = e.length();
    if (elen < mindst) {
        length = m_edge.length();
        ret = m_edge.node2().point();
    }
    setPosition(length);
    return ret;
}

void PVRunner::moveToHead()
{
    m_atVertex = true;
    m_atStart = true;
    if (m_backwards)
        _toEnd();
    else
        _toBegin();
}

void PVRunner::moveToTail()
{
    m_atVertex = true;
    m_atEnd = true;
    if (!m_backwards)
        _toEnd();
    else
        _toBegin();
}



PVRunner &PVRunner::operator +=(qreal distance)
{
    if (m_backwards)
        m_position -= distance;
    else
        m_position += distance;
    return *this;
}

PVRunner &PVRunner::operator -=(qreal distance)
{
    if (m_backwards)
        m_position += distance;
    else
        m_position -= distance;
    return *this;
}

PVNode PVRunner::aheadNode()
{
    if(m_backwards)
        return m_node.prev();
    return m_node.next();
}

PVNode PVRunner::behindNode()
{
    if (m_backwards)
        return m_node.next();
    return m_node.prev();
}

void PVRunner::nearMove(qreal dst)
{
    if (dst == 0)
        return;
    if (m_backwards) {
        m_position -= dst;
        nodePos -= dst;
    } else {
        m_position += dst;
        nodePos += dst;
    }
    if (nodePos == 0)
        m_atVertex = true;
}

void PVRunner::_toNext()
{
    nodePoint += m_node.vector();
    m_position += m_node.length() - nodePos;
    nodePos = 0;
    m_atVertex = true;
    m_node = m_node.next();
}

void PVRunner::_toPrev()
{
    if (m_atVertex) {
        if (m_node.hasPrev()) {
            m_position -= m_node.prev().length();
            nodePoint -= m_node.prev().vector();
        }
        m_node = m_node.prev();
    } else {
        m_position -= nodePos;
        nodePos = 0;
        m_atVertex = true;
    }
}

void PVRunner::_toBegin()
{
    m_node = m_edge.first();
    m_position = 0;
    nodePos = 0;
    nodePoint = m_edge.node1().point();
}

void PVRunner::_toEnd()
{
    m_node = m_edge.last();
    m_position = m_edge.length();
    nodePos = m_node.length();
    nodePoint = m_edge.node2().point() - m_node.vector();
}

//void PVRunner::notifyChanges()
//{
//    static bool atNode;
//    static bool atStart;
//    static bool atEnd;
//    static qreal pos;
//    if (atNode != m_atVertex) {
//        atNode = m_atVertex;
//        emit atNodeChanged();
//    }
//    if (atStart != m_atStart) {
//        atStart = m_atStart;
//        emit atStartChanged();
//    }
//    if (atEnd != m_atEnd) {
//        atEnd = m_atEnd;
//        emit atEndChanged();
//    }
//    if (pos != m_position) {
//        pos = m_position;
//        emit positionChanged();
//        emit lengthAheadChanged();
//        emit lengthBehindChanged();
//        emit pointChanged();
//    }
//}


void PVRunner::setEdge(PVChain arg, bool backwards)
{
    if (m_edge == arg)
        return;

    m_edge = arg;
    m_backwards = backwards;
    //        emit edgeChanged();
}

void PVRunner::setBackwards(bool back)
{
    m_backwards = back;
}
