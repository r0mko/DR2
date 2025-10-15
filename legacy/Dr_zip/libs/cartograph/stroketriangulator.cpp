#include "stroketriangulator.h"
#include <QVector2D>

StrokeTriangulator::StrokeTriangulator()
    : m_vertexBuffer(0)
{
}

StrokeTriangulator::StrokeTriangulator(VertexBuffer *vertexBuffer, qreal width, qreal offset, Qt::PenJoinStyle joinStyle)
    : m_vertexBuffer(vertexBuffer),
      m_joinStyle(joinStyle),
      m_width(width)
{
    setOffset(offset);
}


void StrokeTriangulator::triangulate(const QPolygonF &polygon, qreal lod)
{
    if (m_joinStyle == Qt::BevelJoin)
        lod = 0;
    if (m_widthLeft < 0 || m_widthRight < 0)
    {
        QPair<QPolygonF, QPolygonF> e = _getOffsetVertices(polygon, lod);
        Q_ASSERT(e.first.size() == e.second.size());
        for (int i = 0; i < e.first.size() - 1; ++i) {
            QPointF left0 = e.second.at(i);
            QPointF left1 = e.second.at(i + 1);
            QPointF right0 = e.first.at(i);
            QPointF right1 = e.first.at(i + 1);
//            QPolygonF tr1;
//            QPolygonF tr2;
            m_vertexBuffer->addTriangle(left0, left1, right0);
            m_vertexBuffer->addTriangle(right0, right1, left1);
        }
        return;
    }
    QVector2D prevNormal;
    qreal prevSlope;
    QPointF rightPoint;
    QPointF leftPoint;

    for (int i = 0; i < polygon.size() - 1; ++i)
    {
        QPointF n0 = polygon.at(i);
        QPointF n1 = polygon.at(i + 1);
        QVector2D seg(n1 - n0);
        seg.normalize();
        QVector2D normal;
        normal.setX(seg.y());
        normal.setY(-seg.x());
        if (i == 0) {
            // flat cap case
            leftPoint = n0 + (normal * m_widthLeft).toPointF();
            rightPoint = n0 - (normal * m_widthRight).toPointF();
            if (polygon.size() == 2) {
                QPointF endLeftPoint = n1 + (normal * m_widthLeft).toPointF();
                QPointF endRightPoint = n1 - (normal * m_widthRight).toPointF();
                m_vertexBuffer->addTriangle(leftPoint, endLeftPoint, rightPoint);
                m_vertexBuffer->addTriangle(rightPoint, endRightPoint, endLeftPoint);
                return;
            }
        }
        qreal ang = atan2(n1.y() - n0.y(), n1.x() - n0.x());
        if (prevNormal.isNull()) {
            prevNormal = normal;
            prevSlope = ang;
        } else {
            qreal angle = (prevSlope - ang);
            qreal w = m_widthLeft;
            bool left = true;
            if (angle < 0)
                angle += 2 * M_PI;
            if (angle > M_PI) {
                w = m_widthRight;
                left = false;
            }
            qreal l = w / cos(angle / 2);
            QPointF junc = n0 + ((prevNormal + normal).normalized() * l).toPointF(); // junction point
            if (left) {

                m_vertexBuffer->addVertex(junc);
                m_vertexBuffer->addVertex(leftPoint);
                m_vertexBuffer->addVertex(rightPoint);

                m_vertexBuffer->addVertex(rightPoint);
                m_vertexBuffer->addVertex(junc);
                leftPoint = junc;
                rightPoint = emitRoundJoin(n0, (w == m_widthRight ? m_widthLeft : m_widthRight), left, prevSlope, ang, junc, lod);
            } else {

                m_vertexBuffer->addVertex(junc);
                m_vertexBuffer->addVertex(leftPoint);
                m_vertexBuffer->addVertex(rightPoint);

                m_vertexBuffer->addVertex(leftPoint);
                m_vertexBuffer->addVertex(junc);
                rightPoint = junc;
                leftPoint = emitRoundJoin(n0, (w == m_widthRight ? m_widthLeft : m_widthRight), left, prevSlope, ang, junc, lod);
            }
            if (i == polygon.size() - 2) {
                QPointF endRight = n1 - (normal * m_widthRight).toPointF();
                QPointF endLeft = n1 + (normal * m_widthLeft).toPointF();

                m_vertexBuffer->addVertex(rightPoint);
                m_vertexBuffer->addVertex(leftPoint);
                m_vertexBuffer->addVertex(endRight);

                m_vertexBuffer->addVertex(leftPoint);
                m_vertexBuffer->addVertex(endRight);
                m_vertexBuffer->addVertex(endLeft);
            }
            prevSlope = ang;
            prevNormal = normal;
        }
    }
}

void StrokeTriangulator::setWidth(const qreal &width)
{
    m_width = width;
    m_widthLeft = width / 2 + m_offset;
    m_widthRight = width / 2 - m_offset;
}


void StrokeTriangulator::setOffset(const qreal &offset)
{
    m_offset = offset;
    setWidth(m_width);
}


QPointF StrokeTriangulator::emitRoundJoin(const QPointF &center, qreal radius, bool leftTurn, qreal startAngle, qreal endAngle, const QPointF &junction, qreal lod)
{
    qreal sweep = endAngle - startAngle;
    if (sweep < -M_PI) {
        if (endAngle < 0)
            endAngle += M_PI * 2;
        sweep = endAngle - startAngle;
    }
    if (sweep > M_PI) {
        if (startAngle < 0)
            startAngle += 2 * M_PI;
        sweep = endAngle - startAngle;
    }
    if (!leftTurn) {
        startAngle += M_PI;
        endAngle = startAngle + sweep;
    }
    QPointF startPoint = center + QPointF(-radius * sin(startAngle), radius * cos(startAngle));
    QPointF endPoint = center + QPointF(-radius * sin(endAngle), radius * cos(endAngle));

    int steps = qCeil(qAbs(sweep) * radius * lod / CURVE_THRESHOLD);
    qreal step = sweep / steps;
    m_vertexBuffer->addVertex(startPoint); // close last triangle
    if (lod == 0) {
        m_vertexBuffer->addTriangle(junction, startPoint, endPoint);
        return endPoint;
    }
    while(steps--) {
        qreal a0 = endAngle - (steps + 1.0) * step;
        qreal a1 = endAngle - (qreal)steps * step;
        QPointF sp = center + QPointF(-radius * sin(a0), radius * cos(a0));
        QPointF ep = center + QPointF(-radius * sin(a1), radius * cos(a1));
        m_vertexBuffer->addTriangle(junction, sp, ep);
    }
    return endPoint;
}

QPair<QPolygonF, QPolygonF> StrokeTriangulator::getRounding(const QPointF &center, qreal radius, bool leftTurn, qreal startAngle, qreal endAngle, qreal lod)
{
    QPair<QPolygonF, QPolygonF> ret;
    qreal sweep = endAngle - startAngle;
    if (sweep < -M_PI) {
        if (endAngle < 0)
            endAngle += M_PI * 2;
        sweep = endAngle - startAngle;
    }
    if (sweep > M_PI) {
        if (startAngle < 0)
            startAngle += 2 * M_PI;
        sweep = endAngle - startAngle;
    }
    if (!leftTurn) {
        startAngle += M_PI;
        endAngle = startAngle + sweep;
    }
    //    QPointF startPoint = center + QPointF(-radius * sin(startAngle), radius * cos(startAngle));
    qreal rl = radius - m_width / 2;
    qreal rr = radius + m_width / 2;
    QPointF endPointL = center + QPointF(-rl * sin(endAngle), rl * cos(endAngle));
    QPointF endPointR = center + QPointF(-rr * sin(endAngle), rr * cos(endAngle));
    int steps = qCeil(qAbs(sweep) * radius * lod / CURVE_THRESHOLD);
    qreal step = sweep / steps;
    int i = 0;
    while(i < steps) {
        qreal a1 = startAngle + (qreal)i * step;
        ret.first.append(center + QPointF(-rl * sin(a1), rl * cos(a1)));
        ret.second.append(center + QPointF(-rr * sin(a1), rr * cos(a1)));
        i++;

    }
    ret.first.append(endPointL);
    ret.second.append(endPointR);
    return ret;
}

void StrokeTriangulator::emitRoundCap(const QPointF &center, qreal angle, bool end, qreal lod)
{
    qreal dw = (m_widthRight - m_widthLeft) / 2;
    QPointF dp = QPointF(-dw * sin(angle), dw * cos(angle));
    QPointF cp = end ? center - dp : center + dp;
    qreal width = (m_widthLeft + m_widthRight) / 2;
    int steps = qCeil(M_PI * width * lod / CURVE_THRESHOLD);
    qreal step = M_PI / steps;
    qreal endAngle = angle - M_PI;
    while(steps--) {
        qreal a0 = endAngle - (steps + 1.0) * step;
        qreal a1 = endAngle - (qreal)steps * step;
        QPointF sp = cp + QPointF(-width * sin(a0), width * cos(a0));
        QPointF ep = cp + QPointF(-width * sin(a1), width * cos(a1));
        m_vertexBuffer->addTriangle(cp, sp, ep);
    }
}

void StrokeTriangulator::emitSquareCap(const QPointF &left, const QPointF &right, qreal angle)
{
    qreal width = qMax(m_widthLeft, m_widthRight);
    QPointF d = QPointF(-width * sin(angle + M_PI_2), width * cos(angle + M_PI_2));

    m_vertexBuffer->addTriangle(left, right, right + d);
    m_vertexBuffer->addTriangle(right + d, left + d, left);

}


QPair<QPolygonF, QPolygonF> StrokeTriangulator::_getOffsetVertices(const QPolygonF polygon, qreal lod)
{
    QPair<QPolygonF, QPolygonF> result;
    QVector2D prevNormal;
    qreal prevSlope;
    bool negOffset = m_offset > 0;
    qreal offset = qAbs(m_offset);
    for (int i = 0; i < polygon.size() - 1; ++i)
    {
        QPointF n0 = polygon.at(i);
        QPointF n1 = polygon.at(i + 1);
        QVector2D seg(n1 - n0);
        seg.normalize();
        QVector2D currNormal(seg.y(), -seg.x());
        qreal currSlope = atan2(n1.y() - n0.y(), n1.x() - n0.x());
        if (i == 0) {
            //            ret.append(n0 - (currNormal * m_offset).toPointF());
            if (negOffset) {
                result.first.append(n0 + (currNormal * (m_offset - m_width / 2)).toPointF());
                result.second.append(n0 + (currNormal * (m_offset + m_width / 2)).toPointF());
            } else {
                result.first.append(n0 + (currNormal * (m_offset + m_width / 2)).toPointF());
                result.second.append(n0 + (currNormal * (m_offset - m_width / 2)).toPointF());
            }
        }
        if (prevNormal.isNull()) {
            // first segment
            prevNormal = currNormal;
            prevSlope = currSlope;
        } else {
            // other segments
            qreal angle = (prevSlope - currSlope);
            bool left = true;
            if (angle < 0)
                angle += 2 * M_PI;
            if (angle > M_PI) {
                left = false;
            }
            if (negOffset != left) {
                auto r = getRounding(n0, offset, left, prevSlope, currSlope, lod);
                result.first << r.first;
                result.second << r.second;
            } else {
                qreal ll = (offset - m_width / 2) / cos(angle / 2);
                qreal lr = (offset + m_width / 2) / cos(angle / 2);
                result.first.append(n0 + ((prevNormal + currNormal).normalized() * ll).toPointF());
                result.second.append(n0 + ((prevNormal + currNormal).normalized() * lr).toPointF());
            }
            prevSlope = currSlope;
            prevNormal = currNormal;
            if (i == polygon.size() - 2) {
                // last segment
                if (negOffset) {
                    result.first.append(n1 + (currNormal * (m_offset - m_width / 2)).toPointF());
                    result.second.append(n1 + (currNormal * (m_offset + m_width / 2)).toPointF());
                } else {
                    result.first.append(n1 + (currNormal * (m_offset + m_width / 2)).toPointF());
                    result.second.append(n1 + (currNormal * (m_offset - m_width / 2)).toPointF());
                }
            }
        }
    }

    return result;
}



void StrokeTriangulator::setCapStyle(const Qt::PenCapStyle &capStyle)
{
    m_capStyle = capStyle;
}

QVector<QPointF> StrokeTriangulator::vertices() const
{
    return m_vertexBuffer->vertices();
}

QVector<uint> StrokeTriangulator::indices() const
{
    return m_vertexBuffer->fillIndices();
}



void StrokeTriangulator::setJoinStyle(const Qt::PenJoinStyle &joinStyle)
{
    m_joinStyle = joinStyle;
}



VertexBuffer *StrokeTriangulator::vertexBuffer() const
{
    return m_vertexBuffer;
}

void StrokeTriangulator::setVertexBuffer(VertexBuffer *vertexBuffer)
{
    m_vertexBuffer = vertexBuffer;
}




