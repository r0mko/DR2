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
    emit p1Changed();
    emit x1Changed();
    emit y1Changed();
    emit angleChanged();
    emit lengthChanged();
}

void Line::setP2(QPointF arg)
{
    if (line.p2() == arg) return;
    line.setP2(arg);
    emit p2Changed();
    emit x2Changed();
    emit y2Changed();
    emit angleChanged();
    emit lengthChanged();
}

void Line::setX1(qreal arg)
{
    if (line.x1() == arg) return;
    line.setP1(QPointF(arg, line.y1()));
    emit x1Changed();
    emit p1Changed();
    emit angleChanged();
    emit lengthChanged();
}

void Line::setY1(qreal arg)
{
    if (line.y1() == arg) return;
    line.setP1(QPointF(line.x1(), arg));
    emit y1Changed();
    emit p1Changed();
    emit angleChanged();
    emit lengthChanged();
}

void Line::setX2(qreal arg)
{
    if (line.x2() == arg) return;
    line.setP2(QPointF(arg, line.y2()));
    emit x2Changed();
    emit p2Changed();
    emit angleChanged();
    emit lengthChanged();
}

void Line::setY2(qreal arg)
{
    if (line.y2() == arg) return;
    line.setP2(QPointF(line.x2(), arg));
    emit y2Changed();
    emit p2Changed();
    emit angleChanged();
    emit lengthChanged();
}

void Line::setLength(qreal arg)
{
    if (line.length() == arg) return;
    line.setLength(arg);
    emit p2Changed();
    emit x2Changed();
    emit y2Changed();
    emit lengthChanged();
}

void Line::setAngle(qreal arg)
{
    if (line.angle() == arg) return;
    line.setAngle(arg);
    emit angleChanged();
    emit p2Changed();
    emit x2Changed();
    emit y2Changed();
}
