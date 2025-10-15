#ifndef POLYLINERANGE_H
#define POLYLINERANGE_H
#include <QtMath>
#include <QPolygonF>
#include <QVector2D>

struct WireNode {
    QPointF pt;
    QVector2D normal;
    inline QPointF point(qreal offset) const { return pt + (normal * offset).toPointF(); }
};


class SpringWire : public QVector<WireNode>
{
    qreal m_previousAngle;
    mutable qreal preparedAngle;
    mutable const WireNode * preparedNode;
public:

    SpringWire() : m_previousAngle(NAN), preparedNode(nullptr) {}

    void prepareAppend(const WireNode &node, qreal *distance = nullptr, qreal *angleDelta = nullptr) const;
    void commitAppend();

    inline qreal previousAngle() const { return m_previousAngle; }
    QPolygonF toPolygonF() const;
};


class PolyLineRange
{
    struct PolNode {
        PolNode() : segLength(0), position(0), turnAngle(0)
        { }

        qreal   segLength;
        qreal   position;
        qreal   turnAngle;
        QVector2D   normal;
    };
public:
    PolyLineRange();
    void calculateRounded(qreal radius, int precision);
    void calculateCommonRounded(qreal radius, int precision);
    void calculateSimple();
    qreal length() const;
    QPointF nearestPathPoint(const QPointF &pos, qreal *length) const;
    SpringWire getRounding(qreal radius, int precision, qreal minAngleThreshold = 0.1, qreal maxAngleThreshold = 10);
    QPointF getCurvePoint(qreal position, qreal range, int order, bool *sameSegment = nullptr, int *startNode = nullptr, int *endNode = nullptr) const;
    QPointF pointAtLength(qreal pos, int *nodeNo = nullptr) const;
    QPointF pointAtPercent(qreal percent) const;
    qreal angleAtLength(qreal pos, int *nodeNo = nullptr) const;
    QPolygonF getPolylinePart(qreal startLength, qreal endLength) const;
    QPolygonF polygon() const;
    QPolygonF calculateOffset(const SpringWire &wire, qreal offset);
    void setPolygon(const QPolygonF &polygon);

    SpringWire wire() const;

protected:
    void init();
    QPolygonF           m_polygon;
    qreal               m_length;
    QVector<PolNode>    nodes;
    SpringWire          m_wire;
};

#endif // POLYLINERANGE_H
