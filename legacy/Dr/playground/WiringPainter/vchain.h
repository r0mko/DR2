#ifndef VCHAIN_H
#define VCHAIN_H



#include <QObject>
#include <QPolygonF>
#include <QVector2D>
#include <QtMath>
#include <QVariant>
#include <QLineF>
#include "pathrunner.h"
//#include "vrunner.h"
#include "vnode.h"

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

class GuidePath
{

public:



    GuidePath();
    GuidePath(const QPolygonF &polygon) { init(); addPolygon(polygon); }
    ~GuidePath();
    friend class VRunner;
    friend class PathRunner;
//    VChain getPart(qreal startPos, qreal endPos) const;
//    VChain buildRounding(qreal radius, qreal step, qreal startPos = 0, qreal endPos = 1e+127) const;
//    VChain buildOffset(qreal offset, bool dataAsNormal = false) const;

//    inline VRunner setRunner(qreal pos) const { VRunner r(++nodes.begin()); r.moveForward(pos); return r; }
    inline PathRunner setRunner(qreal pos) const { PathRunner r(++nodes.begin()); r.moveForward(pos); return r; }
    inline bool isEmpty() const { return nodes.size() < 3; }
    inline qreal length() const { return m_length; }
    inline int size() const { return nodes.size(); }
    inline int count() const { return nodes.size() - 2; }

    void trim(const QLinkedList<VectorNode>::const_iterator &after);

//    inline QLineF segment(int index) const { return nodes[index].segment(); }

//    QPointF pointAtLength(qreal pos, int *legIndex = nullptr) const;

//    void resize(int n);

    void appendPoint(const QPointF &point);
    void addPolygon(const QPolygonF &polygon, QVariant nodeData = QVariant());
    QPolygonF roundedPolygon(qreal radius, qreal precision) const;
    GuidePath buildOffset(qreal offset) const;

    QPolygonF toPolygon() const;
    QPointF nearestPoint(const QPointF &pos, qreal *length) const;
    void clear();
    void dumpNodes() const;
    qreal checkLength();
    VectorNode &firstVectorNode();
    VectorNode &lastVectorNode();
    const VectorNode &firstVectorNode() const;
    const VectorNode &lastVectorNode() const;
    inline VertexNode &headNode() { return *static_cast<VertexNode*>(&nodes.first()); }
    inline VertexNode &tailNode() { return *static_cast<VertexNode*>(&nodes.last()); }
    inline const VertexNode &headNode() const { return *static_cast<const VertexNode*>(&nodes.first()); }
    inline const VertexNode &tailNode() const { return *static_cast<const VertexNode*>(&nodes.last()); }
    inline QLinkedList<VectorNode>::const_iterator head() const { return i_head; }
    inline QLinkedList<VectorNode>::const_iterator tail() const { return i_tail; }
    inline QLinkedList<VectorNode>::const_iterator first() const { return i_head + 1; }
    inline QLinkedList<VectorNode>::const_iterator last() const { return i_tail - 1; }
//    const VNode *nodeAt(int index) const;
//    const QList<VNode> &nodes()

//    VectorPath split(qreal pos);
//    VectorPath reversed() const;
    //    Runner split(qreal pos, VectorPath *left, VectorPath *right) const;
    qreal findNearestPos(const QPointF &pos, const QPointF &origin, bool reversed, qreal *distance);
    GuidePath rounded(qreal radius, qreal precision) const;
private:
    void init();
    friend class Groove;

    void appendNode(const VectorNode &node);
    void prependNode(const VectorNode &node);
    VectorNode takeLastNode();
    VectorNode takeFirstNode();
    QLinkedList<VectorNode> nodes;
//    QList<VNode> nodes;
    qreal m_length = 0;
    QLinkedList<VectorNode>::iterator i_head;
    QLinkedList<VectorNode>::iterator i_tail;

    QPointF weightedAverage(qreal range, const QPointF &origin);
};



#endif // VCHAIN_H
