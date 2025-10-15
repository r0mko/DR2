#include "curl.h"
#include <QLineF>
#include "../../libs/Graphics/polygon.h"
#include "../../libs/Graphics/graphicspath.h"
#include <QDebug>
#include <QVector2D>
Curl::Curl(QObject *parent) :
    QObject(parent),
    m_polygon(nullptr),
    m_path(nullptr)
{
    pts.reserve(2048);
}

inline QPointF segPointAtPercent(const QPointF &p1, const QPointF &p2, qreal percent)
{
    return p1 + percent * (p2 - p1);
}
inline QPointF segPointAtLength(const QPointF &p1, const QPointF &p2, qreal length)
{
    qreal l = QLineF(p1, p2).length();
    return segPointAtPercent(p1, p2, length / l);
}


QPointF Curl::currentPoint() const
{
    return m_currentPoint;
}

qreal Curl::length() const
{
    return m_length;
}

qreal Curl::currentPosition() const
{
    return m_currentPosition;
}

qreal Curl::currentSegmentLength() const
{
    QPolygonF p = getPolygon();
    if (cpoint + 1 >= p.size()) {
        return 0;
    }
    return QLineF(p.at(cpoint), p.at(cpoint+1)).length();
}

qreal Curl::currentSegmentPosition() const
{
    QPolygonF p = getPolygon();
    return QLineF(p.at(cpoint), m_currentPoint).length();
}

qreal Curl::nearestObstacle(qreal max) const
{
    QPolygonF p = getPolygon();
    QLineF pr = QLineF(p.at(cpoint), m_currentPoint);
    QLineF nx = QLineF(p.at(cpoint + 1), m_currentPoint);
    return qMin(qMin(pr.length(), nx.length()), max);
}

int Curl::currentSegment() const
{
    return cpoint;
}

bool Curl::atEnd() const
{
    return m_atEnd;
}

GPolygon *Curl::polygon() const
{
    return m_polygon;
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

QPointF Curl::getTangent(qreal radius, int order, bool *onSameSegment, qreal *angle, qreal position)
{
    QPolygonF p = getPolygon();
    if (order < 2 || p.size() < 3)
        return m_currentPoint;
    save();
    rewind();
    qreal spos = s_m_currentPosition - radius;
    if (spos > 0)
        wind(spos);
    qreal epos = s_m_currentPosition + radius;
    qreal step = (2*radius) / (1 << order);
    qreal pos = spos;
    int cp = cpoint;
    pts.clear();
    bool singleSeg = true;
    while (pos <= epos) {
        if (pos < 0) {
            pts.append(p.first());
            if (step > qAbs(pos))
                wind(step + pos);
        } else if (pos > m_length) {
            pts.append(p.last());
        } else {
            pts.append(m_currentPoint);
            wind(step);
            if (cp != cpoint)
                singleSeg = false;
        }
        pos += step;
    }

    if (singleSeg) {
        if (onSameSegment)
            *onSameSegment = true;
        restore();
        return m_currentPoint;
    }

    if (onSameSegment)
        *onSameSegment = false;

    while (pts.size() > 2) {
        QVector<QPointF> rpts;
        rpts.reserve(pts.size() - 1);
        while (true) {
            QPointF p2 = pts.takeLast();
            if (pts.isEmpty())
                break;
            QPointF p1 = pts.last();
            rpts.append(segPointAtPercent(p1, p2, position));
        }
        pts.swap(rpts);
    }

    restore();
    qreal ang = QLineF(p.at(cpoint), p.at(cpoint + 1)).angle();
    QLineF s(pts.last(), pts.first());
    if (180 - qAbs(s.angle() - ang) < 90.0) {
        s = QLineF(pts.first(), pts.last());
    }
    if (angle)
        *angle = normalizeAngle(s.angle());

    qDebug() << qRound(10*normalizeAngle(s.angle() - ang))/10.0 << cpoint;

    return segPointAtPercent(s.p1(), s.p2(), position);
}

QPainterPath Curl::getPath(qreal smooth, qreal offset, int order, qreal curveThreshold)
{
    QPolygonF p = getPolygon();
    QPainterPath rp;
    QPainterPath ret;
    if (p.size() < 2)
        return ret;
    save();
    rewind();
    QVector2D nv;
    QLineF line;

    line.setP1(m_currentPoint);
    line.setP2(p.at(cpoint + 1));
    QLineF normLine = line.unitVector().normalVector();
    nv.setX(normLine.dx());
    nv.setY(normLine.dy());

    rp.moveTo(m_currentPoint);
    ret.moveTo((nv * offset).toPointF() + m_currentPoint);

    while (m_currentPosition < m_length && !m_atEnd) {
        qreal remLength = QLineF(m_currentPoint, p.at(cpoint + 1)).length();
        if (remLength > smooth) { // draw straight line to next corner rounding
            wind(remLength - smooth);
            line.setP1(rp.currentPosition());
            line.setP2(m_currentPoint);
            normLine = line.unitVector().normalVector();
            nv = QVector2D(normLine.dx(), normLine.dy());
            rp.lineTo(m_currentPoint);
//            ret.moveTo((nv * offset).toPointF() + m_currentPoint);
            ret.lineTo((nv * offset).toPointF() + m_currentPoint);
        }
        bool sameSeg = false;
        qreal d = 0;
        while (!sameSeg && !m_atEnd) {
            wind(curveThreshold);
//            wind(curveThreshold / (qAbs(d) + 1));
            line.setP1(rp.currentPosition());
            qreal angle;
            line.setP2(getTangent(smooth, order, &sameSeg, &angle));
            normLine = line.unitVector().normalVector();
            nv = QVector2D(normLine.dx(), normLine.dy());
            rp.lineTo(line.p2());
//            qDebug() << angle << curveThreshold / qAbs(angle) * offset / 10;
            d = angle;
//            ret.lineTo((nv * offset).toPointF() + line.p2());
            ret.lineTo((nv * offset).toPointF() + line.p2());
//            qDebug() << "Painting rounding" << m_currentPosition << angle << cpoint << rp.angleAtPercent(1.0);

        }
//        qDebug() << "Finished rounding" << m_currentPosition << m_length;
    }
//    qDebug() << "Finished stroke" << m_currentPosition << m_length;
    restore();
    return ret;
}

GraphicsPath *Curl::path() const
{
    return m_path;
}

void Curl::rewind()
{
    QPolygonF p = getPolygon();
    if (p.isEmpty()) {
        m_atEnd = true;
        return;
    }
    cpoint = 0;
    m_currentPoint = p.first();
    m_currentPosition = 0;
    m_atEnd = false;
}

void Curl::rewindToNode(int nodeIdx)
{
    QPolygonF p = getPolygon();
    if (nodeIdx >= p.size())
        return;
    qreal pos = 0;
    for (int i = 0; i < nodeIdx - 1; ++i) {
        const QPointF &p1 = p.at(i);
        const QPointF &p2 = p.at(i + 1);
        pos += QLineF(p1, p2).length();
    }
    m_currentPoint = p.at(nodeIdx);
    m_currentPosition = pos;
}

void Curl::wind(qreal length)
{
    QPolygonF p = getPolygon();
    if (cpoint + 1 >= p.size()) {
        m_atEnd = true;
        return;
    }
    do {
        const QPointF &pt = p.at(cpoint + 1);
        qreal segLen = sqrt(pow(pt.x() - m_currentPoint.x(), 2) + pow(pt.y() - m_currentPoint.y() , 2));
        if (length > segLen) {
            m_currentPoint = pt;
            //            m_currentSegmentLength = 0;
            m_currentPosition += segLen;
            length -= segLen;
            if ((cpoint + 2) < p.size()) {
                ++cpoint;
            } else {
                ++cpoint;
                m_atEnd = true;
                break;
            }
        } else {
            //            m_currentSegmentLength += length;
            m_currentPosition += length;
            m_currentPoint = segPointAtPercent(m_currentPoint, pt, length / segLen);
            break;
        }
    }
    while (true);
}



void Curl::save()
{
    s_cpoint = cpoint;
    s_m_currentPosition = m_currentPosition;
    s_m_currentPoint = m_currentPoint;
    s_m_atEnd = m_atEnd;
}

void Curl::restore()
{
    cpoint = s_cpoint;
    m_currentPosition = s_m_currentPosition;
    m_currentPoint = s_m_currentPoint;
    m_atEnd = s_m_atEnd;
}

void Curl::updatePath(qreal smooth, qreal offset, int order, qreal curveThreshold)
{
    writePath(getPath(smooth, offset, order, curveThreshold));
}

void Curl::setPolygon(GPolygon *arg)
{
    if (m_polygon == arg)
        return;
    m_polygon = arg;
    int pidx = m_polygon->metaObject()->indexOfProperty("polygon");
    polProp = m_polygon->metaObject()->property(pidx);
    emit polygonChanged(arg);
}

void Curl::setPath(GraphicsPath *arg)
{
    if (m_path == arg)
        return;

    m_path = arg;
    int pidx = m_path->metaObject()->indexOfProperty("path");
    pathProp = m_path->metaObject()->property(pidx);
    emit pathChanged(arg);
}

void Curl::init()
{
    rewind();
    QPolygonF p = getPolygon();
    m_length = 0;
    for (int i = 0; i < p.size() - 1; ++i) {
        const QPointF &p1 = p.at(i);
        const QPointF &p2 = p.at(i + 1);
        m_length += QLineF(p1, p2).length();
    }
}


QPolygonF Curl::getPolygon() const
{
    if (!polProp.isValid() || !m_polygon)
        return QPolygonF();
    return polProp.read(m_polygon).value<QPolygonF>();
}

void Curl::writePath(const QPainterPath &path) const
{
    if (!m_path)
        return;
    pathProp.write(m_path, QVariant::fromValue(path));
}
