#include "polychamfer.h"
#include <QLineF>
#include "../../libs/Graphics/polygon.h"
#include "../../libs/Graphics/graphicspath.h"
#include <QDebug>
#include <QVector2D>


inline QPointF segPointAtPercent(const QPointF &p1, const QPointF &p2, qreal percent)
{
    return p1 + percent * (p2 - p1);
}
inline QPointF segPointAtLength(const QPointF &p1, const QPointF &p2, qreal length)
{
    qreal l = QLineF(p1, p2).length();
    return segPointAtPercent(p1, p2, length / l);
}

qreal normalizeAngle(qreal angle)
{
    qreal m = fmod(angle, 360);
    if (m < 0)
        m += 360;
    if (m > 180)
        m -= 360;
    return m;
}

void PolyChamfer::setPolygon(GPolygon *arg)
{
    if (m_gpolygon == arg)
        return;
    m_gpolygon = arg;
    int pidx = m_gpolygon->metaObject()->indexOfProperty("polygon");
    polProp = m_gpolygon->metaObject()->property(pidx);
    emit polygonChanged(arg);
}

void PolyChamfer::setPath(GraphicsPath *arg)
{
    if (m_path == arg)
        return;

    m_path = arg;
    int pidx = m_path->metaObject()->indexOfProperty("path");
    pathProp = m_path->metaObject()->property(pidx);
    emit pathChanged(arg);
}





int PolyChamfer::order() const
{
    return m_order;
}

qreal PolyChamfer::strength() const
{
    return m_strength;
}

qreal PolyChamfer::offset() const
{
    return m_offset;
}

qreal PolyChamfer::curveThreshold() const
{
    return m_curveThreshold;
}

QPolygonF PolyChamfer::calculateOffset(const SpringWire &wire, qreal offset)
{
    QPolygonF raw;

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
        qDebug() << "bad source data starting from node" << result.size();
        int i = 0;
        while (i < wire.size()) {
            QString str = QString("=(%1,%2)\t=(%3,%4)").arg(raw[i].x()).arg(raw[i].y()).arg(wire[i].pt.x()).arg(wire[i].pt.y());
            if (i + 1 < wire.size()) {
                QPointF lp1 = wire[i].point(offset);
                QPointF lp2 = wire[i+1].pt + (wire[i].normal * offset).toPointF();
                str += QString("\t=(%1,%2)\t=(%3,%4)").arg(lp1.x()).arg(lp1.y()).arg(lp2.x()).arg(lp2.y());
            }
            if (i < result.size()) {
                str.append(QString("\t=(%1,%2)").arg(result[i].x()).arg(result[i].y()));
            }
            qDebug() << str.toLocal8Bit().data();
            ++i;
        }
    }
#endif
    return result;
}

void PolyChamfer::init()
{
    m_polygon.clear();
    QPolygonF p = getPolygon();
    m_length = 0;
    qreal prevAngle;
    nodes.clear();
    nodes.reserve(p.size());
    for (int i = 0; i < p.size() - 1; ++i) {
        QLineF seg2(p.at(i), p.at(i + 1));
        PolNode n;
        n.segLength = seg2.length();
        n.position = m_length;
        n.azimuth = seg2.angle();
        m_length += n.segLength;
        QPointF vector = p.at(i + 1) - p.at(i);
        n.normal = QVector2D(-vector.y(), vector.x()).normalized();
        if (i) {
            n.angle = normalizeAngle(n.azimuth - prevAngle);
        }
        prevAngle = n.azimuth;
        nodes.append(n);
    }
    PolNode n;
    n.position = m_length;
    if (!nodes.isEmpty())
        n.normal = nodes.last().normal;
    nodes.append(n);
    emit lengthChanged(m_length);
    update();
}

void PolyChamfer::update()
{
    updatePath(m_strength, m_offset, m_order, m_curveThreshold);
}

void PolyChamfer::updatePath(qreal smooth, qreal offset, int order, qreal curveThreshold)
{
    QPainterPath path;
    SpringWire wire = getRounding(smooth, order);
    if (wire.isEmpty())
        return;
    QVector<QPointF> ch;
    for (const WireNode &n:wire) {
        ch.append(n.pt);
    }
//    path.addPolygon(ch);
        path.addPolygon(calculateOffset(wire, offset));
    writePath(path);
}


void PolyChamfer::setOrder(int arg)
{
    if (m_order == arg)
        return;

    m_order = arg;
    emit orderChanged(arg);
    update();
}

void PolyChamfer::setStrength(qreal arg)
{
    if (m_strength == arg)
        return;

    m_strength = arg;

    emit strengthChanged(arg);
    update();
}

void PolyChamfer::setOffset(qreal arg)
{
    if (m_offset == arg)
        return;

    m_offset = arg;
    emit offsetChanged(arg);
    update();
}

void PolyChamfer::setCurveThreshold(qreal arg)
{
    if (m_curveThreshold == arg)
        return;

    m_curveThreshold = arg;
    emit curveThresholdChanged(arg);
    update();
}

QPointF PolyChamfer::pointAtLength(qreal pos, int *nodeNo) const
{
    QPolygonF p = getPolygon();
    if (pos < 0) {
        if (nodeNo)
            *nodeNo = 0;
        return p.first();
    }
    int i = 0;
    for (const PolNode &n:nodes) {
        if (n.position + n.segLength < pos)
            ++i;
        else
            break;
    }
    if (nodeNo)
        *nodeNo = i;
    if (i < nodes.size() - 1)
        return segPointAtPercent(p.at(i), p.at(i + 1), (pos - nodes.at(i).position) / nodes.at(i).segLength);
    return p.last();
}

QPointF PolyChamfer::pointAtPercent(qreal percent) const
{
    return pointAtLength(m_length * percent);
}

QPointF PolyChamfer::getCurvePoint(qreal position, qreal range, int order, bool *sameSegment, int *startNode, int *endNode) const
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
    if (startPos < 0) {
        underShoot = -startPos / range * 2;
        --stNode;
    }
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
    return cPt +
            negVec * (1 - overShoot) + posVec * overShoot +
            posVec * (1 - underShoot) + negVec * underShoot;
}

SpringWire PolyChamfer::getRounding(qreal smooth, int order, qreal minAngleThreshold, qreal maxAngleThreshold)
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
    qreal step = smooth / order;
    while (pos <= m_length) {
        const PolNode &node = nodes.at(i);
        if (i + 2 == m_polygon.size())
            break;
        qreal straightEnd = (node.position + node.segLength) - smooth / 2;
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
            w.pt = getCurvePoint(pos, smooth, order, &onSameSegment, &stnode, &idx);
            if (onSameSegment)
                break;
            result.prepareAppend(w, &dist, &turn);
            //            if (qAbs(turn) > 160 && result.size() > 1) {
            //#ifdef QT_DEBUG
            //                qDebug() << "problem at node:" << result.size() << "\nangle:" << turn << "\ndistance from last:" << dist << "\nspan:" << idx - stnode;
            //#endif
            //                continue;
            //            }
            if (dist > 0)
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

QPolygonF PolyChamfer::getPolygon() const
{
    if (!polProp.isValid() || !m_gpolygon)
        return QPolygonF();
    if (m_polygon.isEmpty())
        m_polygon = polProp.read(m_gpolygon).value<QPolygonF>();
    return m_polygon;
}

void PolyChamfer::writePath(const QPainterPath &path) const
{
    if (!m_path)
        return;
    pathProp.write(m_path, QVariant::fromValue(path));
}






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
