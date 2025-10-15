#ifndef VNODE_H
#define VNODE_H

#include <QLineF>
#include <QPointF>
#include <QVariant>
#include <QVector2D>

class VectorNode
{
    Q_GADGET

    friend class GuidePath;
    friend class PathRunner;
public:
    enum NodeType {
        Vector = 0x1,
        Node = 0x2,
        Start = 0x12,
        Finish = 0x22
    };
    Q_ENUM(NodeType)
    VectorNode() : m_type(Vector) {}
    VectorNode(QPointF unitVector, qreal length) : m_unitVector(unitVector), m_length(length) { }
    VectorNode(const QLineF &l);
    VectorNode(const QPointF &p1, const QPointF &p2);

    inline QPointF unitVector() const { return m_unitVector; }

    qreal length() const { return m_length; }

    NodeType type() const { return m_type; }
    inline bool isVector() const { return m_type & Vector; }
    inline bool isNode() const { return m_type & Node; }

//    inline QPointF pointAtLength(qreal length) const { return m_point + (m_unitVector * length).toPointF(); }
    inline QPointF normal() const { QPointF nv(m_unitVector); qSwap(nv.rx(), nv.ry() *= -1); return nv;}
    inline QPointF offsetPoint(const QPointF &from, qreal distance) const { return from + (normal() * distance); }
    inline QPointF vector() const { return m_unitVector * m_length; }
    inline QPointF endPoint(const QPointF &from) const { return from + (m_unitVector * m_length); }
    inline QLineF segment(const QPointF &from) const { return QLineF(from, endPoint(from)); }

protected:
    VectorNode(NodeType type) : m_type(type) {}
    QPointF m_unitVector;
    union { double m_length = 0; void *d; };
    NodeType m_type = Vector;
    inline void setUnitVector(const QPointF &unitVector) { m_unitVector = unitVector; }
    void setLength(const qreal &length) { m_length = length; }
};

class VertexNode : public VectorNode
{
public:
    inline QPointF point() const { return m_unitVector; }
    inline QPointF &pointRef() { return m_unitVector; }
    inline void setPoint(const QPointF &point) { m_unitVector = point; }
protected:
    using VectorNode::VectorNode;
};

class HeadNode : public VertexNode
{
public:
    HeadNode() : VertexNode(Finish) {}
};

class StartNode : public VertexNode
{
public:
    StartNode() : VertexNode(Start) {}

};

//inline bool operator ==(const VNode &arg1, const VNode &arg2)
//{
//    return arg1.m_length == arg2.m_length && arg1.m_unitVector == arg2.m_unitVector;
//}

#endif // VNODE_H
