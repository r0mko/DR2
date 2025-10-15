#include "pathrunner.h"
#include "guidepath.h"
#include <QDebug>
#include <QtMath>

GuidePath::GuidePath()
{
    init();
}

GuidePath::~GuidePath()
{
}

<<<<<<< .mine
float invSqrt(float x){
    float xhalf = 0.5f * x;
    int i = *(int*)&x;          // store floating-point bits in integer
    i = 0x5f3759d5 - (i >> 1);  // initial guess for Newton's method
    x = *(float*)&i;            // convert new bits into float
    x = x*(1.5f - xhalf*x*x);   // One round of Newton's method
    return x;
}
=======


>>>>>>> .r2725


void GuidePath::trim(const PathIterator &after)
{
    PathIterator i_lv = --PathIterator(d->iTail);
    while (i_lv != after) {
        const PathNode &lv = *i_lv;
        if (!lv.isVector()) {
            qDebug() << "cannot erase node" << lv.type();
            return;
        }
        VectorNode *vn = i_lv.vnode();
        tailNode().pointRef() -= vn->vector();
        d->length -= vn->length();
        i_lv = --d->nodes.erase(i_lv);
    }
}


GuidePath GuidePath::offseted(qreal offset, bool fast) const
{
    GuidePath result;
    if (isEmpty())
        return result;

    QPointF sourcePt = headNode().point();
    QPointF offsetPt = sourcePt + firstVNode().normal() * offset;
    result.appendPoint(offsetPt);
    int i = 0;
    QPointF lastValidPoint;
    auto i_lastValidNode = first();
    bool inCusp = false;
    for (PathIterator i_prevVec = first(); i_prevVec != tail(); ++i_prevVec) {
        ++i;
        PathIterator i_nextVec = i_prevVec + 1;

        QVector2D cv = QVector2D(i_nextVec.vnode()->unitVector());
        if (!i_nextVec->isVector())
            break;
        //        if (cv.length() != 1)
        //            qDebug() << "current vector UV" << cv.length() << i << i_nextVec.c()->type();
        float dot = QPointF::dotProduct(i_prevVec.vnode()->unitVector(), -i_nextVec.vnode()->unitVector());
        QPointF nextnorm = i_nextVec.vnode()->unitVector();
        qSwap(nextnorm.rx(), nextnorm.ry() *= -1);
        qreal bs = QPointF::dotProduct(i_prevVec.vnode()->unitVector(), nextnorm);

        qreal bisectorLength = offset * invSqrt((1 - dot) / 2)  * sgn(bs);
        QVector2D bisect = QVector2D(i_prevVec.vnode()->unitVector() - i_nextVec.vnode()->unitVector());
        QVector2D bisectUnit = bisect.normalized();
        //        qDebug() << "bisector length" << bisectorLength << "bs length" << bisect.length() << "bs length ^2" << bisect.lengthSquared();
        sourcePt += i_prevVec.vnode()->vector();
        offsetPt = sourcePt + (bisectUnit * bisectorLength).toPointF();
        result.appendPoint(offsetPt);
        if (fast)
            continue;
        // fix self-intersection
        qreal dot_intersect = QPointF::dotProduct(result.lastVNode().unitVector(), i_prevVec.vnode()->unitVector());
        if (dot_intersect < 0)  {
            if (!inCusp) {
                //                qDebug() << "Self-intersect at" << i << dot_intersect << result.lastVectorNode().length() << i_nextVec->length() << result.lastVectorNode().length();
                inCusp = true;
                lastValidPoint = result.tailNode().point();
                i_lastValidNode = result.last();
            }
        } else {
            if (inCusp) {
                QLineF likelyValidSegment(offsetPt, offsetPt - result.lastVNode().vector());
                //                qDebug() << "Possible leave cusp at" << i << dot_intersect << result.lastVectorNode().length() << likelyValidSegment.length() << i_lastValidNode->length();
                auto it_back = i_lastValidNode;
                QPointF endPoint = lastValidPoint;
                while (it_back != result.head()) {
                    QLineF segment(endPoint - it_back.vnode()->vector(), endPoint);
                    QPointF intersectPoint;
                    QLineF::IntersectType itype = likelyValidSegment.intersect(segment, &intersectPoint);
                    if (itype == QLineF::BoundedIntersection) {
                        //                        qDebug() << "intersected!" << i ;
                        result.trim(--it_back);
                        result.appendPoint(intersectPoint);
                        //                        QLineF validSegment = Q
                        inCusp = false;
                        break;
                    }
                    endPoint -= it_back.vnode()->vector();
                    it_back--;
                }
            }
        }
    }
    sourcePt = tailNode().point();
    offsetPt = sourcePt + lastVNode().normal() * offset;
    if (inCusp) {
        result.trim(i_lastValidNode);
    }
    result.appendPoint(offsetPt);
    return result;
}

QPainterPath GuidePath::outline(qreal width, Qt::PenCapStyle caps) const
{
    if (isEmpty())
        return QPainterPath();
    GuidePath leftPath = offseted(-width / 2, true);
    GuidePath rightPath = offseted(width / 2, true);
    QPainterPath left = leftPath.toPath();
    QPainterPath right = rightPath.toPath().toReversed();
    qreal k = 0.5522847498;
    switch (caps) {
    case Qt::RoundCap: {
        QPointF lP = lastVNode().unitVector() * width / 2 * k;
        QPointF lN = lastVNode().normal() * width / 2 * k;
        QPointF lcP = tailNode().point() + lastVNode().unitVector() * width / 2;
        left.cubicTo(leftPath.tailNode().point() + lP, lcP - lN, lcP);
        left.cubicTo(lcP + lN, rightPath.tailNode().point() + lP, rightPath.tailNode().point());
        break;
    }
    case Qt::SquareCap:
        left.lineTo(leftPath.tailNode().point() + lastVNode().unitVector() * width / 2);
        left.lineTo(rightPath.tailNode().point() + lastVNode().unitVector() * width / 2);
        break;
    case Qt::FlatCap:
    default:
        break;
    }

    //    left.arcTo();
    for (int i = 0; i < right.elementCount(); ++i) {
        auto e = right.elementAt(i);
        if (e.isLineTo() || e.isMoveTo()) {
            left.lineTo(e);
        }
    }
    switch (caps) {
    case Qt::RoundCap: {
        QPointF fP = -firstVNode().unitVector() * width / 2 * k;
        QPointF fN = firstVNode().normal() * width / 2 * k;
        QPointF fcP = headNode().point() - firstVNode().unitVector() * width / 2;
        left.cubicTo(left.currentPosition() + fP, fcP + fN, fcP);
        left.cubicTo(fcP - fN, leftPath.headNode().point() + fP, leftPath.headNode().point());
        break;
    }
    case Qt::SquareCap:
        left.lineTo(rightPath.headNode().point() - firstVNode().unitVector() * width / 2);
        left.lineTo(leftPath.headNode().point() - firstVNode().unitVector() * width / 2);
        break;
    case Qt::FlatCap:
    default:
        break;
    }

    left.closeSubpath();
    return left;

}

QPainterPath GuidePath::toPath() const
{
    QPainterPath ret;
    QPointF stPoint = headNode().point();
    ret.moveTo(stPoint);
    for (PathIterator it = d->iHead; it != d->iTail; ++it) {
        stPoint += it.vnode()->vector();
        ret.lineTo(stPoint);
    }
    return ret;
}

QPolygonF GuidePath::toPolygon() const
{
    QPolygonF ret;
    QPointF stPoint = headNode().point();
    ret.append(stPoint);
    for (PathIterator it = d->iHead; it != d->iTail; ++it) {
        stPoint += it.vnode()->vector();

        ret.append(stPoint);
    }
    //    ret.append(lastNode.point);
    return ret;
}


void GuidePath::appendPoint(const QPointF &point)
{
    if (d->nodes.isEmpty())
        init();
    if (isEmpty() && headNode().point().isNull()) {
        headNode().setPoint(point);
        tailNode().setPoint(point);
        return;
    }
    VectorNode node = createNextNode(point);
    if (node.length() < 1E-5)
        return;
    d->length += node.length();
    tailNode().setPoint(point);
    d->iTail = ++d->nodes.insert(d->iTail, node);
    d->iHead = ++d->nodes.begin();
    //    qDebug() << "Appending point" << point << "count" << d->nodes.size() << "length" << d->length << d->iHead->type() << d->iTail->type();
    //    dumpNodes();
}

void GuidePath::setPolygon(const QPolygonF &polygon)
{
    if(polygon.size() < 2) {
        return;
    }
    clear();
    for (const QPointF &pt : polygon) {
        appendPoint(pt);
    }
    //    for (int i = 0; i < polygon.size(); ++i) {
    //        appendPoint();
    //        const QPointF &p1 = polygon[i-1];
    //        const QPointF &p2 = polygon[i];
    //        VectorNode n(p1, p2);
    //        if (n.length() != 0)
    //            appendNode(n);
    //    }
    //    headNode().pointRef() = polygon.first();
    //    tailNode().pointRef() = polygon.last();
    //    qDebug() << "added polygon size" << polygon.size() << "count" << count();
}

GuidePath GuidePath::clone()
{
    d.detach();
    d->nodes.detach();
    //    copy.d->nodes.detach();
    d->iHead = ++d->nodes.begin();
    d->iTail = --d->nodes.end();
    return *this;
}

GuidePath GuidePath::part(qreal p1, qreal p2) const
{
    GuidePath ret;
    qreal start = qMin(p1, p2);
    qreal end = qMax(p1, p2);
    PathRunner r(this, start);
    ret.appendPoint(r.point());
    qreal dist = end - start;
    while (dist > r.vectorLengthAhead() && !r.atTail()) {
        dist -= r.hopForward();
        ret.appendPoint(r.point());
    }
    if (dist > 0 && !r.atTail()) {
        ret.appendPoint(r.point());
        if (dist == r.vectorLengthAhead()) {
            r.hopForward();
        } else {
            Q_ASSERT(dist < r.vectorLengthAhead());
            r.moveForward(dist);
        }
    }
    ret.appendPoint(r.point());
    if (p1 > p2)
        return ret.reversed();
    return ret;
}

void GuidePath::appendPath(GuidePath other)
{
    if (other.isEmpty())
        return;

    qDebug() << "Appending path count" << other.count() << "l =" << other.length() << "into source path count" << count() << "l =" << length();

    QPointF pt = other.headNode().point();
    //    if (isEmpty())
    //        appendPoint(pt);
    if (tailNode().point() != pt) {
        qDebug() << "Gap between paths" << QLineF(tailNode().point(), pt).length();
        appendPoint(pt);
    }
    for (PathIterator it = other.first(); it != other.tail(); ++it) {
        //        pt += it.vnode()->vector();
        //        appendPoint(pt);
        const VectorNode &n = *it.vnode();
        if (n.length() < 1E-5) {
            qWarning() << "Zerolength node" << n.length() << n.unitVector();
        } else {
            //            qDebug() << "Appending vnode length"<< n.length() << "vector" << n.unitVector();
            appendNode(n);
        }
    }
    appendPoint(other.tailNode().point());




    ////    if (isEmpty())
    ////        tailNode().setPoint(headNode().point());

}


PathRunner GuidePath::findNearestPos(const QPointF &pos)
{
    PathRunner r(this);
    PathRunner ret(this);
    //    r.setOriginPoint(origin);
    //    r._toBegin();
    qreal mindst = 1e127;
    QVector2D pv(pos);
    while (!r.atTail()) {
        qreal dst = pv.distanceToLine(QVector2D(r.nodePoint), QVector2D(r.unitVector()));
        if (dst < mindst) {
            QPointF endP = (r.normal() * (dst + 1));
            QLineF l_perpendicular(pos - endP, pos + endP);
            QPointF iPoint;
            QLineF::IntersectType sect = r.segment().intersect(l_perpendicular, &iPoint);
            if (sect == QLineF::BoundedIntersection) {
                mindst = dst;
                qreal seg_pos = QLineF(r.nodePoint, iPoint).length();
                ret = r;
                ret.moveForward(seg_pos);
                //                ret.setPosition(r.position() + seg_pos);
            }
        }
        qreal stlen = QLineF(pos, r.nodePoint).length();
        if (stlen < mindst) {
            mindst = stlen;
            ret = r;
        }
        r.hopForward();
    }
    qreal stlen = QLineF(pos, r.point()).length();
    if (stlen < mindst) {
        mindst = stlen;
        ret = r;
    }
    //    qDebug() << Q_FUNC_INFO << "pos:" << ret.position() << "point:" << ret.point() << "dst:" << mindst << "atEnd:" << ret.atEnd() << "atStart:" << ret.atStart();
    return ret;
}

//QPointF VChain::nearestPoint(const QPointF &pos, qreal *length) const
//{
//    QPointF ret;
//    QVector2D point(pos);
//    qreal len = 0;
//    qreal mindst = 1e+127;
//    for (auto it = i_head; it != i_tail; ++it) {
////    for (int i = 0; i < nodes.size(); ++i) {
//        const VNode *n = *it;
//        //        qreal c_dst = c_vec.length();
//        qreal dst = point.distanceToLine(QVector2D(n->point), n->unitVector);
//        if (dst < mindst) {
//            QPointF endP = (n->normal() * (dst + 1)).toPointF();
//            QLineF l_perpendicular(pos - endP, pos + endP);
//            QPointF iPoint;
//            QLineF::IntersectType sect = n->segment().intersect(l_perpendicular, &iPoint);
//            if (sect == QLineF::BoundedIntersection) {
//                mindst = dst;
//                ret = iPoint;
//                qreal seg_pos = QLineF(n->point, iPoint).length();
//                *length = len + seg_pos;
//            }
//        }
//        QLineF st(pos, n->point);
//        qreal stlen = st.length();
//        if (stlen < mindst) {
//            mindst = stlen;
//            *length = len;
//            ret = n->point;
//        }
//        len += n->length;
//    }
//    QLineF e(pos, lastVector()->endPoint());
//    qreal elen = e.length();
//    if (elen < mindst) {
//        *length = m_length;
//        ret = lastVector()->endPoint();
//    }
//    return ret;
//}

void GuidePath::clear()
{
    //    qDebug() << "cleared path size" << d->nodes.size();
    d.detach();
    init();
    headNode().setPoint(QPointF());
    tailNode().setPoint(QPointF());
    //    qDebug() << "new path size" << d->nodes.size();

}

QPainterPath GuidePath::wavefront_debug(qreal offset) const
{
    QPainterPath ret;
    if (isEmpty())
        return ret;
    Wavefront wf(*this);
    QPointF pt = wf.sourcePoint();

    for (QLinkedList<Wavefront::node>::const_iterator ii = wf.nodes.constBegin(); ii != wf.nodes.constEnd(); ++ii) {
        //        qreal len = ii->maxOffset();
        //        if (len < 0) {
        //            pt += ii->vnode.vector();
        //            ret.moveTo(pt);
        //            continue;
        //        }
        //        ret.lineTo(pt + qMin(wf.minPosLength, offset) * ii.i->p->t.bsuv.toPointF());
        //        ret.moveTo(pt);
        ret.moveTo(pt);
        ret.lineTo(pt + qMin(offset, wf.maxPositiveOffset()) * ii->bsuv.toPointF());
        pt += ii->vnode.vector();
        //        qDebug() << qPrintable(QString("segment (%1:%2) length = %3 maxoffset = %4, ldecay %5 rdecay %6").arg(i - 1).arg(i).arg(l).arg(len).arg(ldecay).arg(rdecay));

    }
//    qDebug() << "shortest pos offset" << wf.maxPositiveOffset() << offset << ret.elementCount() << ret.length();
    GuidePath r = wf.getOffset(offset);
    ret += r.toPath();
    return ret;
}

QRectF GuidePath::boundingRect() const
{
    if (isEmpty())
        return QRectF();
    QPointF pd = headNode().point();
    qreal minx, maxx, miny, maxy;
    minx = maxx = pd.x();
    miny = maxy = pd.y();
    for (PathIterator it = d->iHead; it != d->iTail; ++it) {
        pd += it.vnode()->vector();
        if (pd.x() < minx)
            minx = pd.x();
        else if (pd.x() > maxx)
            maxx = pd.x();
        if (pd.y() < miny)
            miny = pd.y();
        else if (pd.y() > maxy)
            maxy = pd.y();
    }
    return QRectF(minx,miny, maxx - minx, maxy - miny);
}

void GuidePath::dumpNodes() const
{
    QDebug dbg = qDebug().nospace();
    for (PathIterator it = d->nodes.begin(); it != d->nodes.end(); ++it) {
        if (it->isVector())
            dbg << "(dx: " << it.vnode()->unitVector().x() << " dy: " << it.vnode()->unitVector().y() << ") len: " << it.vnode()->length();
        else if (it->isPoint()) {
            dbg << " point: " << it.pnode()->point();
        }
        dbg << " ]";
        if(it->isVector())
            dbg << "->\n";
    }
}

QPointF GuidePath::headPoint() const
{
    if (d->nodes.isEmpty())
        return QPointF();
    return headNode().point();
}

QPointF GuidePath::tailPoint() const
{
    if (d->nodes.isEmpty())
        return QPointF();
    return tailNode().point();
}

qreal GuidePath::checkLength()
{
    qreal l = 0;
    for (PathIterator it = d->iHead; it != d->iTail; ++it) {
        l += it.vnode()->length();
    }
    qreal dl = d->length - l;
    if (d->length != l) {
        if (qAbs(dl) > 1)
            qDebug() << "length mismatch, delta =" << d;
        d->length = l;
    }
    return dl;
}
const VectorNode &GuidePath::firstVNode() const
{
    Q_ASSERT(d->nodes.size() > 2);
    return *d->iHead.vnode();
}

VectorNode &GuidePath::firstVNode()
{
    Q_ASSERT(d->nodes.size() > 2);
    return *d->iHead.vnode();
}

const VectorNode &GuidePath::lastVNode() const
{
    Q_ASSERT(d->nodes.size() > 2);
    return *d->iTail.vnode(d->iTail.prev());
}

VectorNode &GuidePath::lastVNode()
{
    Q_ASSERT(d->nodes.size() > 2);
    return *d->iTail.vnode(d->iTail.prev());
}



qreal GuidePath::dotToPoint(const QPointF &point) const
{
    if (isEmpty())
        return -1;
    QPointF vec = QVector2D(point - tailNode().point()).normalized().toPointF();
    return QPointF::dotProduct(vec, -lastVNode().unitVector());

}

VectorNode GuidePath::createNextNode(const QPointF &nextPt) const
{
    QVector2D v(nextPt - tailNode().point());
    VectorNode node;
    node.l = v.length();
    node.pt = v.toPointF() / node.l;
    return node;
}

GuidePath::GuidePath(GuidePath::GuidePathData *data) :
    d(data)
{}




QPointF GuidePath::weightedAverage(qreal range, const QPointF &origin)
{
    QPointF ret;
    QPointF pp = origin;
    if (isEmpty())
        return ret;
    for (PathIterator it = d->iHead; it != d->iTail; ++it) {
        const VectorNode &v = *it.vnode();
        qreal w = v.length() / range;
        ret += (pp + v.vector() / 2) * w;
        pp += v.vector();
    }
    if (range / 2 > length()) {
        qreal rem = range / 2 - length();
        qreal w = rem / range;
        ret += pp * w;
    }
    return ret - origin / 2;
}

//GuidePath GuidePath::smoothed(qreal radius, qreal precision, qreal startPos, qreal endPos) const
//{
//    GuidePath ret;
//    if (isEmpty())
//        return ret;
//    if (endPos < 0)
//        endPos = d->length;
//    //    ret.appendPoint(headNode().point());
//    PathRunner r(this, startPos);
//    qreal angleTreshold = 6 / precision;
//    QPointF currentPt;
//    qreal curveFwd, curveBack, step, turnAhead, turnBehind, marginBehind, marginAhead, sharpness;

//    currentPt = r.point() + r.roundingDelta(radius, &curveFwd, &curveBack);
//    ret.appendPoint(currentPt);

//    do {
//        turnAhead = r.hasNext() ? r.vnode()->fastAngleTo(*r.vnode(r.next())) : 0;
//        turnBehind = r.hasPrev() ? r.vnode(r.prev())->fastAngleTo(*r.vnode()) : 0;
//        marginBehind = (turnBehind < angleTreshold) ? 0 : radius / 2;
//        marginAhead = (turnAhead < angleTreshold) ? 0 : radius / 2;
//        bool straight = (r.nodePos > marginBehind && r.nodePos < r.vnode()->length() - marginAhead);

//        if (straight) {
//            step = qMin(endPos - r.position(), r.vectorLengthAhead() - marginAhead);
//            if (marginAhead == 0) {
//                r.hopForward();
//                ret.appendPoint(r.point());
//                continue;
//            } else {
//                r.moveForward(step);
//                straight = false;
//            }
//        }
//        step = qMin(endPos - r.position(), radius / (qAbs(turnAhead) * precision) * 6);
//        if (step < 0) {
//            qWarning() << "Step < 0" << "at node" << ret.count() << "radius"  << radius  << "turn"  << turnAhead << "step" << step<< "straight" << straight << "pos" << r.position() << "endPos" << endPos;
//            step = 5;
//        }
//        r.moveForward(step);
//        if (!r.atVector() || r.position() > endPos)
//            break;
//        const VectorNode &reference = ret.isEmpty() ? *r.vnode() : ret.lastVectorNode();
//        currentPt = r.point() + r.roundingDelta(radius, &curveFwd, &curveBack);
//        VectorNode n = ret.createNextNode(currentPt);
//        sharpness = qAbs(n.fastAngleTo(reference));
//        if (sharpness > 0) {
//            ret.appendNode(n);
//            qreal ratio = sharpness / angleTreshold;
//            qDebug() << "at node" << ret.count()
//                     << "turn_sharpness" << sharpness
//                     << "weighted" << sharpness * n.length()
//                     << "ratio" << ratio
//                     << "step" << step
//                     << "turnAhead" << turnAhead
//                     << "turnBehind" << turnBehind
//                     << "straight" << straight
//                     << "pos" << r.position()
//                     << "ahead" << r.vectorLengthAhead() << endPos;

//        }
//    } while (!r.atTail() && r.position() < endPos);
//    ret.appendPoint(r.point());
//    return ret;
//}

GuidePath GuidePath::smoothed(qreal m_radius, qreal m_precision, qreal m_startPos, qreal m_endPos) const
{
    GuidePath m_result;
    if (isEmpty())
        return m_result;
    qreal step = 0;

    if (m_endPos < 0)
        m_endPos = d->length;
    //    ret.appendPoint(headNode().point());
    PathRunner vertexRunner(this, m_startPos);
    PathRunner smoothRunner(this, m_startPos);
    m_result.appendPoint(vertexRunner.point());
    vertexRunner.hopForward();

    bool hasPoint = true;
    while (!vertexRunner.atTail()) {
        float turn = vertexRunner.fastTurn();
        //        float turn = vertexRunner.fastTurn(ret.isEmpty() ? nullptr : &ret.lastVectorNode());
        if (turn < 0)
            turn *= -0.4; //
        if (turn > 0) {
            //            if (!step || )

            step = m_radius / (turn * m_precision) * 6;
            //            qreal step = radius / (qSqrt(turn) * precision);
            qreal negLimit = vertexRunner.position() - m_radius / 2;
            qreal posLimit = vertexRunner.position() + m_radius / 2;
            //                        qreal negLimit = vr.position() - qMin(vr.lengthBehind(), radius / 2);
            //                        qreal posLimit = vr.position() + qMin(vr.lengthAhead(), radius / 2);
            //                        qreal delta = negLimit - pr.position();
            if (negLimit > smoothRunner.position()) {
                //                qDebug() << "span:" << posLimit - negLimit;
                smoothRunner.moveForward(negLimit - smoothRunner.position());
                hasPoint = false;
            }
            if (smoothRunner.position() + step > posLimit) {
                //                ret.appendPoint(vertexRunner.point());
                //                VectorNode n = ret.createNextNode(vertexRunner.point() + vertexRunner.roundingDelta(radius));
                ////                qDebug() << ret.count() << "node alone, vecnode len:" << n.length() << "step" << step << "pos" << smoothRunner.position() << "vert pos" << vertexRunner.position() << "turn" << turn;
                //                if (vertexRunner.position() > smoothRunner.position()) {
                //                    ret.appendNode(n);
                //                }
                //                if (turn < 2 / precision) {
                //                    if (!step || step < n.length()) {
                //    //                    vertexRunner.hopForward();
                //                        continue;
                //                    }
                //                }
                vertexRunner.hopForward();
                continue;
            }
            while (!smoothRunner.atTail() && smoothRunner.position() < posLimit) {
                if (!hasPoint) {
                    const VectorNode &reference = m_result.isEmpty() ? *smoothRunner.vnode() : m_result.lastVNode();
                    QPointF pt = smoothRunner.point() + smoothRunner.roundingDelta(m_radius);
                    //                    currentPt = r.point() + r.roundingDelta(radius, &curveFwd, &curveBack);
                    VectorNode n = m_result.createNextNode(pt);
                    if (step / n.length() < 10) {

                        //                    if (n.length() < )
                        float sharpness = qAbs(n.fastAngleTo(reference));
                        //                        qDebug() << "at node" << ret.count()
                        //                                 << "\npos" << smoothRunner.position()
                        //                                 << "\nturn sharpness" << sharpness
                        //                                 << "\nstepratio" << step /n.length()
                        //                                 << "\nvec length:" << n.length();

                        m_result.appendNode(n);
                        hasPoint = true;
                        if (sharpness == 0) {
                            //                    } else if (vertexRunner.position() < smoothRunner.position()) {
                            //                            qDebug() << "nothing to smooth at node" << ret.count();
                            break; // go to next vertex if current rounding is complete
                        }
                    }
                }
                if (!smoothRunner.atTail() && (smoothRunner.position() + step) <= posLimit) {
                    smoothRunner.moveForward(step);
                    hasPoint = false;
                } else {
                    break;
                }
            }
        }
        vertexRunner.hopForward();
    }
    //    qDebug() << vr.point() << pr.point() << vr.currentNodeType() << pr.currentNodeType();
    m_result.appendPoint(vertexRunner.point());
    //    qDebug() << "Rounding done" << ret.count() << (ret.head() == ret.tail());
    return m_result;
}

GuidePath GuidePath::simplified(qreal threshold, qreal range) const
{
    GuidePath ret;
    if (isEmpty())
        return ret;
    PathRunner vr(this);
    ret.appendPoint(vr.point());
    //    QPointF pp = vr.point();
    vr.hopForward();
    int c = 0;
    while (!vr.atTail()) {
        qreal fc, rc;
        QPointF dp_pt(vr.roundingDelta(range, &fc, &rc));
        qreal rem;
        GuidePath p_pos = vr.windowAhead(range / 2, &rem);
        GuidePath p_neg = vr.windowBehind(range / 2, &rem);

        QVector2D dp(dp_pt);
        //        QPointF axis_pt(vr.point() + dp_pt - pp);
        QPointF axis_pt(p_pos.tailPoint() - p_neg.tailPoint());
        //        pp = vr.point() + dp_pt;

        qSwap(axis_pt.rx(), axis_pt.ry() *= -1);
        QVector2D axisnorm(axis_pt);


        qreal contrib = qAbs(QVector2D::dotProduct(axisnorm.normalized(), QVector2D(vr.vnode()->vector())));

        qDebug() << "At node" << c++
                 << "dist^2" << dp.lengthSquared()
                    //                 << "node length" << vr.vnode()->length()
                 << "contrib" << contrib
                 << "threshold" << threshold
                 << "neg" << p_neg.tailPoint() - vr.point()
                 << "pos" << p_pos.tailPoint() - vr.point();

        if (contrib > threshold)
            ret.appendPoint(vr.point());
        vr.hopForward();
    }
    ret.appendPoint(vr.point());
    return ret;
}


GuidePath GuidePath::reversed() const
{
    GuidePath ret;
    QPointF pt = tailNode().point();
    ret.appendPoint(pt);

    for (PathIterator it = last(); it != head(); --it) {
        //                Q_ASSERT(it->isVector());
        pt -= (it.vnode()->vector());

        ret.appendPoint(pt);
        //        VectorNode n = *it.vnode();
        //        n.setUnitVector(-n.unitVector());
        //        ret.appendNode(n);
    }

    ret.appendPoint(headNode().point());
    //    qDebug() << Q_FUNC_INFO << "reversed done:\n"
    //             << "Result start:" << ret.headNode().point() << "end:" << ret.tailNode().point() << "length" << ret.d->length
    //             << "\nSource start:" << headNode().point() << "end:" << tailNode().point() << "length" << d->length;
    //    ret.dumpNodes();

    return ret;
}

void GuidePath::init()
{
    if (!d)
        d = new GuidePathData;
    d->nodes.clear();
    d->nodes.append(HeadNode());
    d->nodes.prepend(TailNode());
    d->iHead = ++d->nodes.begin();
    d->iTail = d->iHead;
    d->length = 0;

}


void GuidePath::appendNode(const VectorNode &node)
{
    //    qDebug() << "Append node" << node.unitVector() << node.length();
    d->iTail = ++d->nodes.insert(d->iTail, node);
    d->length += node.length();
    d->iHead = ++d->nodes.begin();
    tailNode().pointRef() += node.vector();
    //    dumpNodes();
}

void GuidePath::prependNode(const VectorNode &node)
{
    d->iHead = d->nodes.insert(d->iHead, node);
    d->length += node.length();
    //    i_head = ++nodes.begin();
    //    qDebug() << "prepend, len =" << node.length << "length =" << m_length;
}

VectorNode GuidePath::takeLastVectorNode()
{
    VectorNode n = lastVNode();
    d->iTail = d->nodes.erase(--d->iTail);
    tailNode().pointRef() -= n.vector();
    d->length -= n.length();
    return n;
}

VectorNode GuidePath::takeFirstVectorNode()
{
    VectorNode n = firstVNode();
    headNode().pointRef() += n.vector();
    d->nodes.erase(d->iHead);
    d->iHead = ++d->nodes.begin();
    d->length -= firstVNode().length();
    return n;
    //    qDebug() << "take first, len =" << nodes.first().length << "length =" << m_length;
    //    return nodes.takeFirst();
}


VectorNode::VectorNode(const QLineF &l)
{
    QLineF uv = l.unitVector();
    setLength(l.length());
    setUnitVector(QPointF(uv.dx(), uv.dy()));
}

VectorNode::VectorNode(const QPointF &p1, const QPointF &p2)
    : VectorNode(QLineF(p1, p2))
{ }

float VectorNode::fastAngleTo(const VectorNode &pv) const
{
    QVector2D v1(pv.unitVector());
    QVector2D v2(unitVector());
    float a1 = norm_atan2(v1.y(), v1.x());
    float a2 = norm_atan2(v2.y(), v2.x());
    return fixNormAngle(a2 - a1) * 90;
}

//VChain VChain::buildOffset(qreal offset) const
//{
//    VChain result;
//    if (isEmpty())
//        return result;
//    VNode dv = firstVector();
//    QPointF startPt = headNode().point();
//    QPointF endPt;
//    QPointF prevPt = startPt + dv.normal() * offset;
//    //    result.headNode()->setPoint(prevPt);
//    result.appendPoint(prevPt);
//    bool inLoop;
////    VNode *loopEnterVector;
//    QLinkedList<VNode>::const_iterator i_loopEnter;
//    QPointF loopEnterPoint;
//    QLineF prevSegment;
//    QPointF prevVector;
//    QLineF::IntersectType iType;
//    int i = 0;
//    for (auto it = i_head; it != i_tail; ++it) {
////    while (!dv.isNull()) {
//        dv = *it;
//        QPointF currentVector = dv.vector();
//        endPt = startPt + currentVector;
//        QPointF offsetStartPt = startPt + dv.normal() * offset;
//        QPointF offsetEndPt = endPt + dv.normal() * offset;
//        QLineF seg(offsetStartPt, offsetEndPt);
//        if (!prevSegment.isNull()) {
//            QPointF intersectPoint;
//            iType = prevSegment.intersect(seg, &intersectPoint);
//            if (iType == QLineF::NoIntersection) {
//                result.appendPoint(prevPt);
//                continue;
//            }
//            QPointF ovec(intersectPoint - prevPt);
//            QPointF fvec(offsetEndPt - intersectPoint);
//            qreal dp = QPointF::dotProduct(ovec, prevVector);
//            QPointF resPt;
//            qreal fdp = dp;
//            if (dp < 0) {
//                resPt = offsetStartPt;
//            } else {
//                dp = QPointF::dotProduct(ovec, fvec);
//                if (dp < 0)
//                    resPt = offsetStartPt;
//                else
//                    resPt = intersectPoint;
//            }
//            QLineF currentSegment(prevPt, resPt);
//            VNode vec(currentSegment);
//            dp = QPointF::dotProduct(currentSegment.unitVector().p2() - prevPt, dv.unitVector());
//            if (dp < 0) {
////                qDebug() << "Entered self-intersection loop" << loopEnterPoint << result.size() << dp;
//                if (!inLoop) {
//                    i_loopEnter = result.i_tail;
//                    i_loopEnter--;
////                    result.lastVector();
//                    loopEnterPoint = prevPt;
//                }
//                inLoop = true;
//            } else {
//                if (inLoop) {
//                    auto v = i_loopEnter;
//                    --v;
////                    qDebug() << "in loop" << loopEnterPoint << result.size();
//                    QPointF bPt = loopEnterPoint;
//                    while (v->isVector()) {
//                        QLineF seg(bPt, bPt - v->vector());
//                        iType = currentSegment.intersect(seg, &intersectPoint);
//                        if (iType == QLineF::BoundedIntersection) {
////                            dumpNodes();
////                            result.dumpNodes();
//                            qDebug() << "before trim" << result.count() << result.length();
//                            result.trim(i_loopEnter);
////                            return result;
//                            qDebug() << "after trim" << result.count() << result.length();
////                            result.dumpNodes();
//                            result.appendPoint(intersectPoint);
////                            QLineF inSeg(seg.p2(), intersectPoint);
////                            result.
////                            qDebug() << "before length()"<< result.lastVector()->length() << inSeg.length();
////                            result.lastVector().setLength(inSeg.length());
////                            qDebug() << "after result.lastVector().length()"<< result.lastVector()->length() << inSeg.length();
//                            QLineF outSeg(intersectPoint, resPt);
//                            qDebug() << "\n>> complete\noutseg len:" << outSeg.length()
//                                     << "\noutseg:" << outSeg
//                                     << "\nvec.len:" <<  vec.length()
//                                     << "\nfdp:" << fdp
//                                     << "\nresPt:" << resPt
//                                     << "\noffsetStartPt:" << offsetStartPt
////                                     << "\nintersectPoint:" << intersectPoint
//                                     << "\nstartPt:" << startPt
//                                     << "\ndv.length:" << dv.length();
////                            dumpNodes();

//                            vec.setLength(outSeg.length());
////                            result.appendPoint(resPt);
////                            result.appendPoint(intersectPoint);
////                            result.appendNode(vec);
//                            inLoop = false;
//                            break;
//                        }
//                        bPt -= v.i->t.vector();
//                        --v;
//                    }
//                }
//            }

//            result.appendNode(vec);
////            result.appendPoint(resPt);
//            prevPt = resPt;
//        }
//        ++i;
//        prevVector = currentVector;
//        prevSegment = seg;
//        startPt = endPt;
//    }
//    dv = lastVector();
//    result.appendNode(VNode(prevPt, endPt + dv.normal() * offset));
//    return result;
//}

//VChain VChain::getPart(qreal startPos, qreal endPos) const
//{
//    // TODO: make it more clever (do not reconstruct nodes, copy them instead)
//    VChain ret;
//    VRunner r = setRunner(startPos);
//    if (endPos > startPos) {
//        while (r.position() < endPos && !r.atEnd()) {
//            ret.appendPoint(r.point(), r.data());
//            r.toNextNode(endPos - r.position());
//        }
//    } else {
//        while (r.position() > endPos && !r.atStart()) {
//            ret.appendPoint(r.point(), r.data());
//            r.toPrevNode(r.position() - endPos);
//        }
//    }
//    ret.appendPoint(r.point());
//    return ret;
//}

//VectorPath VectorPath::split(qreal pos)
//{
//    qreal len = 0;
//    VectorPath rest;
//    int i;
//    QList<VNode>::iterator it = nodes.begin();
//    while (it != nodes.end()) {
//        if ((len + it->length) >= pos)
//            break;
//        len += it->length;
//        ++it;
//    }
//    QPointF stp = it->pointAtLength(pos - len);
//    ++it;
//    rest.addPoint(stp);
//    QList<VNode>::iterator it2 = it;
//    //    --it2;
//    while (it2 != nodes.end()) {
//        rest.addPoint(it2->point);
//        ++it2;
//    }
//    rest.addPoint(lastNode.point);
//    lastNode.point = QPointF();
//    nodes.erase(it, nodes.end());
//    addPoint(stp);
//    nodes.last().next = &rest.nodes.first();
//    rest.nodes.first().prev = &nodes.last();
//    //    if (nodes.last().next) {
//    //        nodes.last().next->prev = nullptr;
//    //        nodes.last().next = nullptr;
//    //    }
//    m_length = pos;
//    return rest;
//}

//VectorPath VectorPath::reversed() const
//{
//    VectorPath ret;
//    for (const VNode &node : nodes) {
//        VNode n;
//        n->length = node.length;
//        n->point = node.endPoint();
//        n->unitVector = -node.unitVector;
//        n->prev = node.next;
//        n->next = node.prev;
//        n->data = node.data;
//        ret.prependNode(n);
//    }
//    return ret;
//}

//Runner VectorPath::split(qreal pos, VectorPath *left, VectorPath *right) const
//{
//    Runner ret = setRunner(pos);
//    VectorPath *p = left;
//    for (const VNode &n:nodes) {
//        VNode *prev = nullptr;
//        if (!p->nodes.isEmpty())
//            prev = &p->nodes.last();
//        p->nodes.append(n);
//        p->nodes.last().prev = prev;
//        if (prev) {
//            prev->next = &p->nodes.last();
//        }
//        if (&n == ret.i->t) {
//            p->nodes.last().length = ret.lengthBehind();
//            p->nodes.last().next = nullptr;
//            p->lastNode.point = ret.point();
//            p = right;
//            p->addPoint(ret.point(), n->data);
//            p->addPoint(ret.i->t.endPoint(), n->data);
//        }
//    }
//    p->lastNode = lastNode;
//    left->m_length = pos;
//    right->m_length = m_length - pos;
//    return ret;
//}

//VChain VChain::buildRounding(qreal radius, qreal step, qreal startPos, qreal endPos) const
//{
//    VChain result;
//    if (isEmpty())
//        return result;
//    VRunner r = setRunner(startPos);
//    qreal k;
//    bool expectExit = false;
//    qreal behind = r.straightLengthBehind(radius / 2, &k);
//    if (behind < 0) {
//        expectExit = true;
//    }
//    qreal rd = radius * k;
//    qreal fwd;
//    QPointF sp = r.getSmoothedPoint(radius);
//    result.appendPoint(sp, r.position());
//    qreal dist = endPos - startPos;
//    while (dist > 0 && !r.atEnd()) {
//        if (!expectExit) {
//            fwd = r.straightLengthAhead(radius / 2, &k);
//            rd = radius * k;
//            if (fwd > 0) {
//                if (k < 0.01) {
//                    fwd = r.lengthAhead();
//                    dist -= qMin(dist, fwd);
//                    if (fwd > dist) {
//                        r.moveBy(dist);
//                        result.appendPoint(r.getSmoothedPoint(rd), r.position());
//                        dist = 0;
//                        break;
//                    } else {
//                        r.moveBy(fwd);
//                        result.appendPoint(r.point(), r.position());
//                        continue;
//                    }
//                }
//                if (fwd > dist) {
//                    r.moveBy(dist);
//                    dist = 0;
//                    result.appendPoint(r.getSmoothedPoint(rd), r.position());
//                    break;
//                }
//                dist -= r.moveBy(fwd);
//            }
//        }
//        while (true) {
//            result.appendPoint(r.getSmoothedPoint(rd), r.position());
//            if (r.lengthAhead() < step)
//                expectExit = true;
//            dist -= r.moveBy(qMin(step, dist));
//            if (dist <= 1E-9) {
//                dist = 0;
//                break;
//            }
//            if (r.atEnd()) {
//                break;
//            }
//            if(expectExit) {
//                qreal ahead = r.lengthAhead();
//                qreal behind = r.lengthBehind();
//                if (behind > rd / 2 && ahead > radius / 2) {
//                    expectExit = false;
//                    break;
//                }
//            }
//        }
//        result.appendPoint(r.getSmoothedPoint(rd), r.position());
//    }
////    if (r.atEnd() && dist > 1)
////        qDebug() << "remaining dist" << dist;
//    return result;
//}

//VChain VChain::buildOffset(qreal offset) const
//{
//    QPolygonF raw;
//    VChain result;
//    if (nodes.isEmpty())
//        return result;
//    const VNode *node = firstVector();
////    raw.append(node->offsetPoint(o));
//    raw.append(headNode()->point());
//    QLineF prev_seg;
//    QPointF prev_vec;
//    QLineF::IntersectType iType;
////    int i = 0;
//    QPointF spt, ept;
//    spt = raw.last();
//    for (auto it = i_head; it != i_tail; ++it) {
//        const VNode *wn1 = *it;
//        ept = spt + wn1->vector();
//        QPointF cvec = wn1->vector();
//        QPointF p1_1 = spt + (wn1->normal() * offset);
//        QPointF p2_1 = ept + (wn1->normal() * offset);;
//        QLineF seg(p1_1, p2_1);
//        if (!prev_seg.isNull()) {
//            QPointF iPoint;
//            iType = prev_seg.intersect(seg, &iPoint);
//            if (iType == QLineF::NoIntersection) {
//                raw.append(p1_1);
//                continue;
//            }
//            QPointF ovec(iPoint - raw.last());
//            QPointF fvec(p2_1 - iPoint);
//            qreal dp = QPointF::dotProduct(ovec, prev_vec);
//            if (dp < 0) {
//                raw.append(p1_1);
//            } else {
//                dp = QPointF::dotProduct(ovec, fvec);
//                if (dp < 0) {
//                    raw.append(p1_1);
//                } else {
//                    raw.append(iPoint);
//                }
//            }
//        }
//        prev_vec = cvec;
//        prev_seg = seg;
//    }

//    node = lastVector();
//    raw.append(tailNode()->point() + node->normal() * offset);
//    result.appendPoint(raw.first());
//    bool inLoop;
//    QLinkedList<VNode*>::const_iterator i_loopEnter = ++nodes.begin();
//    QLinkedList<VNode*>::const_iterator i_likelyExit = ++nodes.begin();
//    int loopEnterIdx = 0;
//    QPointF loopEnterPoint;
//    int loopExitCandidate = 0;
//    do {
//        inLoop = false;
////        for (auto it = i_loopEnter; )
//        auto i_src = i_loopEnter;
//        for (int i = loopEnterIdx; i < raw.size() - 1; ++i, ++i_src) {
//            const QPointF &p1 = raw[i];
//            const QPointF &p2 = raw[i + 1];
//            const VNode *cNode = *i_src;
////            const VNode *cNode = nodes[i];
//            QLineF cseg = QLineF(p1, p2);
//            QVector2D sVec(cNode->unitVector());
//            QVector2D cVec(cseg.dx(), cseg.dy());
//            float dp = QVector2D::dotProduct(cVec.normalized(), sVec);
//            if (dp < 0) {
//                if (!inLoop)
//                    loopEnterIdx = i;
//                    loopEnterPoint = p2;
//                    i_loopEnter = i_src;
//                inLoop = true;
//            } else {
//                if (inLoop) {
//                    Q_ASSERT(result.count() > 0);
//                    auto i_lookBehind = result.i_tail;
//                    int j = result.count() - 1;
//                    while (i_lookBehind != result.nodes.begin()) {
////                    while (j >= 0) {
//                        QLineF seg = (*i_lookBehind)->segment(loopEnterPoint);
////                        segment(j);
//                        QPointF iPoint;
//                        iType = cseg.intersect(seg, &iPoint);
//                        if (iType == QLineF::BoundedIntersection) {
//                            result.trim(i_lookBehind);
//                            result.appendPoint(iPoint);
//                            result.appendPoint(p2);
//                            inLoop = false;
//                            loopExitCandidate = 0;
//                            break;
//                        }
//                        if (loopExitCandidate == 0 && dp == 1.0) {
//                            loopExitCandidate = i;
//                            //                            qDebug() << "Exit candidate for loop from" << loopEnterIdx << loopExitCandidate << dp;
//                        }
//                        --j;
//                        --i_lookBehind;
//                    }
//                } else {
//                    result.appendPoint(p2);
//                }
//            }
//        }
//        if (inLoop) {
//            qDebug() << "in loop from node" << loopEnterIdx++;
//            result.appendPoint(raw.at(loopEnterIdx));
//            result.appendPoint(raw.at(loopExitCandidate));
//            if (loopExitCandidate != 0)
//                loopEnterIdx = loopExitCandidate;
//            else
//                ++loopEnterIdx;
//            if (loopEnterIdx >= raw.size() - 1) {
//                result.appendPoint(raw.last());
//                break;
//            }
//            loopExitCandidate = 0;
//            continue;
//        }
//    } while (inLoop);
//    qDebug() << "build offset size" << result.size() << "from source" << nodes.size();
//    return result;
//}



//const VNode *VectorPath::nodeAt(int index) const
//{
//    if (nodes.size() < index)
//        return &nodes.at(index);
//    return nullptr;
//}

//QPointF VChain::pointAtLength(qreal pos, int *legIndex) const
//{
//    if (isEmpty())
//        return QPointF();
//    if (pos < 0) {
//        if (legIndex)
//            *legIndex = 0;
//        return headNode()->point();
//    }
//    int i = 0;
//    const VNode *n;
//    auto it = i_head;
//    while (it != i_tail) {
//        n = *it;
//        if (pos <= n->length)
//            break;
//        pos -= n->length;
//        ++i;
//        ++it;
//    }
////    const VNode *n = *it;
//    if (legIndex)
//        *legIndex = i;
//    if (pos > n->length) {
//        return n->endPoint();
//    }
//    return n->pointAtLength(pos);
//}

//void VectorPath::resize(int n)
//{
//    if (nodes.last()->next)
//        nodes.last()->next->prev = nullptr;
//    while (nodes.size() > n && !nodes.isEmpty()) {
//        nodes.removeLast();
//    }
//    if (!nodes.isEmpty()) {
//        nodes.last()->next = nullptr;
//    }
//    lastNode.prev = nullptr;
//    lastNode.point = QPointF();
//}


QDataStream &operator<<(QDataStream &out, const GuidePath &p)
{
    qDebug() << "exporting pathguide size" << p.d->nodes.size() << "length" << p.d->length;
    out << p.d->pathData;
    out << p.d->length;
    out << p.d->nodes;
    //    for (PathIterator it = p.begin(); it != p.end(); ++it) {
    //        out << *it;
    //    }
    return out;
}


QDataStream &operator>>(QDataStream &in, GuidePath &p)
{
    p.d->nodes.clear();
    in >> p.d->pathData;
    in >> p.d->length;
    in >> p.d->nodes;
    //    int size;
    //    in >> size;
    //    in >> p.d->length;
    //    in >> p.d->pathData;
    qDebug() << "imported pathguide size" << p.d->nodes.size() << "length" << p.d->length;
    p.d->iHead = p.d->nodes.begin() + 1;
    p.d->iTail = p.d->nodes.end() - 2;
    //    p.dumpNodes();
    //    while (size--) {
    //        PathNode node;
    //        in >> node;
    //        p.d->nodes.append(node);
    //        qDebug() << "Added node type" << node.type();
    //    }
    return in;

}


float norm_atan2(float y, float x)
{
    static const uint32_t sign_mask = 0x80000000;
    static const float b = 0.596227f;

    // Extract the sign bits
    uint32_t ux_s  = sign_mask & (uint32_t &)x;
    uint32_t uy_s  = sign_mask & (uint32_t &)y;

    // Determine the quadrant offset
    float q = (float)( ( ~ux_s & uy_s ) >> 29 | ux_s >> 30 );

    // Calculate the arctangent in the first quadrant
    float bxy_a = ::fabs( b * x * y );
    float num = bxy_a + y * y;
    float atan_1q =  num / ( x * x + bxy_a + num );

    // Translate it to the proper quadrant
    uint32_t uatan_2q = (ux_s ^ uy_s) | (uint32_t &)atan_1q;
    return q + (float &)uatan_2q;
}


float fixNormAngle(float angle)
{
    qreal m = fmod(angle, 4);
    if (m < 0)
        m += 4;
    if (m > 2)
        m -= 4;
    return m;
}


GuidePath::Wavefront::node::node(const VectorNode *prevVectorNode, const VectorNode *vectorNode)
{
    qreal dot = QPointF::dotProduct(prevVectorNode->unitVector(), -vectorNode->unitVector());
    qreal bs_proj = QPointF::dotProduct(prevVectorNode->unitVector(), vectorNode->normal());
    qreal bsFactor = invSqrt((1 - dot) / 2) * sgn(bs_proj);
    bsuv = bsFactor * QVector2D(prevVectorNode->unitVector() - vectorNode->unitVector()).normalized();
    vnode = *vectorNode;
}

<<<<<<< .mine
float GuidePath::Wavefront::node::maxOffset() const
=======




GuidePath::Wavefront::Wavefront(const GuidePath &path)
>>>>>>> .r2725
{
<<<<<<< .mine
    return vnode.length() / (leftDecay + rightDecay);
=======
    if (path.isEmpty())
        return;
    m_sourcePoint = path.headPoint();
    for (PathIterator i_vec = path.first(); i_vec != path.tail(); ++i_vec) {
        PathIterator i_prevVec = i_vec - 1;
        if (!i_prevVec->isVector()) {
            // head node
            //            qDebug() << "at head node" << i_prevVec.pnode()->point();
            Wavefront::node f;
            f.vnode = path.firstVNode();
            f.bsuv = QVector2D(f.vnode.normal());
            nodes.append(f);
            continue;
        }
        //            break;
        Wavefront::node nd(i_prevVec.vnode(), i_vec.vnode());
        //        qDebug() << "created node" << nodes.size() << nd.bsuv;
        nodes.last().setDecayRate(nd);
        float offs = nodes.last().maxOffset();
//        nodes.append(nd);
        auto i = nodes.insert(nodes.end(), nd);
        offsetMap.insert(offs, i.i);
    }
//    if (offsetMap.isEmpty())
//        return;

    //    qDebug() << __func__ << "minPosLength" << offsetMap.keys() << "maxOffset" << maxPositiveOffset();
>>>>>>> .r2725
}
<<<<<<< .mine

void GuidePath::Wavefront::node::setDecayRate(const GuidePath::Wavefront::node &next)
{
    leftDecay = QVector2D::dotProduct(bsuv, QVector2D(vnode.unitVector()));
    rightDecay = QVector2D::dotProduct(next.bsuv, -QVector2D(vnode.unitVector()));
}

GuidePath::Wavefront::Wavefront(const GuidePath &path)
{
    if (path.isEmpty())
        return;
    m_sourcePoint = path.headPoint();
    for (PathIterator i_vec = path.first(); i_vec != path.tail(); ++i_vec) {
        PathIterator i_prevVec = i_vec - 1;
        if (!i_prevVec->isVector()) {
            // head node
            //            qDebug() << "at head node" << i_prevVec.pnode()->point();
            Wavefront::node f;
            f.vnode = path.firstVNode();
            f.bsuv = QVector2D(f.vnode.normal());
            nodes.append(f);
            continue;
        }
        //            break;
        Wavefront::node nd(i_prevVec.vnode(), i_vec.vnode());
        //        qDebug() << "created node" << nodes.size() << nd.bsuv;
        nodes.last().setDecayRate(nd);
        float offs = nodes.last().maxOffset();
//        nodes.append(nd);
        auto i = nodes.insert(nodes.end(), nd);
        offsetMap.insert(offs, i.i);
    }
//    if (offsetMap.isEmpty())
//        return;

    //    qDebug() << __func__ << "minPosLength" << offsetMap.keys() << "maxOffset" << maxPositiveOffset();
}

GuidePath GuidePath::Wavefront::getOffset(qreal offset)
{
    doOffset(offset);
    GuidePath ret;
    ret.appendPoint(m_sourcePoint + nodes.first().vnode.normal() * offset);

    for (node n : nodes) {
        ret.appendNode(n.vnode);
    }
    return ret;

}

void GuidePath::Wavefront::doOffset(qreal offset)
{
    if (offset < 0) {
        qWarning() << "implement negative offset"; // FIXME: implement it
        return;
    }

    float currentOffset = 0;
    while (offset > 0) {
        auto i_currentMax = offsetMap.upperBound(currentOffset);
        float currentMax = i_currentMax.key();
        if (currentMax < currentOffset + offset) {
            offset -= currentMax;
            currentOffset += currentMax;
            QLinkedList<Wavefront::node>::iterator rem(i_currentMax.value());
            rem = nodes.erase(rem);
            if (rem == nodes.end()) {
                qDebug() << "Removed the last node";
                nodes.last().rightDecay = 0;
            } else if (rem == nodes.begin()) {
                qDebug() << "removed first vnode, next" << rem->vnode.length();
//                qreal ldecay = QVector2D::dotProduct(bsuv, QVector2D(vnode.unitVector()));
//                qreal rdecay = QVector2D::dotProduct(next.bsuv, -QVector2D(vnode.unitVector()));
//                decayRate = ldecay + rdecay;
                rem->leftDecay = 0;
                rem->bsuv = QVector2D(rem->vnode.normal());
            } else {
                auto prev_rem = rem--;


                qDebug() << "removed vnode, next" << rem->vnode.length() << "prev" << prev_rem->vnode.length();
            }
        }
        for (QLinkedList<Wavefront::node>::iterator ii = nodes.begin(); ii != nodes.end(); ++ii) {
            ii->vnode.l -= currentOffset * ii->leftDecay;
        }
        offsetMap.erase(i_currentMax);
    }


}

=======

GuidePath GuidePath::Wavefront::getOffset(qreal offset)
{
    doOffset(offset);
    GuidePath ret;
    ret.appendPoint(m_sourcePoint + nodes.first().vnode.normal() * offset);

    for (node n : nodes) {
        ret.appendNode(n.vnode);
    }
    return ret;

}

void GuidePath::Wavefront::doOffset(qreal offset)
{
    if (offset < 0) {
        qWarning() << "implement negative offset"; // FIXME: implement it
        return;
    }

    float currentOffset = 0;
    while (offset > 0) {
        auto i_currentMax = offsetMap.upperBound(currentOffset);
        float currentMax = i_currentMax.key();
        if (currentMax < currentOffset + offset) {
            offset -= currentMax;
            currentOffset += currentMax;
            QLinkedList<Wavefront::node>::iterator rem(i_currentMax.value());
            rem = nodes.erase(rem);
            if (rem == nodes.end()) {
                qDebug() << "Removed the last node";
                nodes.last().rightDecay = 0;
            } else if (rem == nodes.begin()) {
                qDebug() << "removed first vnode, next" << rem->vnode.length();
//                qreal ldecay = QVector2D::dotProduct(bsuv, QVector2D(vnode.unitVector()));
//                qreal rdecay = QVector2D::dotProduct(next.bsuv, -QVector2D(vnode.unitVector()));
//                decayRate = ldecay + rdecay;
                rem->leftDecay = 0;
                rem->bsuv = QVector2D(rem->vnode.normal());
            } else {
                auto prev_rem = rem--;


                qDebug() << "removed vnode, next" << rem->vnode.length() << "prev" << prev_rem->vnode.length();
            }
        } else {

        }
        for (QLinkedList<Wavefront::node>::iterator ii = nodes.begin(); ii != nodes.end(); ++ii) {
            ii->vnode.l -= currentOffset * ii->leftDecay;
        }
        offsetMap.erase(i_currentMax);
    }


}

float invSqrt(float x){
    float xhalf = 0.5f * x;
    int i = *(int*)&x;          // store floating-point bits in integer
    i = 0x5f3759d5 - (i >> 1);  // initial guess for Newton's method
    x = *(float*)&i;            // convert new bits into float
    x = x*(1.5f - xhalf*x*x);   // One round of Newton's method
    return x;
}
>>>>>>> .r2725
