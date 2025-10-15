#ifndef VECTORPATH_H
#define VECTORPATH_H

#include <QObject>
#include <QPolygonF>
#include <QVector2D>
#include <QtMath>
#include <QVariant>
#include <QLineF>

class VectorPath
{
    friend class PathSplitter;
public:
    struct VNode {
        VNode *prev;
        VNode *next;
        QPointF point;
        QVector2D unitVector;
        qreal sideLength;
        QVariant data;
        VNode() :
            prev(nullptr),
            next(nullptr),
            sideLength(0)
        {}
        inline QPointF pointAtLength(qreal sideLength) const;
        inline QVector2D prevUnit() const;
        inline qreal prevLength() const;
        inline qreal turn() const;
        inline QVector2D normal() const { return QVector2D(-unitVector.y(), unitVector.x()); }
        inline QPointF offsetPoint(qreal distance) const { return point + (normal() * distance).toPointF(); }
        inline QVector2D vector() const { return unitVector * sideLength; }
        inline QPointF endPoint() const { return point + (unitVector * sideLength).toPointF(); }
        inline QLineF segment() const { return QLineF(point, endPoint()); }
        qreal setLength(qreal sideLength);
        void shrink(qreal d);
        void extend(qreal d);
        inline void setNext(VNode *other);
        inline void setPrev(VNode *other);
        inline void resetNext();
        inline void resetPrev();
        inline bool isConnectedTo(const VNode *other) const;
        static void link(VNode *left, VNode *right);
    };

    VectorPath();
    ~VectorPath();
    friend class Runner;
    friend class link;

    class link
    {
        VNode *node1;
        VNode *node2;
        VNode oldNode1;
        VNode oldNode2;
        friend class VectorPath;
    public:
        link() {}
        ~link();
    private:

        link(VNode* node1, VNode *node2);
    };

    class Runner
    {
    public:
        Runner() :
            m_node(nullptr),
            m_position(NAN),
            node_pos(NAN),
            m_atStart(false),
            m_atEnd(false),
            m_atNode(false)
        {}
        Runner(const Runner &other) :
            m_node(other.m_node),
            m_position(other.m_position),
            node_pos(other.node_pos),
            m_atStart(other.m_atStart),
            m_atEnd(other.m_atEnd),
            m_atNode(other.m_atNode)
        {}
        friend class VectorPath;
        inline bool atStart() const;
        inline bool atEnd() const;
        inline bool atNode() const;
        inline bool isNull() const;
        inline qreal position() const;
        inline QPointF point() const;
        inline qreal nextAngularRatio(qreal legPos = NAN) const;
        inline qreal prevAngularRatio(qreal legPos = NAN) const;
        inline qreal angle() const;
        inline QVector2D normal() const;
        inline bool hasNext() const;
        inline bool hasPrev() const;
        QVariant data() const;
        QVariant nextData() const;
        QVariant prevData() const;
        QVariant *dataPtr() const;
        void setData(const QVariant &data);

        qreal moveBy(qreal len);
        qreal straightLengthAhead(qreal chamferRadius, qreal *ratio = nullptr) const;
        qreal straightLengthBehind(qreal chamferRadius, qreal *ratio = nullptr) const;
        QPointF getSmoothedPoint(qreal range) const;

        QVector2D toPrevNode(qreal travelLimit = 1e+127, qreal *factTravel = nullptr);
        QVector2D toNextNode(qreal travelLimit = 1e+127, qreal *factTravel = nullptr);

        inline qreal lengthAhead() const;
        inline qreal lengthBehind() const;




    private:
        friend class GrooveRunner;
        Runner (VectorPath::VNode *node, qreal pos);
        VNode *m_node;
        qreal m_position;
        qreal node_pos;

        bool m_atStart:1;
        bool m_atEnd:1;
        bool m_atNode:1;

        qreal moveForward(qreal len);
        qreal moveBack(qreal len);
    };

    VectorPath getPart(qreal startPos, qreal endPos) const;
    VectorPath split(qreal pos);
    VectorPath reversed() const;
    Runner split(qreal pos, VectorPath *left, VectorPath *right) const;
    VectorPath buildRounding(qreal radius, qreal step, qreal startPos = 0, qreal endPos = 1e+127) const;
    VectorPath buildOffset(qreal offset, bool dataAsNormal = false) const;

    inline Runner setRunner(qreal pos) const { return Runner(const_cast<VNode*>(&nodes.first()), pos); }
    link attachTail(VectorPath &other);
    link attachHead(VectorPath &other);
    inline bool isNull() const { return nodes.isEmpty(); }
    inline qreal length() const { return m_length; }
    inline int size() const { return nodes.size(); }
    inline int count() const { return nodes.size(); }
    inline QLineF segment(int index) const { return nodes[index].segment(); }

    QPointF pointAtLength(qreal pos, int *legIndex = nullptr) const;

    void resize(int n);

    void addPoint(const QPointF &point, QVariant nodeData = QVariant());
    void addPolygon(const QPolygonF &polygon, QVariant nodeData = QVariant());
    void addVectorPath(const VectorPath &other, QVariant data = QVariant());

    QPolygonF toPolygon() const;
    QPointF nearestPoint(const QPointF &pos, qreal *length) const;
    void clear();
    void dumpNodes() const;
    qreal checkLength();
    const VNode *head() const;
    const VNode *tail() const;
    const VNode *nodeAt(int index) const;
//    const QList<VNode> &nodes()

private:
    friend class Groove;

    void appendNode(const VNode &node);
    void prependNode(const VNode &node);
    VNode takeLastNode();
    VNode takeFirstNode();

    QList<VNode> nodes;
    qreal m_length;
    VNode lastNode;
};

qreal VectorPath::Runner::position() const
{
    return m_position;
}

QPointF VectorPath::Runner::point() const
{
    if (m_node)
        return m_node->pointAtLength(node_pos);
    return QPointF();
}

#include <QDebug>
qreal VectorPath::Runner::prevAngularRatio(qreal legPos) const
{
    Q_UNUSED(legPos)
    if (!m_node->prev)
        return 0;
    qreal dp = QVector2D::dotProduct(m_node->prev->unitVector, m_node->unitVector);
    return qSqrt(1-dp);

}
qreal VectorPath::Runner::nextAngularRatio(qreal legPos) const
{
    Q_UNUSED(legPos)
    if (!m_node->next)
        return 0;
    qreal dp = QVector2D::dotProduct(m_node->unitVector, m_node->next->unitVector);
    return qSqrt(1-dp);
}

qreal VectorPath::Runner::angle() const
{
    const QVector2D &v = m_node->unitVector;
    return qAtan2(v.y(), v.x()) * 180 / M_PI;
}

QVector2D VectorPath::Runner::normal() const
{
    return m_node->normal();

}

bool VectorPath::Runner::hasNext() const
{
    return m_node->next != nullptr;
}

bool VectorPath::Runner::hasPrev() const
{
    if( m_node == nullptr ) qDebug() << "no node !!!??";
    return m_node->prev != nullptr;

}

qreal VectorPath::Runner::lengthAhead() const
{
    return m_node->sideLength - node_pos;
}

qreal VectorPath::Runner::lengthBehind() const
{
    if (m_atNode && m_node->prev)
        return m_node->prev->sideLength;
    return node_pos;
}

bool VectorPath::Runner::atEnd() const
{
    return m_atEnd;
}

bool VectorPath::Runner::atNode() const
{
    return m_atNode;
}

bool VectorPath::Runner::isNull() const
{
    return m_node == nullptr;
}

bool VectorPath::Runner::atStart() const
{
    return m_atStart;
}

QPointF VectorPath::VNode::pointAtLength(qreal length) const
{
    return point + (unitVector * length).toPointF();
}

QVector2D VectorPath::VNode::prevUnit() const
{
    if (!prev)
        return QVector2D();
    return -prev->unitVector;
}

qreal VectorPath::VNode::prevLength() const {
    if (!prev)
        return NAN;
    return -prev->sideLength;
}

qreal VectorPath::VNode::turn() const
{
    if (!next)
        return 0;
    qreal dp = QVector2D::dotProduct(unitVector, next->unitVector);
    return sqrt((1 - dp) / (1 + dp));
}

void VectorPath::VNode::setNext(VectorPath::VNode *other)
{
    if (other)
        other->prev = this;
    next = other;
}

void VectorPath::VNode::setPrev(VectorPath::VNode *other)
{
    if (other)
        other->next = this;
    prev = other;
}

void VectorPath::VNode::resetNext()
{
    if (next) {
        next->prev = nullptr;
        next = nullptr;
    }
}

void VectorPath::VNode::resetPrev()
{
    if (prev) {
        prev->next = nullptr;
        prev = nullptr;
    }
}

bool VectorPath::VNode::isConnectedTo(const VNode * other) const
{
    if (other->next == this && prev == other)
        return true;
    if (other->prev == this && next == other)
        return true;
    return false;
}

inline bool operator ==(const VectorPath::VNode &arg1, const VectorPath::VNode &arg2)
{
    return arg1.point == arg2.point && arg1.unitVector == arg2.unitVector;
}
#endif // VECTORPATH_H
