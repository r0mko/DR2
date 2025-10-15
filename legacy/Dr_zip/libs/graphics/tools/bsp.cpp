#include "bsp.h"

LineIntersectionType intersects(const QLine &one, const QLine &other, QPointF *intersectionPoint)
{
    // ipmlementation is based on Graphics Gems III's "Faster Line Segment Intersection"
    QPoint pt1 = one.p1();
    QPoint pt2 = one.p2();
    const QPoint a = pt2 - pt1;
    const QPoint b = other.p2() - other.p1();
    const QPoint c = pt1 - other.p2();

    const float denominator = (float)a.y() * b.x() - (float)a.x() * b.y();
    if (denominator == 0)
        return NoIntersection;

    const float reciprocal = 1.0 / denominator;
    const float na = ((float)b.y() * c.x() - (float)b.x() * c.y()) * reciprocal;
    if (intersectionPoint)
        *intersectionPoint = QPointF(pt1) + QPointF(a) * na;

    if (na < 0 || na > 1)
        return UnboundedIntersection;

    const float nb = ((float)a.x() * c.y() - (float)a.y() * c.x()) * reciprocal;
    if (nb < 0 || nb > 1)
        return UnboundedIntersection;

    return BoundedIntersection;
}

RectIntersectionType lineRectIntersection(const QRect &rect, const QLine &line)
{
    QLine t = QLine(rect.topLeft(), rect.topRight());
    QLine l = QLine(rect.topLeft(), rect.bottomLeft());
    QLine b = QLine(rect.bottomLeft(), rect.bottomRight());
    QLine r = QLine(rect.topRight(), rect.bottomRight());
    if (rect.contains(line.p1()) && rect.contains(line.p2()))
        return Contains;
    int result = None;
    if (intersects(t, line) == BoundedIntersection)
        result |= TopSide;
    if (intersects(b, line) == BoundedIntersection)
        result |= BottomSide;
    if (intersects(l, line) == BoundedIntersection)
        result |= LeftSide;
    if (intersects(r, line) == BoundedIntersection)
        result |= RightSide;
    return (RectIntersectionType)result;
}


