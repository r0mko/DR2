#include "pathrunner.h"
#include "pnode.h"
#include "vchain.h"
#include <QDebug>

qreal GuideRunner::lengthAhead() const
{
    if (atStart() && hasNext())
        return next().length();
    else if (atVector())
        return i->t.length() - nodePos;
    return 0;
}



qreal GuideRunner::moveForward(qreal len)
{
    if (len <= 0)
        return 0;
    qreal cpos = m_position;
    while (len >= lengthAhead()) {
        len -= hopForward();
        if (atEnd()) {
            qDebug() << "Reached end by move forward" << len << m_position;
            break;
        }
    }
    if (!atEnd())
        nearMove(len);
    return qAbs(m_position - cpos);
}

void GuideRunner::nearMove(qreal dst)
{
    if (dst == 0)
        return;
    m_position += dst;
    nodePos += dst;
    m_atVertex = nodePos == 0;
}

GuideRunner::GuideRunner() { i = nullptr; }

GuideRunner::GuideRunner(GuidePath *chain, qreal pos) :
    QLinkedList<VectorNode>::iterator(chain->nodes.begin())
{
    if (i->t.type() == VectorNode::Start) {
        nodePoint = pnode().point();
    }
    moveForward(pos);
}







bool GuideRunner::atStart() const
{
    return (node().type() & VectorNode::Start) == VectorNode::Start;
}

bool GuideRunner::atEnd() const
{
    return (node().type() & VectorNode::Finish) == VectorNode::Finish;
}

bool GuideRunner::atNode() const
{
    return i->t.type() & VectorNode::Node;
}

bool GuideRunner::isNull() const
{
    return i == nullptr;
}

qreal GuideRunner::position() const
{
    return m_position;
}

void GuideRunner::setPoint(const QPointF &pt) { nodePoint = pt; }

QPointF GuideRunner::point() const
{
    if (atNode())
        return pnode().point();
    else if (atVector())
        return nodePoint + i->t.unitVector() * nodePos;
    return QPointF();
}

qreal GuideRunner::angle() const
{
    QPointF v = node().unitVector();
    return qAtan2(v.y(), v.x()) * 180 / M_PI;
}

QPointF GuideRunner::normal() const
{
    return i->t.normal();

}

qreal GuideRunner::dotProduct() const
{
    if (!m_atVertex)
        return 1;
    if (!hasPrev())
        return -INFINITY;
    return QPointF::dotProduct(prev().unitVector(), node().unitVector());

}
qreal GuideRunner::turnAngle() const
{
    if (!m_atVertex)
        return 0;
    if (!hasPrev())
        return 0;
    const VectorNode &p = prev();
    const VectorNode &n = i->t;
    QPointF v1 = p.unitVector();
    QPointF v2 = n.unitVector();
    qreal a1 = qAtan2(v1.y(), v1.x());
    qreal a2 = qAtan2(v2.y(), v2.x());
    return normalizeAngle(a1 - a2) * 180 / M_PI;
}

bool GuideRunner::hasNext() const
{
    return next().type() == VectorNode::Vector;
}

bool GuideRunner::hasPrev() const
{
    return prev().type() == VectorNode::Vector;
}

QLineF GuideRunner::segment() const
{
    return QLineF(nodePoint, nodePoint + i->t.vector());
}


qreal GuideRunner::hopForward()
{
    qreal cpos = m_position;
    if (atStart())
        i = i->n;
    if (atEnd()) {
        nodePoint = pnode().point();
        m_atVertex = true;
        return 0;
    }
    qreal dp = node().length() - nodePos;
    m_position += dp;
    if (hasNext()) {
        nodePos = 0;
        nodePoint += node().vector();
    } else {
        nodePos = node().length();
        nodePoint = pnode(next()).point();
    }
    i = i->n;
    m_atVertex = true;
    return qAbs(m_position - cpos);
}








qreal GuideRunner::hopBack()
{
    if (atEnd())
        i = i->p;
    if (atStart()) {
        m_position = 0;
        nodePos = 0;
        nodePoint = pnode().point();
        m_atVertex = true;
        return 0;
    }
    qreal cpos = m_position;
    if (m_atVertex) {
        i = i->p;
        if (atVector()) {
            m_position -= node().length();
            nodePoint -= node().vector();
        } else {
            m_position = 0;
            nodePoint = pnode().point();
        }
    } else {
        m_position -= nodePos;
        nodePos = 0;
        m_atVertex = true;
    }
    return qAbs(m_position - cpos);
}

QPointF GuideRunner::roundingDelta(qreal range, qreal *aheadCurvature, qreal *behindCurvature) const
{
    if (!atVector()) {
        qDebug() << "Round at non-vector element";
        return QPointF();
    }
    if (range == 0 || isnan(range))
        return QPointF();
    QPointF cPt = point();
    QPointF negVec;
    qreal underShoot = 0;
    qreal rem;
    GuidePath negPath = negVP(range / 2, &rem);
    negVec = negPath.weightedAverage(range, cPt);
    underShoot = rem / range / 2;
    QPointF posVec;
    qreal overShoot = 0;
    GuidePath posPath = posVP(range / 2, &rem);
    posVec = posPath.weightedAverage(range, cPt);
    overShoot = rem / range / 2;
    QPointF negPt = negVec * (1 - overShoot) + posVec * overShoot;
    QPointF posPt = posVec * (1 - underShoot) + negVec * underShoot;
    if (behindCurvature)
        *behindCurvature = QPointF::dotProduct(normal(), negPt);
    if (aheadCurvature)
        *aheadCurvature = QPointF::dotProduct(normal(), posPt);
    return negPt + posPt;
}

GuidePath GuideRunner::posVP(qreal length, qreal *remaining) const
{
    GuidePath ret;
    if (atEnd())
        return ret;
    qreal l = node().length() - nodePos;
    auto v = i;
    while (length >= l) {
        if (l > 0)
            ret.appendNode(VectorNode(v->t.unitVector(), l));
        length -= l;
        if (v->n->t.isVector())
            v = v->n;
        else
            break;
        l = v->t.length();
    }
    if (length > 0) {
        if (v->t.isVector()) {
            ret.appendNode(VectorNode(v->t.unitVector(), length));
            length = 0;
        }
    }
    if (remaining)
        *remaining = length;
    return ret;
}

GuidePath GuideRunner::negVP(qreal length, qreal *remaining) const
{
    GuidePath ret;
    if (atStart())
        return ret;
    qreal l = nodePos;
    auto v = i;
    while (length >= l) {
        if (l > 0)
            ret.appendNode(VectorNode(-v->t.unitVector(), l));
        length -= l;
        if (v->p->t.isVector())
            v = i->p;
        else break;
        l = v->t.length();
    }
    if (length > 0) {
        if (v->t.isVector()) {
            ret.appendNode(VectorNode(-v->t.unitVector(), length));
            length = 0;
        }
    }
    if (remaining)
        *remaining = length;
    return ret;
}


//Path PathRunner::partPath(qreal length, qreal *remaining) const
//{
//    Link v = vElement();
//    if (!element().isVector())
//        qDebug() << "posvp is non vector";
//    Path ret;
//    if (v.isNull())
//        return ret;
//    bool neg = length < 0;
//    if (neg) {
//        length *= -1;
//    }
//    while (length > 0) {
//        if (v.type() != Link::DVectorElement) {
//            v = neg ? prev(v) : next(v);
//            continue;
//        }
//        DVector pv = neg ? negVector(v) : posVector(v);
//        if (pv.length() > length)
//            pv.setLength(length);
//        length -= pv.length();
//        ret.appendDVector(pv);
//        v = pv;
//        if (v.isNull())
//            break;
//    }
//    if (remaining)
//        *remaining = length;
//    return ret;
//}


qreal normalizeAngle(qreal angle)
{
    qreal m = fmod(angle, (2 * M_PI));
    if (m < 0)
        m = M_PI * 2 + m;
    if (m > M_PI)
        m = -2 * M_PI + m;
    return m;
}
