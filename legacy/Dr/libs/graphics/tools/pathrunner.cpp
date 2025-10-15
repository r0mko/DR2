#include "pathrunner.h"
#include "guidepath.h"
#include <QDebug>


qreal normalizeAngle(qreal angle)
{
    qreal m = fmod(angle, (2 * M_PI));
    if (m < 0)
        m = M_PI * 2 + m;
    if (m > M_PI)
        m = -2 * M_PI + m;
    return m;
}

qreal PathRunner::vectorLengthAhead() const
{
    if (atHead() && hasNext())
        return vnode(next())->length();
    else if (atVector())
        return vnode()->length() - nodePos;
    return 0;
}

qreal PathRunner::vectorLengthBehind() const
{
    if (atHead())
        return 0;
    if (m_atVertex && hasPrev())
        return vnode(prev())->length();
    return nodePos;
}

qreal PathRunner::moveForward(qreal len)
{
    if (len <= 0)
        return 0;
    qreal cpos = m_position;
    while (len >= vectorLengthAhead()) {
        len -= hopForward();
        if (atTail()) {
//            qDebug() << "Reached end by move forward" << len << m_position;
            break;
        }
    }
    if (!atTail())
        nearMove(len);
    return qAbs(m_position - cpos);
}

qreal PathRunner::moveBack(qreal len)
{
    if (len <= 0)
        return 0;
    qreal cpos = m_position;

    while (len >= vectorLengthBehind()) {
        len -= hopBack();
        if (atHead())
            break;
    }
    if (!atHead()) {
        if (m_atVertex) {
            VectorNode *pv = vnode(prev());
            nodePos = pv->length();
            nodePoint -= pv->vector();
            m_atVertex = false;
        }
        nearMove(-len);
    }
    return qAbs(m_position - cpos);
}

QPolygonF PathRunner::negativePart(qreal length) const
{
    qreal rem;
    return windowBehind(length, &rem).toPolygon();
}

QPolygonF PathRunner::positivePart(qreal length) const
{
    qreal rem;
    return windowAhead(length, &rem).toPolygon();

}

void PathRunner::nearMove(qreal dst)
{
    if (dst == 0)
        return;
    m_position += dst;
    nodePos += dst;
    m_atVertex = nodePos == 0;
}

PathRunner::PathRunner(const GuidePath *path, qreal pos) :
    PathIterator(path->d->nodes.cbegin())
{
    if (i->t.type() == VectorNode::HeadType) {
        nodePoint = pnode()->point();
        i = i->n;
    }
    moveForward(pos);
}

qreal PathRunner::position() const
{
    return m_position;
}

void PathRunner::setPoint(const QPointF &pt) { nodePoint = pt; }

QPointF PathRunner::point() const
{
    if (atPoint())
        return pnode()->point();
    else if (atVector())
        return nodePoint + vnode()->unitVector() * nodePos;
    return QPointF();
}

qreal PathRunner::angle() const
{
    if (!atVector())
        return 0;
    QPointF v = vnode()->unitVector();
    return qAtan2(v.y(), v.x()) * 180 / M_PI;
}

QPointF PathRunner::normal() const
{
    return vnode()->normal();

}

qreal PathRunner::dotProduct() const
{
    if (!m_atVertex)
        return 1;
    if (!hasPrev())
        return -INFINITY;
    return QPointF::dotProduct(vnode(prev())->unitVector(), vnode()->unitVector());

}
qreal PathRunner::turnAngle(const VectorNode *pv) const
{
    if (!m_atVertex)
        return 0;
    if (!hasPrev())
        return 0;
    if (!pv)
        pv = vnode(prev());
    const VectorNode *nv = vnode();
    QPointF v1 = pv->unitVector();
    QPointF v2 = nv->unitVector();
    qreal a1 = qAtan2(v1.y(), v1.x());
    qreal a2 = qAtan2(v2.y(), v2.x());
    return normalizeAngle(a1 - a2) * 180 / M_PI;
}


float PathRunner::fastTurn(const VectorNode *pv) const
{
    if (!m_atVertex)
        return 0;
    if (!hasPrev())
        return 0;
    if (!pv)
        pv = vnode(prev());
    const VectorNode *nv = vnode();
    QVector2D v1(pv->unitVector());
    QVector2D v2(nv->unitVector());
    float a1 = norm_atan2(v1.y(), v1.x());
    float a2 = norm_atan2(v2.y(), v2.x());
    return fixNormAngle(a1 - a2) * 90;
}

QLineF PathRunner::segment() const
{
    return QLineF(nodePoint, nodePoint + vnode()->vector());
}

qreal PathRunner::hopForward()
{
    qreal cpos = m_position;
    if (atHead())
        i = i->n;
    if (atTail())
        return 0;
    qreal dp = vnode()->length() - nodePos;
    m_position += dp;
    if (next()->isVector()) {
        nodePos = 0;
        nodePoint += vnode()->vector();
    } else {
        nodePos = vnode()->length();
        nodePoint = pnode(next())->point();
    }
    i = i->n;
    m_atVertex = true;
    return qAbs(m_position - cpos);
}

qreal PathRunner::hopBack()
{
    if (atTail())
        i = i->p;
    if (atHead()) {
        m_position = 0;
        nodePos = 0;
        nodePoint = pnode()->point();
        m_atVertex = true;
        return 0;
    }
    qreal cpos = m_position;
    if (m_atVertex) {
        i = i->p;
        if (atVector()) {
            m_position -= vnode()->length();
            nodePoint -= vnode()->vector();
        } else {
            m_position = 0;
            nodePoint = pnode()->point();
        }
    } else {
        m_position -= nodePos;
        nodePos = 0;
        m_atVertex = true;
    }
    return qAbs(m_position - cpos);
}

void PathRunner::setPosition(qreal position)
{
    if (m_position < position)
        moveForward(position - m_position);
    else if (m_position > position)
        moveBack(m_position - position);
}

QPointF PathRunner::roundingDelta(qreal range, qreal *aheadCurvature, qreal *behindCurvature) const
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
    GuidePath negPath = windowBehind(range / 2, &rem);
    negVec = negPath.weightedAverage(range, cPt);
    underShoot = rem / range / 2;
    QPointF posVec;
    qreal overShoot = 0;
    GuidePath posPath = windowAhead(range / 2, &rem);
    posVec = posPath.weightedAverage(range, cPt);
    overShoot = rem / range / 2;
//    if (overShoot || underShoot)
//        qDebug() << "underShoot" << underShoot << "overShoot" << overShoot << "pos" << m_position;
    QPointF negPt = negVec * (1 - overShoot) + posVec * overShoot;
    QPointF posPt = posVec * (1 - underShoot) + negVec * underShoot;
    if (behindCurvature)
        *behindCurvature = QPointF::dotProduct(normal(), negPt);
    if (aheadCurvature)
        *aheadCurvature = QPointF::dotProduct(normal(), posPt);
    return negPt + posPt;
}

QVariantMap PathRunner::debugRounding(qreal range) const
{
    QVariantMap ret;
    if (!atVector()) {
        qDebug() << "Round at non-vector element";
        return ret;
    }
    if (range == 0 || isnan(range))
        return ret;
    QPointF cPt = point();
    QPointF negVec;
    qreal underShoot = 0;
    qreal rem;
    GuidePath negPath = windowBehind(range / 2, &rem);
    ret["negPath"] = QVariant::fromValue(negPath);
    negVec = negPath.weightedAverage(range, cPt);
    ret["negVec"] = negVec;
    underShoot = rem / range / 2;
    ret["undershoot"] = underShoot;
    QPointF posVec;
    qreal overShoot = 0;
    GuidePath posPath = windowAhead(range / 2, &rem);
    ret["posPath"] = QVariant::fromValue(posPath);
    posVec = posPath.weightedAverage(range, cPt);
    ret["posVec"] = posVec;
    overShoot = rem / range / 2;
    ret["overshoot"] = overShoot;
//    if (overShoot || underShoot)
//        qDebug() << "underShoot" << underShoot << "overShoot" << overShoot << "pos" << m_position;
    QPointF negPt = negVec * (1 - overShoot) + posVec * overShoot;
    QPointF posPt = posVec * (1 - underShoot) + negVec * underShoot;
    ret["negPt"] = negPt;
    ret["posPt"] = posPt;
    return ret;
}


GuidePath PathRunner::windowAhead(qreal length, qreal *remaining) const
{
    GuidePath ret;
    ret.appendPoint(point());
    if (atTail()) {
        if (remaining)
            *remaining = length;
        return ret;
    }
    qreal l = vnode()->length() - nodePos;
    auto v = i;
    while (length >= l) {
        if (l > 0)
            ret.appendNode(VectorNode(vnode(&v->t)->unitVector(), l));
        length -= l;
        v = v->n;
        if (!v->t.isVector())
            break;
        l = vnode(&v->t)->length();
    }
    if (length > 0) {
        if (v->t.isVector()) {
            ret.appendNode(VectorNode(vnode(&v->t)->unitVector(), length));
            length = 0;
        }
    }
    if (remaining)
        *remaining = length;
//    qDebug() << __func__ << "length" << length << "fact" << ret.length() << ret.count();
    return ret;
}

GuidePath PathRunner::windowBehind(qreal length, qreal *remaining) const
{
    GuidePath ret;
    ret.appendPoint(point());
    if (atHead()) {
        if (remaining)
            *remaining = length;
        return ret;
    }
    qreal l = nodePos;
    auto v = i;
    while (length >= l) {
        if (l > 0)
            ret.appendNode(VectorNode(-vnode(&v->t)->unitVector(), l));
        length -= l;
        v = v->p;
        if (!v->t.isVector())
            break;
        l = vnode(&v->t)->length();
    }
    if (length > 0) {
        if (v->t.isVector()) {
            ret.appendNode(VectorNode(-vnode(&v->t)->unitVector(), length));
            length = 0;
        }
    }
    if (remaining)
        *remaining = length;
//    qDebug() << __func__ << "length" << length << "fact" << ret.length() << ret.count();
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


