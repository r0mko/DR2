#ifndef VNODE_H
#define VNODE_H

#include <QLineF>
#include <QPointF>
#include <QVariant>
#include <QVector2D>
#include <qdatastream.h>
#include <QDebug>
#include <QtMath>
class GuideNode
{
    Q_GADGET
    Q_PROPERTY(NodeType type READ type)
    Q_ENUMS(NodeType)

    friend QDataStream &operator<<(QDataStream &out, const GuideNode &p);
    friend QDataStream &operator>>(QDataStream &in, GuideNode &p);

public:
    GuideNode() : m_type(InvalidNode) {}
    enum NodeType {
        InvalidNode =       0x0,
        VectorType =        0x1,
        PointType =         0x2,
        HeadType =          0x4 + PointType,
        TailType =          0x8 + PointType,
        InterlinkType =     0x10 + VectorType,
        JunctionType =      0x20 + VectorType,
        ConvergenceType =   0x40 + VectorType

    };
    Q_ENUM(NodeType)
    NodeType type() const { return m_type; }
    inline bool isVector() const { return m_type & VectorType; }
    inline bool isPoint() const { return m_type & PointType; }

    inline bool operator==(const GuideNode &other) const { return s == other.s && v == other.v && m_type == other.m_type; }
protected:
    friend class Guide;
    GuideNode(NodeType type) : m_type(type) {}
    double s = 0;
    QPointF v;
    NodeType m_type;
};



class VectorNode : public GuideNode
{
    Q_GADGET

    Q_PROPERTY(QPointF unitVector READ unitVector)
    Q_PROPERTY(qreal length READ length)

    friend class Guide;
    friend class Wavefront;
    friend class Runner;

public:

    VectorNode() : GuideNode(VectorType) {}
    VectorNode(QPointF unitVector, qreal length): VectorNode() { v = unitVector; s = length; }
    VectorNode(const QLineF &line);
    VectorNode(const QPointF &p1, const QPointF &p2);

    inline QVector2D unitVectorF() const { return QVector2D(v); }
    inline QPointF unitVector() const { return v; }
    inline QVector2D vectorF() const { return QVector2D(v * s); }
    inline QPointF vector() const { return v * s; }
    inline double length() const { return s; }
    inline QVector2D normalF() const { return QVector2D(normal()); }
    inline QPointF normal() const { QPointF nv(v); qSwap(nv.rx(), nv.ry() *= -1); return nv;}
    inline QPointF offsetPoint(const QPointF &from, qreal distance) const { return from + (normal() * distance); }
    float fastAngleTo(const VectorNode &other) const;

protected:
    using GuideNode::GuideNode;
    inline void setUnitVector(const QPointF &unitVector) { v = unitVector; }
    double &lref() { return s; }
};

class VertexNode : public GuideNode
{
    Q_GADGET
    Q_PROPERTY(QPointF point READ point WRITE setPoint)
    Q_PROPERTY(qreal position READ position WRITE setPosition)

public:
    using GuideNode::GuideNode;
    inline QPointF point() const { return v; }
    inline QPointF &pointRef() { return v; }
    inline void setPoint(const QPointF &point) { v = point; }
    inline qreal position() const { return s; }
    void setPosition(qreal position) { s = position; }
};

class HeadNode : public VertexNode
{
    Q_GADGET
public:
    HeadNode() : VertexNode(HeadType) {}
};

class TailNode : public VertexNode
{
    Q_GADGET
public:
    TailNode() : VertexNode(TailType) {}

};
class InterlinkNode : public VectorNode
{
    Q_GADGET
public:
    inline InterlinkNode(QPointF pt1, QPointF pt2) : VectorNode(InterlinkType) {
        QLineF line(pt1, pt2);
        if (pt1 != pt2) {
            QLineF uv = line.unitVector();
            s = line.length();
            setUnitVector(-QPointF(uv.dx(), uv.dy()));
        }
//        if (l > 5)
//            qDebug() << "interlink length" << l;

    }
    InterlinkNode() : VectorNode(InterlinkType) {}
};

class JunctionNode : public VectorNode
{
    Q_GADGET
public:
    JunctionNode() : VectorNode(JunctionType) {}
};

class ConvergenceNode : public VectorNode
{
    Q_GADGET
public:
    ConvergenceNode() : VectorNode(ConvergenceType) {}
};

template <typename T> inline int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

float norm_atan2( float y, float x );

float fixNormAngle(float angle);

float angleBetweenVectors(const QVector2D &vecb, const QVector2D &vecf);

float invSqrt(float x);

qreal bisectorFactor(const VectorNode *vectorNode, const VectorNode *prevVectorNode);

QVector2D unitBisector(const VectorNode *vectorNode, const VectorNode *prevVectorNode);

inline QDataStream &operator<<(QDataStream &out, const GuideNode &p)
{
    QVariant v; // FIXME: compatibility kludge
    out << p.m_type << p.v << p.s << v;
    return out;
}

inline QDataStream &operator>>(QDataStream &in, GuideNode &p)
{
    int t;
    in >> t;
    p.m_type = (GuideNode::NodeType)t;
    in >> p.v;
    in >> p.s;
    QVariant v; // FIXME: compatibility kludge
    in >> v;
    return in;
}


Q_DECLARE_METATYPE(GuideNode)
Q_DECLARE_METATYPE(VectorNode)
Q_DECLARE_METATYPE(VertexNode)
Q_DECLARE_METATYPE(HeadNode)
Q_DECLARE_METATYPE(TailNode)
Q_DECLARE_METATYPE(InterlinkNode)
Q_DECLARE_METATYPE(JunctionNode)
Q_DECLARE_METATYPE(ConvergenceNode)

#endif // VNODE_H



