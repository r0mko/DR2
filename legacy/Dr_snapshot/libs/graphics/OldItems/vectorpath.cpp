#include "vectorpath.h"
#include <QLineF>
#include <QDebug>

VectorPath::VectorPath() :
    m_length(0)
{

}

VectorPath::~VectorPath()
{
}

VectorPath VectorPath::getPart(qreal startPos, qreal endPos) const
{
    // TODO: make it more clever (do not reconstruct nodes, copy them instead)
    VectorPath ret;
    if(nodes.isEmpty())
        return ret;
    Runner r = setRunner(startPos);
    if (endPos > startPos) {
        while (r.position() < endPos && !r.atEnd()) {
            ret.addPoint(r.point(), r.data());
            r.toNextNode(endPos - r.position());
        }
    } else {
        while (r.position() > endPos && !r.atStart()) {
            ret.addPoint(r.point(), r.data());
            r.toPrevNode(r.position() - endPos);
        }
    }
    ret.addPoint(r.point());
    return ret;
}

VectorPath VectorPath::split(qreal pos)
{
    qreal len = 0;
    VectorPath rest;
    int i;
    QList<VNode>::iterator it = nodes.begin();
    while (it != nodes.end()) {
        if ((len + it->sideLength) >= pos)
            break;
        len += it->sideLength;
        ++it;
    }
    QPointF stp = it->pointAtLength(pos - len);
    ++it;
    rest.addPoint(stp);
    QList<VNode>::iterator it2 = it;
    //    --it2;
    while (it2 != nodes.end()) {
        rest.addPoint(it2->point);
        ++it2;
    }
    rest.addPoint(lastNode.point);
    lastNode.point = QPointF();
    nodes.erase(it, nodes.end());
    addPoint(stp);
    nodes.last().next = &rest.nodes.first();
    rest.nodes.first().prev = &nodes.last();
    //    if (nodes.last().next) {
    //        nodes.last().next->prev = nullptr;
    //        nodes.last().next = nullptr;
    //    }
    m_length = pos;
    return rest;
}

VectorPath VectorPath::reversed() const
{
    VectorPath ret;
    for (const VNode &node : nodes) {
        VNode n;
        n.sideLength = node.sideLength;
        n.point = node.endPoint();
        n.unitVector = -node.unitVector;
        n.prev = node.next;
        n.next = node.prev;
        n.data = node.data;
        ret.prependNode(n);
    }
    return ret;
}

VectorPath::Runner VectorPath::split(qreal pos, VectorPath *left, VectorPath *right) const
{
    Runner ret = setRunner(pos);
    VectorPath *p = left;
    for (const VNode &n:nodes) {
        VNode *prev = nullptr;
        if (!p->nodes.isEmpty())
            prev = &p->nodes.last();
        p->nodes.append(n);
        p->nodes.last().prev = prev;
        if (prev) {
            prev->next = &p->nodes.last();
        }
        if (&n == ret.m_node) {
            p->nodes.last().sideLength = ret.lengthBehind();
            p->nodes.last().next = nullptr;
            p->lastNode.point = ret.point();
            p = right;
            p->addPoint(ret.point(), n.data);
            p->addPoint(ret.m_node->endPoint(), n.data);
        }
    }
    p->lastNode = lastNode;
    left->m_length = pos;
    right->m_length = m_length - pos;
    return ret;
}

VectorPath VectorPath::buildRounding(qreal radius, qreal step, qreal startPos, qreal endPos) const
{
    VectorPath result;
    if (nodes.isEmpty())
        return result;
    Runner r = setRunner(startPos);
    qreal k;
    bool expectExit = false;
    qreal behind = r.straightLengthBehind(radius / 2, &k);
    if (behind < 0) {
        expectExit = true;
    }
    qreal rd = radius * k;
    qreal fwd;
    QPointF sp = r.getSmoothedPoint(radius);
    result.addPoint(sp, r.position());
    qreal dist = endPos - startPos;
    while (dist > 0 && !r.atEnd()) {
        if (!expectExit) {
            fwd = r.straightLengthAhead(radius / 2, &k);
            rd = radius * k;
            if (fwd > 0) {
                if (k < 0.01) {
                    fwd = r.lengthAhead();
                    dist -= qMin(dist, fwd);
                    if (fwd > dist) {
                        r.moveBy(dist);
                        result.addPoint(r.getSmoothedPoint(rd), r.position());
                        dist = 0;
                        break;
                    } else {
                        r.moveBy(fwd);
                        result.addPoint(r.point(), r.position());
                        continue;
                    }
                }
                if (fwd > dist) {
                    r.moveBy(dist);
                    dist = 0;
                    result.addPoint(r.getSmoothedPoint(rd), r.position());
                    break;
                }
                dist -= r.moveBy(fwd);
            }
        }
        while (true) {
            result.addPoint(r.getSmoothedPoint(rd), r.position());
            if (r.lengthAhead() < step)
                expectExit = true;
            dist -= r.moveBy(qMin(step, dist));
            if (dist <= 1E-9) {
                dist = 0;
                break;
            }
            if (r.atEnd()) {
                break;
            }
            if(expectExit) {
                qreal ahead = r.lengthAhead();
                qreal behind = r.lengthBehind();
                if (behind > rd / 2 && ahead > radius / 2) {
                    expectExit = false;
                    break;
                }
            }
        }
        result.addPoint(r.getSmoothedPoint(rd), r.position());
    }
    if (r.atEnd() && dist > 1)
        qDebug() << "remaining dist" << dist;
    return result;
}

VectorPath VectorPath::buildOffset(qreal offset, bool dataAsNormal) const
{
    QPolygonF raw;
    VectorPath result;
    if (nodes.isEmpty())
        return result;
    VNode node = nodes.first();
    qreal o;
    if (dataAsNormal)
        o = node.data.toReal();
    else
        o = offset;
    raw.append(node.offsetPoint(o));
    QLineF prev_seg;
    QVector2D prev_vec;
    QLineF::IntersectType iType;
    for (int i = 0; i < size(); ++i) {
        const VNode &wn1 = nodes.at(i);
        QVector2D cvec = wn1.vector();
        if (dataAsNormal)
            o = wn1.data.toReal();
        else
            o = offset;
        QPointF p1_1 = wn1.point + (wn1.normal() * o).toPointF();
        QPointF p2_1 = wn1.endPoint() + (wn1.normal() * o).toPointF();
        QLineF seg(p1_1, p2_1);
        if (!prev_seg.isNull()) {
            QPointF iPoint;
            iType = prev_seg.intersect(seg, &iPoint);
            if (iType == QLineF::NoIntersection) {
                raw.append(p1_1);
                continue;
            }
            QVector2D ovec(iPoint - raw.last());
            QVector2D fvec(p2_1 - iPoint);
            qreal dp = QVector2D::dotProduct(ovec, prev_vec);
            if (dp < 0) {
                raw.append(p1_1);
            } else {
                dp = QVector2D::dotProduct(ovec, fvec);
                if (dp < 0) {
                    raw.append(p1_1);
                } else {
                    raw.append(iPoint);
                }
            }
        }
        prev_vec = cvec;
        prev_seg = seg;
    }

    node = nodes.last();
    if (dataAsNormal)
        o = node.data.toReal();
    else
        o = offset;
    raw.append(lastNode.point + (node.normal() * o).toPointF());
    result.addPoint(raw.first(), nodes.first().data);
    bool inLoop;
    int loopEnterIdx = 0;
    int loopExitCandidate = 0;
    do {
        inLoop = false;
        for (int i = loopEnterIdx; i < raw.size() - 1; ++i) {
            const QPointF &p1 = raw[i];
            const QPointF &p2 = raw[i + 1];
            const VNode &cNode = nodes[i];
            QLineF cseg = QLineF(p1, p2);
            QVector2D sVec(cNode.unitVector);
            QVector2D cVec(cseg.dx(), cseg.dy());
            qreal dp = QVector2D::dotProduct(cVec.normalized(), sVec);
            if (dp < 0) {
                if (!inLoop)
                    loopEnterIdx = i;
                inLoop = true;
            } else {
                if (inLoop) {

                    int j = result.size() - 1;
                    while (j >= 0) {
                        QLineF seg = result.segment(j);
                        QPointF iPoint;
                        iType = cseg.intersect(seg, &iPoint);
                        if (iType == QLineF::BoundedIntersection) {
                            result.resize(j + 1);
                            result.addPoint(iPoint, nodes[loopEnterIdx].data);
                            result.addPoint(p2, cNode.data);
                            inLoop = false;
                            loopExitCandidate = 0;
                            break;
                        }
                        if (loopExitCandidate == 0 && dp == 1.0) {
                            loopExitCandidate = i;
                            //                            qDebug() << "Exit candidate for loop from" << loopEnterIdx << loopExitCandidate << dp;
                        }
                        --j;
                    }
                } else {
                    result.addPoint(p2, cNode.data);
                }
            }
        }
        if (inLoop) {
            qDebug() << "in loop from node" << loopEnterIdx++;
            result.addPoint(raw.at(loopEnterIdx));
            result.addPoint(raw.at(loopExitCandidate));
            if (loopExitCandidate != 0)
                loopEnterIdx = loopExitCandidate;
            else
                ++loopEnterIdx;
            if (loopEnterIdx >= raw.size() - 1) {
                result.addPoint(raw.last());
                break;
            }
            loopExitCandidate = 0;
            continue;
        }
    } while (inLoop);
//    qDebug() << "build offset size" << result.size() << "from source" << nodes.size();
    return result;
}

VectorPath::link VectorPath::attachTail(VectorPath &other)
{
    return link(&nodes.last(), &other.nodes.first());
}

VectorPath::link VectorPath::attachHead(VectorPath &other)
{
    return link(&other.nodes.last(), &nodes.first());

}

QPointF VectorPath::pointAtLength(qreal pos, int *legIndex) const
{
    if (nodes.isEmpty())
        return QPointF();
    if (pos < 0) {
        if (legIndex)
            *legIndex = 0;
        return nodes.first().point;
    }
    int i;

    for (i = 0; i < nodes.size(); ++i) {
        const VNode &n = nodes.at(i);
        if (pos <= n.sideLength)
            break;
        pos -= n.sideLength;
    }
    const VNode &n = nodes.at(i);
    if (legIndex)
        *legIndex = i;
    if (pos > n.sideLength) {
        return n.endPoint();
    }
    return n.pointAtLength(pos);
}

void VectorPath::resize(int n)
{
    if (nodes.last().next)
        nodes.last().next->prev = nullptr;
    while (nodes.size() > n && !nodes.isEmpty()) {
        nodes.removeLast();
    }
    if (!nodes.isEmpty()) {
        nodes.last().next = nullptr;
    }
    lastNode.prev = nullptr;
    lastNode.point = QPointF();
}

void VectorPath::addPoint(const QPointF &point, QVariant nodeData)
{
    if (lastNode.point == point)
        return;
    if (!lastNode.point.isNull()) {
        if (!nodes.isEmpty()) {
            VNode &p = nodes.last();
            nodes.append(lastNode);
            p.next = &nodes.last();
        } else {
            nodes.append(lastNode);
        }
    }
    lastNode.point = point;
    lastNode.data = nodeData;
    if (nodes.isEmpty()) {
        return;
    }
    VNode &p = nodes.last();
    //    if (p.point == lastNode.point)
    //        return;
    QVector2D seg(lastNode.point - p.point);
    p.sideLength = seg.length();
    m_length += p.sideLength;
    p.unitVector = seg.normalized();
    lastNode.prev = &p;
}


void VectorPath::addPolygon(const QPolygonF &polygon, QVariant nodeData)
{
    for (QPointF pt:polygon)
        addPoint(pt, nodeData);
}

void VectorPath::addVectorPath(const VectorPath &other, QVariant data)
{
    if (other.isNull())
        return;
    // TODO: write an optimized merge method
    for (const VNode &n:other.nodes) {
        if (data.isValid())
            addPoint(n.point, data);
        else
            addPoint(n.point, n.data);
    }
    if (data.isValid())
        addPoint(other.lastNode.point, data);
    else
        addPoint(other.lastNode.point, other.lastNode.data);
}

QPolygonF VectorPath::toPolygon() const
{
    QPolygonF ret;
    for (const VNode &n:nodes) {
        ret.append(n.point);
    }
    if (!nodes.isEmpty())
        ret.append(nodes.last().endPoint());
//    ret.append(lastNode.point);
    return ret;
}

QPointF VectorPath::nearestPoint(const QPointF &pos, qreal *length) const
{
    QPointF ret;
    QVector2D point(pos);
    qreal len = 0;
    qreal mindst = 1e+127;
    for (int i = 0; i < nodes.size(); ++i) {
        const VNode &n = nodes[i];
        //        qreal c_dst = c_vec.length();
        qreal dst = point.distanceToLine(QVector2D(n.point), n.unitVector);
        if (dst < mindst) {
            QPointF endP = (n.normal() * (dst + 1)).toPointF();
            QLineF l_perpendicular(pos - endP, pos + endP);
            QPointF iPoint;
            QLineF::IntersectType sect = n.segment().intersect(l_perpendicular, &iPoint);
            if (sect == QLineF::BoundedIntersection) {
                mindst = dst;
                ret = iPoint;
                qreal seg_pos = QLineF(n.point, iPoint).length();
                *length = len + seg_pos;
            }
        }
        QLineF st(pos, n.point);
        qreal stlen = st.length();
        if (stlen < mindst) {
            mindst = stlen;
            *length = len;
            ret = n.point;
        }
        len += n.sideLength;
    }
    QLineF e(pos, nodes.last().endPoint());
    qreal elen = e.length();
    if (elen < mindst) {
        *length = m_length;
        ret = nodes.last().endPoint();
    }
    return ret;
}

void VectorPath::clear()
{
    nodes.clear();
    lastNode.point = QPointF();
    m_length = 0;
}

void VectorPath::dumpNodes() const
{
    int i = 1;
    QDebug d = qDebug().nospace();
    d << "[ ";
    qreal l = 0;
    for (const VNode &n:nodes) {
        if (n.prev) {
            if (n.isConnectedTo(n.prev))
                d << "->";
            else
                d << "»|";
        } else
            d << "  ";
        d << " ( l=" << QString::number(n.sideLength, 'f', 1).toLocal8Bit().data() << " ) ";
        if (n.next) {
            if (n.isConnectedTo(n.next))
                d << "<-";
            else
                d << "|«";
        } else
            d << "  ";
        l += n.sideLength;
    }
    d << " ]\t\t sum:" << l << " len:" << m_length;
}

qreal VectorPath::checkLength()
{
    qreal l = 0;
    for (const VNode &n:nodes) {
        l += n.sideLength;
    }
    qreal d = m_length - l;
    if (m_length != l) {
        if (qAbs(d) > 1)
            qDebug() << "length mismatch, delta =" << d;
        m_length = l;
    }
    return d;
}

const VectorPath::VNode *VectorPath::head() const
{
    if (!nodes.isEmpty())
        return &nodes.first();
    return nullptr;
}

const VectorPath::VNode *VectorPath::tail() const
{
    if (!nodes.isEmpty())
        return &nodes.last();
    return nullptr;
}

const VectorPath::VNode *VectorPath::nodeAt(int index) const
{
    if (nodes.size() < index)
        return &nodes.at(index);
    return nullptr;
}

void VectorPath::appendNode(const VectorPath::VNode &node)
{
    VNode *tail = nullptr;
    if (!nodes.isEmpty()) {
        tail = &nodes.last();
    }
    nodes.append(node);
    nodes.last().setPrev(tail);
    m_length += node.sideLength;
}

void VectorPath::prependNode(const VectorPath::VNode &node)
{
    VNode *head = nullptr;
    if (!nodes.isEmpty()) {
        head = &nodes.first();
    }
    nodes.prepend(node);
    nodes.first().setNext(head);
    m_length += node.sideLength;
//    qDebug() << "prepend, len =" << node.sideLength << "length =" << m_length;
}

VectorPath::VNode VectorPath::takeLastNode()
{
    m_length -= nodes.last().sideLength;
//    qDebug() << "take last, len =" << nodes.last().sideLength << "length =" << m_length;
    return nodes.takeLast();
}

VectorPath::VNode VectorPath::takeFirstNode()
{
    m_length -= nodes.first().sideLength;
//    qDebug() << "take first, len =" << nodes.first().sideLength << "length =" << m_length;
    return nodes.takeFirst();
}

qreal VectorPath::Runner::moveBy(qreal len)
{
    if (len > 0) {
        return moveForward(len);
    } else {
        return -moveBack(-len);
    }
}

qreal VectorPath::Runner::straightLengthBehind(qreal chamferRadius, qreal *ratio) const
{
    qreal dst = lengthBehind();
    if (ratio) {
        *ratio = 1;
    }
    if (!m_node->prev)
        return dst;
    return dst - chamferRadius;
    qreal coef = prevAngularRatio();
    //    qDebug() << "neg angular ratio" << coef;
    if (ratio)
        *ratio = coef;
    return dst - chamferRadius * coef;

}
qreal VectorPath::Runner::straightLengthAhead(qreal chamferRadius, qreal *ratio) const
{
    qreal dst = lengthAhead();
    if (ratio) {
        *ratio = 1;
    }
    if (!m_node->next)
        return dst;
    return dst - chamferRadius;
    qreal coef = nextAngularRatio();
    //    qDebug() << "pos angular ratio" << coef;
    if (ratio)
        *ratio = coef;
    return dst - chamferRadius * coef;
}

QPointF VectorPath::Runner::getSmoothedPoint(qreal range) const
{
    QPointF cPt = point();
    if (range == 0 || isnan(range))
        return cPt;
    Runner neg(*this);
    Runner pos(*this);
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

QVector2D VectorPath::Runner::toPrevNode(qreal travelLimit, qreal *factTravel)
{
    qreal dst = qMin(travelLimit, lengthBehind());
    if (factTravel)
        *factTravel = dst;
    m_position -= dst;
    if (m_atNode) {
        if (!m_node->prev) {
            m_atStart = true;
            return QVector2D();
        }
        m_node = m_node->prev;
        node_pos = m_node->sideLength;
    }
    if (dst != travelLimit) {
        // dst == lengthBehind, so move to node
        node_pos = 0;
        m_atNode = true;
    } else {
        // move inside current leg
        node_pos -= dst;
        m_atNode = false;
        //        if (node_pos < 1E-12) {
        //            qDebug() << "In fact at leg start, i.e. at node";
        //            m_atNode = true;
        //            m_node = m_node->prev;
        //            node_pos = 0;
        //        }
    }
    m_atEnd = false;
    return -QVector2D(m_node->unitVector * dst);
}

QVector2D VectorPath::Runner::toNextNode(qreal travelLimit, qreal *factTravel)
{
    qreal dst = qMin(travelLimit, lengthAhead());
    if (factTravel)
        *factTravel = dst;
    m_position += dst;
    QVector2D ret(m_node->unitVector * dst);
    if (dst != travelLimit) {
        // dst == lengthAhead, so move to node
        if (!m_node->next) {
            m_atEnd = true;
            node_pos = m_node->sideLength;
        } else {
            m_node = m_node->next;
            m_atEnd = false;
            node_pos = 0;
        }
        m_atNode = true;
    } else {
        // move inside leg OR to end of leg
        node_pos += dst;
        m_atNode = false;

        if (node_pos >= m_node->sideLength) {
            m_atNode = true;
            if (hasNext()) {
                node_pos -= m_node->sideLength;
                m_node = m_node->next;
            } else {
                m_atEnd = true;
            }
        }
    }
    m_atStart = false;
    Q_ASSERT(m_node != nullptr);
    return ret;
}



qreal VectorPath::Runner::moveForward(qreal len)
{
    qreal run = m_position;
    m_atStart = false;
    while (len > lengthAhead()) {
        qreal st = lengthAhead();
        len -= st;
        m_position += st;
        m_atNode = true;
        if (!m_node->next) {
            node_pos += st;
            break;
        }
        node_pos = 0;
        m_node = m_node->next;
    }
    if (len >= lengthAhead() && !hasNext()) {
        len = lengthAhead();
        m_atEnd = true;
    }
    if (len > 0) {
        m_position += len;
        node_pos += len;
        //        if (!m_atEnd)
        m_atNode = false;

    }
    return m_position - run;
}

qreal VectorPath::Runner::moveBack(qreal len)
{
    qreal run = m_position;
    m_atEnd= false;
    while (len >= node_pos) {
        qreal st = node_pos;
        len -= node_pos;
        m_position -= node_pos;
        node_pos = 0;
        m_atNode = true;
        if (!m_node->prev) {
            m_atStart = true;
            break;
        }
        m_node = m_node->prev;
        node_pos = m_node->sideLength;
    }
    if (!m_atStart && len > 0) {
        m_position -= len;
        node_pos -= len;
        m_atNode = false;
    }
    return run - m_position;
}


VectorPath::Runner::Runner(VectorPath::VNode *node, qreal pos) :
    m_node(node),
    m_position(0),
    node_pos(0),
    m_atStart(true),
    m_atEnd(false),
    m_atNode(false)
{
    if (pos <= 0) {
        //        m_atNode = true;
        m_atStart = true;
        return;
    }
    m_atStart = false;
    m_atNode = false;
    moveBy(pos);
}


QVariant VectorPath::Runner::data() const
{
    return m_node->data;
}

QVariant VectorPath::Runner::nextData() const
{
    if (m_node->next)
        return m_node->next->data;
    return QVariant();
}

QVariant VectorPath::Runner::prevData() const
{
    if (m_node->prev)
        return m_node->prev->data;
    return QVariant();
}

QVariant *VectorPath::Runner::dataPtr() const
{
    return &m_node->data;
}

void VectorPath::Runner::setData(const QVariant &data)
{
    m_node->data = data;
}


VectorPath::link::~link()
{
    *node1 = oldNode1;
    *node2 = oldNode2;
}

VectorPath::link::link(VectorPath::VNode *node1, VectorPath::VNode *node2) :
    node1(node1),
    node2(node2)
{
    oldNode1 = *node1;
    oldNode2 = *node2;
    node1->setNext(node2);
    node2->setPrev(node1);
}

qreal VectorPath::VNode::setLength(qreal length)
{
    Q_ASSERT(next == nullptr || prev == nullptr);
    qreal dl = length - length;
    if (next && !prev) {
        point -= (unitVector * dl).toPointF();
    }
    length = length;
    return dl;
}

void VectorPath::VNode::shrink(qreal d)
{
    Q_ASSERT(next == nullptr || prev == nullptr);
    if (next && !prev) {
        point += (unitVector * d).toPointF();
    }
    sideLength -= d;
}

void VectorPath::VNode::extend(qreal d)
{
    Q_ASSERT(next == nullptr || prev == nullptr);
    if (next && !prev) {
        point -= (unitVector * d).toPointF();
    }
    sideLength += d;
}

void VectorPath::VNode::link(VectorPath::VNode *left, VectorPath::VNode *right)
{
    left->next = right;
    right->prev = left;
}
