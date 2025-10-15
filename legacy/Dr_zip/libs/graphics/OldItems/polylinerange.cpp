#include "polylinerange.h"
#include <QLineF>
#include <QDebug>
inline QPointF segPointAtPercent(const QPointF &p1, const QPointF &p2, qreal percent)
{
    return p1 + percent * (p2 - p1);
}
inline QPointF segPointAtLength(const QPointF &p1, const QPointF &p2, qreal length)
{
    qreal l = QLineF(p1, p2).length();
    return segPointAtPercent(p1, p2, length / l);
}

//qreal normalizeAngle(qreal angle)
//{
//    qreal m = fmod(angle, 360);
//    if (m < 0)
//        m += 360;
//    if (m > 180)
//        m -= 360;
//    return m;
//}

void SpringWire::prepareAppend(const WireNode &node, qreal *distance, qreal *angleDelta) const
{
    QPointF vector;
    preparedNode = &node;
    if (isEmpty()) {
        //        append(node);
        if (angleDelta)
            *angleDelta = 0;
        if (distance)
            *distance = INFINITY;
        return;
    } else {
        vector = node.pt - last().pt;
        preparedAngle = qAtan2(vector.y(), vector.x()) * 180 / M_PI;
        if (angleDelta) {
            if (isnan(m_previousAngle))
                *angleDelta = 0;
            else
                *angleDelta = normalizeAngle(m_previousAngle - preparedAngle);
        }
        if (distance)
            *distance = qSqrt(vector.x() * vector.x() + vector.y() * vector.y());
    }
}

void SpringWire::commitAppend()
{
    if (preparedNode) {
        m_previousAngle = preparedAngle;
        WireNode w2 = *preparedNode;
        if (!isEmpty()) {
            WireNode &w1 = last();
            QPointF dp(w2.pt - w1.pt);
            QVector2D normal(-dp.y(), dp.x());
            normal.normalize();
            w1.normal = normal;
            w2.normal = normal;
        }
        append(w2);
        preparedNode = nullptr;
    }
}

QPolygonF SpringWire::toPolygonF() const
{
    QPolygonF ret;
    for (int i = 0; i < size(); ++i) {
        ret.append(at(i).pt);
    }
    return ret;
}

PolyLineRange::PolyLineRange()
{
}

void PolyLineRange::calculateRounded(qreal radius, int precision)
{
    if (m_polygon.length() < 2)
        return;
    m_wire.clear();
    qreal pos = 0;
    int i = 0;
    WireNode w;
    QVector2D normal = nodes.first().normal;
    w.pt = pointAtLength(pos, &i);
    w.normal = normal;
    m_wire.prepareAppend(w);
    m_wire.commitAppend();
    if (m_polygon.length() < 3) {
        w.normal = nodes.last().normal;
        w.pt = m_polygon.last();
        m_wire.prepareAppend(w);
        m_wire.commitAppend();
        return;
    }
    qreal step = radius / precision;
    while (pos <= m_length) {
        const PolNode &node = nodes.at(i);
        if (i + 2 == m_polygon.size())
            break;
        qreal straightEnd = (node.position + node.segLength) - radius / 2;
        if (straightEnd > pos) {
            pos = straightEnd;
        }
        bool onSameSegment = false;
        int idx;
        while (!onSameSegment) {
            pos += step;
            if (pos >= m_length)
                break;
            qreal dist;
            qreal turn;
            int stnode;
            w.pt = getCurvePoint(pos, radius, precision, &onSameSegment, &stnode, &idx);
            if (onSameSegment)
                break;
            m_wire.prepareAppend(w, &dist, &turn);
            if (qAbs(turn) > 160 && m_wire.size() > 1) {
#ifdef QT_DEBUG
                qDebug() << "problem at node:" << m_wire.size() << "\nangle:" << turn << "\ndistance from last:" << dist << "\nspan:" << idx - stnode;
#endif
                continue;
            }
            if (dist > 0)
                m_wire.commitAppend();
        } // finished rounding
        w.pt = pointAtLength(pos, &i);
        if (onSameSegment) {
            w.normal = nodes.at(i).normal;
            m_wire.prepareAppend(w);
            m_wire.commitAppend();
        }
    }
    w.pt = m_polygon.last();
    w.normal = nodes.last().normal;
    m_wire.prepareAppend(w);
    m_wire.commitAppend();
}

void PolyLineRange::calculateCommonRounded(qreal radius, int precision)
{
    if (m_polygon.length() < 2)
        return;
    if (m_length < radius)
        return;
    m_wire.clear();
    qreal pos = radius / 2;
    qreal endPos = m_length - radius / 2;
    int i = 0;
    WireNode w;
    w.pt = pointAtLength(pos, &i);
    w.normal = nodes.at(i).normal;
    m_wire.prepareAppend(w);
    m_wire.commitAppend();
    qreal step = radius / precision;
    while (pos <= endPos) {
        const PolNode &node = nodes.at(i);
        if (i + 2 == m_polygon.size())
            break;
        qreal straightEnd = (node.position + node.segLength) - radius / 2;
        if (straightEnd > pos) {
            pos = straightEnd;
        }
        if (pos >= endPos) {
            pos = endPos;
            w.pt = pointAtLength(pos);
            w.normal = node.normal;
            m_wire.prepareAppend(w);
            m_wire.commitAppend();

            break;
        }
        bool onSameSegment = false;
        int idx;
        while (!onSameSegment) {
            pos += step;
            if (pos >= endPos)
                break;
            qreal dist;
            qreal turn;
            int stnode;
            w.pt = getCurvePoint(pos, radius, precision, &onSameSegment, &stnode, &idx);
            if (onSameSegment)
                break;
            m_wire.prepareAppend(w, &dist, &turn);
            if (qAbs(turn) > 160 && m_wire.size() > 1) {
#ifdef QT_DEBUG
                qDebug() << "problem at node:" << m_wire.size() << "\nangle:" << turn << "\ndistance from last:" << dist << "\nspan:" << idx - stnode;
#endif
                continue;
            }
            if (dist > 0)
                m_wire.commitAppend();
        } // finished rounding
        if (pos >= endPos)
            break;
    }
//    qDebug() << "Finished" << pos - endPos << m_length << radius/2;
//    if (pos - endPos > 1) {
//        w.pt = getCurvePoint(endPos, radius, precision);
//        qreal dist;
//        m_wire.prepareAppend(w, &dist);
//        qDebug() << "Last point distance:" << dist;
//        m_wire.commitAppend();
//    }
}

void PolyLineRange::calculateSimple()
{
    m_wire.clear();
    for (int i = 0; i < nodes.size(); ++i) {
        WireNode w;
        w.pt = m_polygon.at(i);
        w.normal = nodes.at(i).normal;
        m_wire.prepareAppend(w);
        m_wire.commitAppend();
    }
}

qreal PolyLineRange::length() const
{
    return m_length;
}

QPointF PolyLineRange::nearestPathPoint(const QPointF &pos, qreal *length) const
{
    QPointF prev_pt;
    QPointF ret;
    QVector2D point(pos);
    qreal len = 0;
    qreal mindst = HUGE;
    for (int i = 0; i < m_polygon.size(); ++i) {
        QPointF c_pt = m_polygon[i];
        QVector2D c_vec(c_pt - pos);
        qreal c_len = c_vec.length();
        qreal seg_len = 0;
        if (!prev_pt.isNull()) {
            QVector2D seg_vec = QVector2D(c_pt - prev_pt);
            seg_len = seg_vec.length();
            QVector2D seg_dir = seg_vec / seg_len;
            qreal dst = point.distanceToLine(QVector2D(prev_pt), seg_dir);
            if (dst < mindst) {
                QVector2D norm = QVector2D(-seg_dir.y(), seg_dir.x());
                QLineF l_seg(prev_pt, c_pt);
                QLineF l_perpendicular(pos - (norm * 200 * (dst + 1)).toPointF(), pos + (norm * 200 * (dst + 1)).toPointF());
                QPointF iPoint;
                QLineF::IntersectType sect = l_seg.intersect(l_perpendicular, &iPoint);
                if (sect == QLineF::BoundedIntersection) {
                    mindst = dst;
                    ret = iPoint;
                    qreal seg_pos = QLineF(prev_pt, iPoint).length();
                    *length = len + seg_pos;
                }
            }
            if (c_len < mindst) {
                mindst = c_len;
                *length = len + seg_len;
                ret = c_pt;
            }
        } else {
            if (c_len < mindst) {
                mindst = c_len;
                *length = 0;
                ret = c_pt;
            }
        }

        len += seg_len;
        prev_pt = c_pt;
    }
    if (ret == m_polygon.last())
        *length = m_length;
    return ret;

}

SpringWire PolyLineRange::getRounding(qreal radius, int precision, qreal minAngleThreshold, qreal maxAngleThreshold)
{
    SpringWire result;
    if (m_polygon.length() < 2)
        return result;
    qreal pos = 0;
    int i = 0;
    WireNode w;
    QVector2D normal = nodes.first().normal;
    w.pt = pointAtLength(pos, &i);
    w.normal = normal;
    result.prepareAppend(w);
    result.commitAppend();
    if (m_polygon.length() < 3) {
        w.normal = nodes.last().normal;
        w.pt = m_polygon.last();
        result.prepareAppend(w);
        result.commitAppend();
        return result;
    }
    qreal step = radius / precision;
    while (pos <= m_length) {
        const PolNode &node = nodes.at(i);
        if (i + 2 == m_polygon.size())
            break;
        qreal straightEnd = (node.position + node.segLength) - radius / 2;
        if (straightEnd > pos) {
            pos = straightEnd;
        }
        bool onSameSegment = false;
        int idx;
        while (!onSameSegment) {
            pos += step;
            if (pos >= m_length)
                break;
            qreal dist;
            qreal turn;
            int stnode;
            w.pt = getCurvePoint(pos, radius, precision, &onSameSegment, &stnode, &idx);
            if (onSameSegment)
                break;
            result.prepareAppend(w, &dist, &turn);
            if (qAbs(turn) > 160 && result.size() > 1) {
#ifdef QT_DEBUG
                qDebug() << "problem at node:" << result.size() << "\nangle:" << turn << "\ndistance from last:" << dist << "\nspan:" << idx - stnode;
#endif
                continue;
            }
            if (dist > 0 && turn > minAngleThreshold)
                result.commitAppend();
        } // finished rounding
        w.pt = pointAtLength(pos, &i);
        if (onSameSegment) {
            w.normal = nodes.at(i).normal;
            result.prepareAppend(w);
            result.commitAppend();
        }
    }
    w.pt = m_polygon.last();
    w.normal = nodes.last().normal;
    result.prepareAppend(w);
    result.commitAppend();
    return result;
}


QPointF PolyLineRange::getCurvePoint(qreal position, qreal range, int order, bool *sameSegment, int *startNode, int *endNode) const
{
    QPointF ret;
    qreal startPos = position - range / 2;
    qreal endPos = position + range / 2;
    if (position < 0) {
        ret = m_polygon.first();
        return ret;
    }
    if (position >= m_length) {
        ret = m_polygon.last();
        return ret;
    }
    int stNode;
    int eNode;
    int cNode;
    QPointF sPt = pointAtLength(startPos, &stNode);
    QPointF cPt = pointAtLength(position, &cNode);
    QPointF ePt = pointAtLength(endPos, &eNode);
    if (startNode)
        *startNode = stNode;
    if (endNode)
        *endNode = eNode;
    if (stNode == eNode) { // begin and end on same segment
        if (sameSegment)
            *sameSegment = true;
        ret = cPt;
        return ret;
    }
    QPointF negVec;
    QPointF posVec;
    qreal underShoot = 0;
    if (startPos < 0)
        underShoot = -startPos / range * 2;
    qreal overShoot = 0;
    QPointF pp = cPt;
    qreal pos = position;
    for (int i = cNode; i > stNode; --i) {
        qreal w = pos - nodes[i].position;
        negVec += (pp + (m_polygon[i] - pp) / 2) * w / range;
        pos -= w;
        pp = m_polygon[i];
    }
    negVec += (pp + (sPt - pp) / 2) * (pos - startPos) / range;
    negVec -= cPt / 2;
    pp = cPt;
    pos = position;
    for (int i = cNode + 1; i < eNode + 1; ++i) {
        if (i == nodes.size()) {
            overShoot = (endPos - m_length) / range * 2;
            break;
        }
        qreal w = nodes[i].position - pos;
        posVec += (pp + (m_polygon[i] - pp) / 2) * w / range;
        pos += w;
        pp = m_polygon[i];
    }
    posVec += (pp + (ePt - pp) / 2) * (endPos - pos) / range;
    posVec -= cPt / 2;
    //    qDebug() << "negVec result:" << negVec * (1 - overShoot) + posVec * overShoot;
    //    qDebug() << "posVec result:" << posVec * (1 - underShoot) + negVec * underShoot;
    return cPt +
            negVec * (1 - overShoot) + posVec * overShoot +
            posVec * (1 - underShoot) + negVec * underShoot;
}

QPointF PolyLineRange::pointAtLength(qreal pos, int *nodeNo) const
{
    if (m_polygon.isEmpty())
        return QPointF();
    if (pos < 0) {
        if (nodeNo)
            *nodeNo = 0;
        return m_polygon.first();
    }
    int i = 0;
    for (const PolNode n:nodes) {
        if (n.position + n.segLength < pos)
            ++i;
        else
            break;
    }
    if (nodeNo)
        *nodeNo = i;
    if (i < nodes.length() - 1)
        return segPointAtPercent(m_polygon.at(i), m_polygon.at(i + 1), (pos - nodes.at(i).position) / nodes.at(i).segLength);
    return m_polygon.last();
}

QPointF PolyLineRange::pointAtPercent(qreal percent) const
{
    return pointAtLength(m_length * percent);
}

qreal PolyLineRange::angleAtLength(qreal pos, int *nodeNo) const
{
    if (m_polygon.isEmpty() || pos < 0 || pos > m_length)
        return NAN;
    int i = 0;
    for (const PolNode n:nodes) {
        if (n.position + n.segLength < pos)
            ++i;
        else
            break;
    }
    if (nodeNo)
        *nodeNo = i;
    if (i < nodes.length() - 1)
        return nodes.at(i).turnAngle;
    return NAN;
}

QPolygonF PolyLineRange::getPolylinePart(qreal startLength, qreal endLength) const
{
    QPolygonF result;
    int stIdx, eIdx;
    QPointF stP = pointAtLength(startLength, &stIdx);
    QPointF endP = pointAtLength(endLength, &eIdx);
    result.append(stP);
    //    qDebug() << "building polygon" << stP << endP << stIdx << eIdx << m_polygon.size();
    int inc = startLength <= endLength ? 1 : -1;
    if (stIdx != eIdx) {
        for (int i = stIdx + inc; i != eIdx + inc; i += inc) {
            if (i >= m_polygon.size()) {
                qDebug() << "vile obscenity";
                continue;
            }
            result.append(m_polygon[i]);
        }
    }
    result.append(endP);
    return result;
}

void PolyLineRange::init()
{
    m_length = 0;
    qreal prevAngle;
    nodes.clear();
    nodes.reserve(m_polygon.size());
    for (int i = 0; i < m_polygon.size() - 1; ++i) {
        QLineF seg2(m_polygon.at(i), m_polygon.at(i + 1));
        PolNode n;
        n.segLength = seg2.length();
        n.position = m_length;
        qreal azimuth = seg2.angle();
        m_length += n.segLength;
        QPointF vector = m_polygon.at(i + 1) - m_polygon.at(i);
        n.normal = QVector2D(-vector.y(), vector.x()).normalized();
        if (i) {
            n.turnAngle = normalizeAngle(azimuth - prevAngle);
        }
        prevAngle = azimuth;
        nodes.append(n);
    }
    PolNode n;
    n.position = m_length;
    if (!nodes.isEmpty())
        n.normal = nodes.last().normal;
    nodes.append(n);
}
SpringWire PolyLineRange::wire() const
{
    return m_wire;
}


QPolygonF PolyLineRange::polygon() const
{
    return m_polygon;
}

QPolygonF PolyLineRange::calculateOffset(const SpringWire &wire, qreal offset)
{
    QPolygonF raw;
    if (wire.isEmpty())
        return raw;
    WireNode node = wire.first();
    raw.append(node.point(offset));
    QLineF prev_seg;

    qreal prev_slope;
    for (int i = 0; i < wire.size() - 1; ++i) {
        const WireNode &wn1 = wire[i];
        const WireNode &wn2 = wire[i + 1];

        QLineF seg(wn1.point(offset), wn2.pt + (wn1.normal * offset).toPointF());
        qreal slope = seg.dy()/seg.dx();
        if (!prev_seg.isNull()) {
            QPointF iPoint;
            qreal dslope = slope - prev_slope;
            if (qAbs(dslope) > 1E-3) {
                prev_seg.intersect(seg, &iPoint);
                raw.append(iPoint);
            } else {
                raw.append((prev_seg.p2() + seg.p1()) / 2);
            }
        }
        prev_seg = seg;
        prev_slope = slope;
    }
    node = wire.last();
    raw.append(node.point(offset));
    QPolygonF result;
    result.append(raw.first());
    bool inLoop = false;
    for (int i = 0; i < raw.size() - 1; ++i) {
        const QPointF &p1 = raw[i];
        const QPointF &p2 = raw[i + 1];
        QLineF cseg = QLineF(p1, p2);
        QVector2D sVec(wire[i + 1].pt - wire[i].pt);
        QVector2D cVec(cseg.dx(), cseg.dy());
        qreal dir = QVector2D::dotProduct(cVec, sVec);
        if (dir < 0) {
            // vector from source polyline segment opposite to one from offset polyline segment means that we are inside a loop
            inLoop = true;
        } else {
            if (inLoop) {
                // try to find intersecting segment
                int j = result.size() - 1;
                bool found = false;
                while (!found && j > 0) {
                    QLineF seg(result[j - 1], result[j]);
                    QPointF iPoint;
                    QLineF::IntersectType iType = cseg.intersect(seg, &iPoint);
                    if (iType == QLineF::BoundedIntersection) {
                        result.resize(j);
                        result.append(iPoint);
                        result.append(cseg.p2());
                        inLoop = false;
                        found = true;
                        break;
                    }
                    --j;
                }
            } else {
                result.append(p2);
            }
        }
    }
#ifdef QT_DEBUG
    if (inLoop) {
        //        qDebug() << "bad source data starting from node" << result.size();
        //        int i = 0;
        //        while (i < raw.size()) {
        //            QString str = QString("=(%1,%2)\t=(%3,%4)").arg(raw[i].x()).arg(raw[i].y()).arg(wire[i].pt.x()).arg(wire[i].pt.y());
        //            if (i + 1 < raw.size()) {
        //                QPointF lp1 = wire[i].point(offset);
        //                QPointF lp2 = wire[i+1].pt + (wire[i].normal * offset).toPointF();
        //                str += QString("\t=(%1,%2)\t=(%3,%4)").arg(lp1.x()).arg(lp1.y()).arg(lp2.x()).arg(lp2.y());
        //            }
        //            if (i < result.size()) {
        //                str.append(QString("\t=(%1,%2)").arg(result[i].x()).arg(result[i].y()));
        //            }
        //            qDebug() << str.toLocal8Bit().data();
        //            ++i;
        //        }
        return raw;
    }
#endif
    return result;
}

void PolyLineRange::setPolygon(const QPolygonF &polygon)
{
    m_polygon = polygon;
    //    init();
}



//QPolygonF SpringWire::toPolygonF() const
//{
//    QPolygonF ret;
//    for (int i = 0; i < size(); ++i) {
//        ret.append(at(i).pt);
//    }
//    return ret;
//}
