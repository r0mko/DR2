#include "line.h"

Line::Line(QObject *parent) :
    QObject(parent)
{
}

QVariantMap Line::intersection(Line *other) const
{
    QVariantMap ret;
    QPointF p;
    QLineF::IntersectType i = line.intersect(other->line, &p);
    ret["type"] = i;
    ret["point"] = p;
    return ret;
}

void Line::setP1(QPointF arg)
{
    if (line.p1() == arg) return;
    line.setP1(arg);
    emit p1Changed(arg);
}

void Line::setP2(QPointF arg)
{
    if (line.p2() == arg) return;
    line.setP2(arg);
    emit p2Changed(arg);
}

void Line::setX1(qreal arg)
{
    if (line.x1() == arg) return;
    line.setP1(QPointF(arg, line.y1()));
    emit x1Changed(arg);
}

void Line::setY1(qreal arg)
{
    if (line.y1() == arg) return;
    line.setP1(QPointF(line.x1(), arg));
    emit y1Changed(arg);
}

void Line::setX2(qreal arg)
{
    if (line.x2() == arg) return;
    line.setP2(QPointF(arg, line.y2()));
    emit x2Changed(arg);
}

void Line::setY2(qreal arg)
{
    if (line.y2() == arg) return;
    line.setP2(QPointF(line.x2(), arg));
    emit y2Changed(arg);
}

void Line::setLength(qreal arg)
{
    if (line.length() == arg) return;
    line.setLength(arg);
    emit lengthChanged(arg);
}

void Line::setAngle(qreal arg)
{
    if (line.angle() == arg) return;
    line.setAngle(arg);
    emit angleChanged(arg);
}
