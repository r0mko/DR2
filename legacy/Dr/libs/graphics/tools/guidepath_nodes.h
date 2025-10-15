#ifndef VNODE_H
#define VNODE_H

#include <QLineF>
#include <QPointF>
#include <QVariant>
#include <QVector2D>
#include <qdatastream.h>

class PathNode
{
    Q_GADGET
    Q_PROPERTY(NodeType type READ type)
    friend QDataStream &operator<<(QDataStream &out, const PathNode &p);
    friend QDataStream &operator>>(QDataStream &in, PathNode &p);
public:
    PathNode() : m_type(InvalidNode) {}
    enum NodeType {
        InvalidNode = 0x0,
        VectorType = 0x1,
        PointType = 0x2,
        HeadType = 0x6,
        TailType = 0xa

    };
    Q_ENUM(NodeType)
    NodeType type() const { return m_type; }
    inline bool isVector() const { return m_type & VectorType; }
    inline bool isPoint() const { return m_type & PointType; }
    inline QVariant userData() const { return m_userData; }
    inline void setUserData(QVariant data) { m_userData = data; }
protected:
    PathNode(NodeType type) : m_type(type) {}
    QPointF pt;
    double l = 0;
    QVariant m_userData;
    NodeType m_type;
};


class VectorNode : public PathNode
{
    Q_GADGET
    Q_PROPERTY(QPointF unitVector READ unitVector)
    friend class GuidePath;
    friend class PathRunner;
public:
    VectorNode() : PathNode(VectorType) {}
    VectorNode(QPointF unitVector, qreal length): VectorNode() { pt = unitVector; l = length; }
    VectorNode(const QLineF &l);
    VectorNode(const QPointF &p1, const QPointF &p2);

    inline QPointF unitVector() const { return pt; }
    inline QPointF vector() const { return pt * l; }
    inline double length() const { return l; }
    inline QPointF normal() const { QPointF nv(pt); qSwap(nv.rx(), nv.ry() *= -1); return nv;}
    inline QPointF offsetPoint(const QPointF &from, qreal distance) const { return from + (normal() * distance); }
    float fastAngleTo(const VectorNode &other) const;

protected:
    inline void setUnitVector(const QPointF &unitVector) { pt = unitVector; }
    void setLength(const qreal &length) { l = length; }
};

class VertexNode : public PathNode
{
    Q_GADGET
    Q_PROPERTY(QPointF point READ point WRITE setPoint)
    Q_PROPERTY(qreal position READ position WRITE setPosition)
    Q_PROPERTY(QVariant userData READ userData WRITE setUserData)

public:
    using PathNode::PathNode;
    inline QPointF point() const { return pt; }
    inline QPointF &pointRef() { return pt; }
    inline void setPoint(const QPointF &point) { pt = point; }
    inline qreal position() const { return l; }
    void setPosition(qreal position) { l = position; }

};

class HeadNode : public VertexNode
{
public:
    HeadNode() : VertexNode(TailType) {}
};

class TailNode : public VertexNode
{
public:
    TailNode() : VertexNode(HeadType) {}

};
Q_DECLARE_METATYPE(VectorNode)
Q_DECLARE_METATYPE(VertexNode)
Q_DECLARE_METATYPE(HeadNode)
Q_DECLARE_METATYPE(TailNode)

//inline bool operator ==(const VNode &arg1, const VNode &arg2)
//{
//    return arg1.m_length == arg2.m_length && arg1.m_unitVector == arg2.m_unitVector;
//}
inline QDataStream &operator<<(QDataStream &out, const PathNode &p)
{
    out << p.m_type << p.pt << p.l << p.m_userData;
    return out;
}
inline QDataStream &operator>>(QDataStream &in, PathNode &p)
{
    int t;
    in >> t;
    p.m_type = (PathNode::NodeType)t;
    in >> p.pt;
    in >> p.l;
    in >> p.m_userData;
    return in;
}
#endif // VNODE_H
