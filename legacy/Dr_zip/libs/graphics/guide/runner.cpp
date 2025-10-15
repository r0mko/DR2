#include "runner.h"
#include "guide.h"
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

qreal Runner::vectorLengthAhead() const
{
    if (atHead() && hasNext())
        return vnode(next())->length();
    else if (i->t.isVector())
        return vnode()->length() - nodePos;
    return 0;
}

qreal Runner::vectorLengthBehind() const
{
    if (atHead())
        return 0;
    if (m_atVertex && hasPrev())
        return vnode(prev())->length();
    return nodePos;
}

//Guide Runner::beforeGuide() const
//{
//    Guide ret;
//    Runner r(*this);
//    ret.appendPoint(r.point());
//    while(!r.atHead()) {
//        r.hopBack();
//        VectorNode n = ret.createNextNode(r.point());
//        if (n.length() > 0)
//            ret.appendNode(n);
//    }
//    return ret;

//}


//Guide Runner::afterGuide() const
//{
//    Guide ret;
//    Runner r(*this);
//    ret.appendPoint(r.point());
//    while(!r.atTail()) {
//        r.hopForward();
//        VectorNode n = ret.createNextNode(r.point());
//        if (n.length() > 0)
//            ret.appendNode(n);
//    }
//    return ret;
//}

qreal Runner::moveForward(qreal len)
{
    if (len <= 0)
        return 0;
    qreal cpos = m_position;
    while (len >= vectorLengthAhead()) {
        len -= hopForward();
        if (atPoint()) {
            //            qDebug() << "Reached end by move forward" << len << m_position;
            break;
        }
    }
    if (!atPoint())
        nearMove(len);
    return qAbs(m_position - cpos);
}

qreal Runner::moveBack(qreal len)
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
            i = i->p;
        }
        nearMove(-len);
    }
    return qAbs(m_position - cpos);
}

qreal Runner::strictMoveForward(qreal len, GuideNode::NodeType nodeType)
{
    if (len <= 0)
        return 0;
    qreal cpos = m_position;
    while (len >= vectorLengthAhead()) {
        len -= hopForward();
        if (currentNodeType() != nodeType)
            break;
    }
    if (currentNodeType() == nodeType)
        nearMove(len);
    return qAbs(m_position - cpos);
}

qreal Runner::strictMoveBack(qreal len, GuideNode::NodeType nodeType)
{
    if (len <= 0)
        return 0;
    qreal cpos = m_position;

    while (currentNodeType() == nodeType && len >= vectorLengthBehind()) {
        len -= hopBack();
    }
    if (currentNodeType() == nodeType) {
        if (m_atVertex) {
            if (prev()->type() == nodeType) {
                VectorNode *pv = vnode(prev());
                nodePos = pv->length();
                nodePoint -= pv->vector();
                m_atVertex = false;
                i = i->p;
            }
        }
        nearMove(-len);
    }
    return qAbs(m_position - cpos);
}

QPolygonF Runner::negativePart(qreal length) const
{
    qreal rem;
    return windowBehind(length, &rem).toPolygon();
}

QPolygonF Runner::positivePart(qreal length) const
{
    qreal rem;
    return windowAhead(length, &rem).toPolygon();

}

void Runner::nearMove(qreal dst)
{
    if (dst == 0) {
        return;
    }
    m_position += dst;
    nodePos += dst;
    m_atVertex = nodePos == 0;
}

Runner::Runner(GuideIterator iterator, QPointF point) :
    GuideIterator(iterator)
{
    if (i->t.isPoint()) {
        nodePoint = pnode()->point();
        m_atVertex = true;
        if (atTail() && hasPrev()) {
            i = i->p;
            nodePos = vnode()->length();
            nodePoint -= vnode()->vector();
        } else if(atHead() && hasNext()) {
            i = i->n;
        }
    }
    if (nodePoint.isNull() && !point.isNull())
        nodePoint = point;
}



Runner::Runner(Guide path, qreal pos) :
    GuideIterator(path.d->nodes.cbegin())
{
    if (i->t.isPoint()) {
        nodePoint = pnode()->point();
        i = i->n;
    }
    moveForward(pos);
}

Runner::Runner(QLinkedListNode<GuideNode> *node, QPointF pos)
    : GuideIterator(node)
{
    qDebug() << "Created runner at node type" << node->t.type() << "point" << pos;
    nodePoint = pos;
}

qreal Runner::position() const
{
    return m_position;
}

void Runner::setPoint(const QPointF &pt) { nodePoint = pt; }

QPointF Runner::point() const
{
    if (atPoint())
        return pnode()->point();
    return nodePoint + vnode()->unitVector() * nodePos;
}

qreal Runner::angle() const
{
    if (!atVector())
        return 0;
    QPointF v = vnode()->unitVector();
    return qAtan2(v.y(), v.x()) * 180 / M_PI;
}

QPointF Runner::normal() const
{
    return vnode()->normal();

}

QPointF Runner::averageNormal(qreal range) const
{
    qreal nr = range / 2;
    qreal pr = range / 2;
    QPointF nv;
    QPointF pv;
    qreal offset = 0;
    Runner n(*this);
    if (!n.atVector())
        offset = n.hopBack();
    while (nr > n.vectorLengthBehind() && n.c()->isVector()) {
        nv += (n.normal() * n.vectorLengthBehind()) / range;
        nr -= n.hopBack();
    }
    if (nr > 0 && !n.atPoint()) {
        nv += (n.normal() * nr);
    }

    Runner p(*this);
    if (!n.atVector())
        n.hopBack();
    while (pr > p.vectorLengthBehind() && p.c()->isVector()) {
        pv += (p.normal() * p.vectorLengthAhead()) / range;
        pr -= p.hopForward();
    }
    if (pr > 0 && !p.atPoint()) {
        pv += (p.normal() * pr);
    }
    if (pr > 0)
        qDebug() << "pos overshoot" << pr;
    if (nr > 0)
        qDebug() << "neg overshoot" << nr;
    return QPointF(QVector2D(nv + pv).normalized().toPointF());
}

QPointF Runner::offsetPoint(qreal distance) const
{
    return point() + normal() * distance;
}

qreal Runner::dotProduct() const
{
    if (!m_atVertex)
        return 1;
    if (!hasPrev())
        return -INFINITY;
    return QPointF::dotProduct(vnode(prev())->unitVector(), vnode()->unitVector());

}
qreal Runner::turnAngle(const VectorNode *pv) const
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


float Runner::fastTurn(const VectorNode *pv) const
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

QLineF Runner::segment() const
{
    return QLineF(nodePoint, nodePoint + vnode()->vector());
}

void Runner::toNearestVector()
{
    if (atVector())
        return;
    if (atTail() && hasPrev()) {
        i = i->p;
        nodePos = vnode()->length();
        nodePoint -= vnode()->vector();
    } else if (atHead() && hasNext()) {
        i = i->n;
    }
}

qreal Runner::hopForward()
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

qreal Runner::hopBack()
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
        if (c()->isVector()) {
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

void Runner::setPosition(qreal position)
{
    if (m_position < position)
        moveForward(position - m_position);
    else if (m_position > position)
        moveBack(m_position - position);
}

QPointF Runner::roundingDelta(qreal range, QPointF *negPoint, QPointF *posPoint) const
{
    //if (range == 0 || isnan(range))
    if (range == 0 || qIsNaN(range))
        return QPointF();
    QPointF cPt = point();
    QPointF negVec;
    qreal underShoot = 0;
    qreal rem;
    Guide negPath = windowBehind(range / 2, &rem);
    negVec = negPath.weightedAverage(range, cPt);
    underShoot = rem / range * 2 ;
    QPointF posVec;
    qreal overShoot = 0;
    Guide posPath = windowAhead(range / 2, &rem);
    posVec = posPath.weightedAverage(range, cPt);
    overShoot = rem / range * 2;
    QPointF negPt = negVec * (1 - overShoot) + posVec * overShoot;
    QPointF posPt = posVec * (1 - underShoot) + negVec * underShoot;
    if (negPoint)
        *negPoint = negPt;
    if (posPoint)
        *posPoint = posPt;
    return negPt + posPt;
}

QVariantMap Runner::debugRounding(qreal range) const
{
    QVariantMap ret;
    if (!atVector()) {
        qDebug() << "Round at non-vector element";
        return ret;
    }
    //    if (range == 0 || isnan(range))
    if (range == 0 || qIsNaN(range))
        return ret;
    QPointF cPt = point();
    QPointF negVec;
    qreal underShoot = 0;
    qreal rem;
    Guide negPath = windowBehind(range / 2, &rem);
    ret["negPath"] = QVariant::fromValue(negPath);
    negVec = negPath.weightedAverage(range, cPt);
    ret["negVec"] = negVec;
    underShoot = rem / range / 2;
    ret["undershoot"] = underShoot;
    QPointF posVec;
    qreal overShoot = 0;
    Guide posPath = windowAhead(range / 2, &rem);
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


Guide Runner::windowAhead(qreal length, qreal *remaining) const
{
    Guide ret;
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

Guide Runner::windowBehind(qreal length, qreal *remaining) const
{
    Guide ret;
    ret.appendPoint(point());
    if (atHead()) {
        if (remaining)
            *remaining = length;
        return ret;
    }
    qreal l = nodePos;
    auto v = i;
    qreal lenRem = length;
    while (lenRem >= l) {
        if (l > 0)
            ret.appendNode(VectorNode(-vnode(&v->t)->unitVector(), l));
        lenRem -= l;
        v = v->p;
        if (!v->t.isVector())
            break;
        l = vnode(&v->t)->length();
    }
    if (lenRem > 0) {
        if (v->t.isVector()) {
            ret.appendNode(VectorNode(-vnode(&v->t)->unitVector(), lenRem));
            lenRem = 0;
        }
    }
    if (remaining)
        *remaining = lenRem;
    //    Q_ASSERT(ret.length() <= length);
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


