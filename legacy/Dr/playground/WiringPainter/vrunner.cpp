#include "vrunner.h"


qreal VRunner::moveBy(qreal len)
{
    if (len > 0) {
        return moveForward(len);
    } else {
        return -moveBack(-len);
    }
}

qreal VRunner::straightLengthBehind(qreal chamferRadius, qreal *ratio) const
{
    qreal dst = lengthBehind();
    if (ratio) {
        *ratio = 1;
    }
    if (!hasPrev())
        return dst;
    return dst - chamferRadius;
}

qreal VRunner::straightLengthAhead(qreal chamferRadius, qreal *ratio) const
{
    qreal dst = lengthAhead();
    if (ratio) {
        *ratio = 1;
    }
    if (!hasNext())
        return dst;
    return dst - chamferRadius;
}

QPointF VRunner::getSmoothedPoint(qreal range) const
{
    QPointF cPt = point();
    if (range == 0 || isnan(range))
        return cPt;
    VRunner neg(*this);
    VRunner pos(*this);
    qreal segLen;

    QPointF negVec;
    qreal underShoot = 0;
    QPointF pp = cPt;

    qreal rem = range / 2;
    while (rem > 0) {
        QPointF vec = neg.toPrevNode(rem, &segLen).toPointF();
        qreal w = segLen / range;
        negVec += (pp + vec / 2) * w;
        if (neg.atStart() && rem > 0) {
            w = (rem - segLen) / range;
            underShoot = (rem - segLen) / range * 2;
            negVec += (pp + vec / 2) * w;
            break;
        }
        rem -= segLen;
        pp = neg.point();
    }
    negVec -= cPt / 2;

    QPointF posVec;
    qreal overShoot = 0;
    pp = cPt;
    rem = range / 2;

    while (rem > 0) {
        QPointF vec = pos.toNextNode(rem, &segLen).toPointF();
        qreal w = segLen / range;
        posVec += (pp + vec / 2) * w;
        if (pos.atEnd() && rem > 0) {
            w = (rem - segLen) / range;
            overShoot = (rem - segLen) / range * 2;
            posVec += (pp + vec / 2) * w;
            break;
        }
        rem -= segLen;
        pp = pos.point();
    }
    posVec -= cPt / 2;

    QPointF d = negVec * (1 - overShoot) + posVec * overShoot +
            posVec * (1 - underShoot) + negVec * underShoot;
    return cPt + d;
}

QVector2D VRunner::toPrevNode(qreal travelLimit, qreal *factTravel)
{
    qreal dst = qMin(travelLimit, lengthBehind());
    if (factTravel)
        *factTravel = dst;
    m_position -= dst;
    if (m_atNode) {
        if(!hasPrev()) {
            m_atStart = true;
            return QVector2D();
        }
        i = i->p;

//        i->t = i->t->prev;
        nodePos = i->t->length();
    }
    if (dst != travelLimit) {
        // dst == lengthBehind, so move to node
        nodePos = 0;
        m_atNode = true;
    } else {
        // move inside current leg
        nodePos -= dst;
        m_atNode = false;
        //        if (node_pos < 1E-12) {
        //            qDebug() << "In fact at leg start, i.e. at node";
        //            m_atNode = true;
        //            i->t = i->t->prev;
        //            node_pos = 0;
        //        }
    }
    m_atEnd = false;
    return -QVector2D(i->t->unitVector() * dst);
}

QVector2D VRunner::toNextNode(qreal travelLimit, qreal *factTravel)
{
    qreal dst = qMin(travelLimit, lengthAhead());
    if (factTravel)
        *factTravel = dst;
    m_position += dst;
    QVector2D ret(i->t->unitVector() * dst);
    if (dst != travelLimit) {
        // dst == lengthAhead, so move to node
        if(!hasNext()) {
            m_atEnd = true;
            nodePos = i->t->length();
        } else {
            i = i->n;
//            i->t = i->t->next;
            m_atEnd = false;
            nodePos = 0;
        }
        m_atNode = true;
    } else {
        // move inside leg OR to end of leg
        nodePos += dst;
        m_atNode = false;

        if (nodePos >= i->t->length()) {
            m_atNode = true;
            if (hasNext()) {
                nodePos -= i->t->length();
                i = i->n;
//                i->t = i->t->next;
            } else {
                m_atEnd = true;
            }
        }
    }
    m_atStart = false;
    Q_ASSERT(i->t != nullptr);
    return ret;
}

qreal VRunner::moveForward(qreal len)
{
    qreal run = m_position;
    m_atStart = false;
    while (len > lengthAhead()) {
        qreal st = lengthAhead();
        len -= st;
        m_position += st;
        m_atNode = true;
        if(!hasNext()) {
            nodePos += st;
            break;
        }
        nodePos = 0;
        i = i->n;
//        i->t = i->t->next;
    }
    if (len >= lengthAhead() && !hasNext()) {
        len = lengthAhead();
        m_atEnd = true;
    }
    if (len > 0) {
        m_position += len;
        nodePos += len;
        //        if (!m_atEnd)
        m_atNode = false;

    }
    return m_position - run;
}

qreal VRunner::moveBack(qreal len)
{
    qreal run = m_position;
    m_atEnd= false;
    while (len >= nodePos) {
        qreal st = nodePos;
        len -= nodePos;
        m_position -= nodePos;
        nodePos = 0;
        m_atNode = true;
        if(!hasPrev()) {
            m_atStart = true;
            break;
        }
        i = i->p;
//        i->t = i->t->prev;
        nodePos = i->t->length();
    }
    if (!m_atStart && len > 0) {
        m_position -= len;
        nodePos -= len;
        m_atNode = false;
    }
    return run - m_position;
}





//#include <QDebug>
//qreal Runner::prevAngularRatio(qreal legPos) const
//{
//    Q_UNUSED(legPos)
//    if (!i->t->prev)
//        return 0;
//    qreal dp = QVector2D::dotProduct(i->t->prev->unitVector, i->t->unitVector);
//    return qSqrt(1-dp);

//}
//qreal Runner::nextAngularRatio(qreal legPos) const
//{
//    Q_UNUSED(legPos)
//    if (!i->t->next)
//        return 0;
//    qreal dp = QVector2D::dotProduct(i->t->unitVector, i->t->next->unitVector);
//    return qSqrt(1-dp);
//}




















