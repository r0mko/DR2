#include "pathrunner.h"
#include "vchain.h"
#include <QDebug>
#include <QtMath>

GuidePath::GuidePath()
{
    init();
}

GuidePath::~GuidePath()
{
}

void GuidePath::trim(const QLinkedList<VectorNode>::const_iterator &after)
{
    auto i_lv = --QLinkedList<VectorNode>::iterator(i_tail);
    while (i_lv != after) {
        VectorNode lv = *i_lv;
        if (!lv.isVector()) {
            qDebug() << "cannot erase node" << lv.type();
            return;
        }
        tailNode().pointRef() -= lv.vector();
        m_length -= lv.length();
        i_lv = --nodes.erase(i_lv);
    }
}


GuidePath GuidePath::buildOffset(qreal offset) const
{
    GuidePath result;
    if (isEmpty())
        return result;

    QPointF sourcePt = headNode().point();
    QPointF offsetPt = sourcePt + firstVectorNode().normal() * offset;
    result.appendPoint(offsetPt);
    int i = 0;
    QPointF lastValidPoint;
    auto i_lastValidNode = head();
    bool inCusp = false;
    for (auto i_prevVec = head(); i_prevVec != tail(); ++i_prevVec) {
        ++i;
        auto i_nextVec = i_prevVec + 1;
        if (!i_nextVec->isVector())
            break;
        qreal dot = QPointF::dotProduct(i_prevVec->unitVector(), -i_nextVec->unitVector());
        QPointF nextnorm = i_nextVec->unitVector();
        qSwap(nextnorm.rx(), nextnorm.ry() *= -1);
        qreal bs = QPointF::dotProduct(i_prevVec->unitVector(), nextnorm);
        qreal bisectorLength = offset / qSqrt((1 - dot) / 2)  * sgn(bs);
        QVector2D bisectUnit = QVector2D(i_prevVec->unitVector() - i_nextVec->unitVector()).normalized();
        sourcePt += i_prevVec->vector();
        offsetPt = sourcePt + (bisectUnit * bisectorLength).toPointF();
        result.appendPoint(offsetPt);
        // fix self-intersection
        qreal dot_intersect = QPointF::dotProduct(result.lastVectorNode().unitVector(), i_prevVec->unitVector());
        if (dot_intersect < 0)  {
            if (!inCusp) {
//                qDebug() << "Self-intersect at" << i << dot_intersect << result.lastVectorNode().length() << i_nextVec->length() << result.lastVectorNode().length();
                inCusp = true;
                lastValidPoint = result.tailNode().point();
                i_lastValidNode = result.last();
            }
        } else {
            if (inCusp) {
                QLineF likelyValidSegment(offsetPt, offsetPt - result.lastVectorNode().vector());
//                qDebug() << "Possible leave cusp at" << i << dot_intersect << result.lastVectorNode().length() << likelyValidSegment.length() << i_lastValidNode->length();
                auto it_back = i_lastValidNode;
                QPointF endPoint = lastValidPoint;
                while (it_back != result.head()) {
                    QLineF segment(endPoint - it_back->vector(), endPoint);
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
                    endPoint -= it_back->vector();
                    it_back--;
                }
            }
        }
    }
    sourcePt = tailNode().point();
    offsetPt = sourcePt + lastVectorNode().normal() * offset;
    if (inCusp) {
        result.trim(i_lastValidNode);
    }
    result.appendPoint(offsetPt);
    return result;
}


void GuidePath::appendPoint(const QPointF &point)
{
    if (isEmpty() && headNode().point().isNull()) {
        headNode().setPoint(point);
        return;
    }
    QVector2D v;
    if (isEmpty()) {
        v = QVector2D(point - headNode().point());
    } else {
        v = QVector2D(point - tailNode().point());
    }
    VectorNode node;
    node.setUnitVector(v.normalized().toPointF());
    node.setLength(v.length());
    m_length += node.length();
    tailNode().setPoint(point);
    i_tail = ++nodes.insert(i_tail, node);
    i_head = ++nodes.begin();
}


void GuidePath::addPolygon(const QPolygonF &polygon, QVariant nodeData)
{
    if(polygon.size() < 2) {
        return;
    }
    clear();
    for (int i = 1; i < polygon.size(); ++i) {
        const QPointF &p1 = polygon[i-1];
        const QPointF &p2 = polygon[i];
        VectorNode n(p1, p2);
        if (n.length() != 0)
            appendNode(n);
    }
    headNode().pointRef() = polygon.first();
    tailNode().pointRef() = polygon.last();
}

QPolygonF GuidePath::toPolygon() const
{
    QPolygonF ret;
    QPointF stPoint = headNode().point();
    ret.append(stPoint);
    for (auto it = i_head; it != i_tail; ++it) {
        stPoint += (*it).vector();

        ret.append(stPoint);
    }
    //    ret.append(lastNode.point);
    return ret;
}

//qreal GuidePath::findNearestPos(const QPointF &pos)
//{
//    GuideRunner r(nodes.begin());
//    //    r.setOriginPoint(origin);
//    //    r._toBegin();
//    qreal mindst = 1e127;
//    QVector2D pv(pos);
//    qreal result;
//    while (!r.atEnd()) {
//        qreal dst = pv.distanceToLine(QVector2D(r.nodePoint), QVector2D(r.unitVector()));
//        if (dst < mindst) {
//            QPointF endP = (r.normal() * (dst + 1));
//            QLineF l_perpendicular(pos - endP, pos + endP);
//            QPointF iPoint;
//            QLineF::IntersectType sect = r.segment().intersect(l_perpendicular, &iPoint);
//            if (sect == QLineF::BoundedIntersection) {
//                mindst = dst;
//                //                point = iPoint;
//                qreal seg_pos = QLineF(r.nodePoint, iPoint).length();
//                result = r.position() + seg_pos;
//            }
//        }
//        qreal stlen = QLineF(pos, r.nodePoint).length();
//        if (stlen < mindst) {
//            mindst = stlen;
//            result = r.position();
//            //            point = r.nodePoint;
//        }
//        r.hopForward();
//    }
//    qreal stlen = QLineF(pos, r.point()).length();
//    if (stlen < mindst) {
//        mindst = stlen;
//        result = r.position();
//        //            point = r.nodePoint;
//    }
//    if (distance)
//        *distance = mindst;
//    return result;

//}

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
    nodes.clear();
    init();
    tailNode().setPoint(QPointF());

}

void GuidePath::dumpNodes() const
{
    QDebug d = qDebug().nospace();
    for (const VectorNode &n : nodes) {
        d << "\n[ " << (VectorNode::NodeType)n.type();
        if (n.isVector())
            d << " dx: " << n.unitVector().x() << " dy: " << n.unitVector().y() << " len: " << n.length();
        else {
            VertexNode p = *static_cast<const VertexNode*>(&n);
            d << " point:"<<  p.point();
        }
        d << " ]";
        if(n.isVector())
            d << "->";
    }
    d << " length: " << length() << " count: " << count();

    //    int i = 1;
    //    qreal l = 0;
    //    for (const VNode *n:nodes) {
    //        if (n->prev) {
    //            if (n->isConnectedTo(n->prev))
    //                d << "->";
    //            else
    //                d << "»|";
    //        } else
    //            d << "  ";
    //        d << " ( l=" << QString::number(n->length, 'f', 1).toLocal8Bit().data() << " ) ";
    //        if (n->next) {
    //            if (n->isConnectedTo(n->next))
    //                d << "<-";
    //            else
    //                d << "|«";
    //        } else
    //            d << "  ";
    //        l += n->length;
    //    }
    //    d << " ]\t\t sum:" << l << " len:" << m_length;
}

qreal GuidePath::checkLength()
{
    qreal l = 0;
    for (auto it = i_head; it != i_tail; ++it) {
        l += it->length();
    }
    qreal d = m_length - l;
    if (m_length != l) {
        if (qAbs(d) > 1)
            qDebug() << "length mismatch, delta =" << d;
        m_length = l;
    }
    return d;
}

VectorNode &GuidePath::firstVectorNode()
{
    Q_ASSERT(nodes.size() > 2);
    return *i_head;
}

VectorNode &GuidePath::lastVectorNode()
{
    Q_ASSERT(nodes.size() > 2);
    return i_tail.i->p->t;
}

const VectorNode &GuidePath::firstVectorNode() const
{
    Q_ASSERT(nodes.size() > 2);
    return *i_head;
}

const VectorNode &GuidePath::lastVectorNode() const
{
    Q_ASSERT(nodes.size() > 2);
    return i_tail.i->p->t;
}


QPointF GuidePath::weightedAverage(qreal range, const QPointF &origin)
{
    QPointF ret;
    QPointF pp = origin;
    if (isEmpty())
        return ret;
    for (auto it = i_head; it != i_tail; ++it) {
        VectorNode v = *it;
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


GuidePath GuidePath::rounded(qreal radius, qreal precision) const
{
    GuidePath ret;
    if (isEmpty())
        return ret;
    qreal curveFwd, curveBack;

//    ret.appendPoint(headNode().point());
    GuideRunner vertexRunner(i_head);
    GuideRunner smoothRunner(i_head);
    vertexRunner.setPoint(headNode().point());
    smoothRunner.setPoint(headNode().point());
    ret.appendPoint(vertexRunner.point());

    vertexRunner.hopForward();
    bool hasPoint = true;
    int c = 0;
    while (!vertexRunner.atEnd()) {
        //        qDebug() << "rounding iteration" << c++ << vr.position() << vr.currentNodeType() << pr.position() << pr.currentNodeType();
        qreal turn = vertexRunner.turnAngle();
        if (turn < 0)
            turn *= -0.3; //
        if (turn > 0.3) {
//            qreal curvature;
            qreal curvature;
            qreal step = radius / (qSqrt(turn) * precision);
            qreal negLimit = vertexRunner.position() - radius / 2;
            qreal posLimit = vertexRunner.position() + radius / 2;
            //                        qreal negLimit = vr.position() - qMin(vr.lengthBehind(), radius / 2);
            //                        qreal posLimit = vr.position() + qMin(vr.lengthAhead(), radius / 2);
            //                        qreal delta = negLimit - pr.position();
            if (negLimit > smoothRunner.position()) {
                smoothRunner.moveForward(negLimit - smoothRunner.position());
                hasPoint = false;
            }
            if (smoothRunner.position() + step > posLimit) {
                vertexRunner.hopForward();
                continue;
            }
            while (!smoothRunner.atEnd() && smoothRunner.position() < posLimit) {
                if (!hasPoint) {
                    QPointF pt = smoothRunner.point() + smoothRunner.roundingDelta(radius, &curveFwd, &curveBack);
                    curvature = qMax(qAbs(curveFwd), qAbs(curveBack));
//                    qDebug() << "at pos" << pr.position() << "curvature" << curvature;
                    if (curvature > 1E-10) {
                        ret.appendPoint(pt);
                        hasPoint = true;
                    } else if (vertexRunner.position() < smoothRunner.position()) {
//                        //                        qDebug() << "nothing to chamfer at pos" << pr.position() << vr.position() << pr.currentNodeType() << vr.currentNodeType();
                        break; // go to next vertex if current rounding is complete
                    }
                }
                if (!smoothRunner.atEnd() && (smoothRunner.position() + step) <= posLimit) {
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
    ret.appendPoint(vertexRunner.point());
    return ret;
}

//QPolygonF GuidePath::roundedPolygon(qreal radius, qreal precision) const
//{
//    QPolygonF ret;
//    if (isEmpty())
//        return ret;
//    qreal curveFwd, curveBack;
//    ret.append(headNode().point());
//    GuideRunner vr(i_head);
//    GuideRunner pr(i_head);
//    vr.setPoint(headNode().point());
//    pr.setPoint(headNode().point());

//    vr.hopForward();
//    bool hasPoint = true;
//    //    qDebug() << "Started rounding" << vr.position() << vr.currentNodeType() << pr.position() << pr.currentNodeType() << pr.point() << vr.point();
//    //    dumpNodes();
//    int c = 0;
//    while (!vr.atEnd()) {
//        //        qDebug() << "rounding iteration" << c++ << vr.position() << vr.currentNodeType() << pr.position() << pr.currentNodeType();
//        qreal turn = vr.turnAngle();
//        if (turn < 0)
//            turn *= -0.3;
//        if (turn > 1) {
//            qreal curvature;
//            qreal step = radius / (qSqrt(turn) * precision);
//            qreal negLimit = vr.position() - radius / 2;
//            qreal posLimit = vr.position() + radius / 2;
//            //                        qreal negLimit = vr.position() - qMin(vr.lengthBehind(), radius / 2);
//            //                        qreal posLimit = vr.position() + qMin(vr.lengthAhead(), radius / 2);
//            //                        qreal delta = negLimit - pr.position();
//            if (negLimit > pr.position()) {
//                pr.moveForward(negLimit - pr.position());
//                hasPoint = false;
//            }
//            if (pr.position() + step > posLimit) {
//                vr.hopForward();
//                continue;
//            }
//            while (!pr.atEnd() && pr.position() < posLimit) {
//                if (!hasPoint) {
//                    QPointF pt = pr.point() + pr.roundingDelta(radius, &curveFwd, &curveBack);
//                    curvature = qMax(qAbs(curveFwd), qAbs(curveBack));
//                    if (curvature > 1E-10) {
//                        ret.append(pt);
//                        hasPoint = true;
//                    } else if (vr.position() < pr.position()) {
//                        //                        qDebug() << "nothing to chamfer at pos" << pr.position() << vr.position() << pr.currentNodeType() << vr.currentNodeType();
//                        break; // go to next vertex if current rounding is complete
//                    }
//                }
//                if (!pr.atEnd() && (pr.position() + step) <= posLimit) {
//                    pr.moveForward(step);
//                    hasPoint = false;
//                } else {
//                    break;
//                }
//            }
//        }
//        vr.hopForward();
//    }
//    //    qDebug() << vr.point() << pr.point() << vr.currentNodeType() << pr.currentNodeType();
//    ret.append(vr.point());
//    return ret;
//}

void GuidePath::init()
{
    nodes.append(FinishNode());
    nodes.prepend(StartNode());
    i_head = ++nodes.begin();
    i_tail = i_head;
    m_length = 0;
}


void GuidePath::appendNode(const VectorNode &node)
{
    //    qDebug() << "Append node" << node.unitVector() << node.length();
    i_tail = ++nodes.insert(i_tail, node);
    m_length += node.length();
    i_head = ++nodes.begin();
    tailNode().pointRef() += node.vector();
    //    dumpNodes();
}

void GuidePath::prependNode(const VectorNode &node)
{
    i_head = nodes.insert(i_head, node);
    m_length += node.length();
    //    i_head = ++nodes.begin();
    //    qDebug() << "prepend, len =" << node.length << "length =" << m_length;
}

VectorNode GuidePath::takeLastNode()
{
    VectorNode n = i_tail.i->p->t;
    i_tail = nodes.erase(--i_tail);
    m_length -= n.length();
    return n;
}

VectorNode GuidePath::takeFirstNode()
{
    m_length -= firstVectorNode().length();
    VectorNode n = firstVectorNode();
    nodes.erase(i_head);
    i_head = ++nodes.begin();
    return n;
    //    qDebug() << "take first, len =" << nodes.first().length << "length =" << m_length;
    //    return nodes.takeFirst();
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
