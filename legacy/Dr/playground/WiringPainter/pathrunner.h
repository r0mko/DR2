#ifndef PATHRUNNER_H
#define PATHRUNNER_H
#include "pnode.h"
#include "vnode.h"

#include <QVector2D>
#include <QtMath>
#include <QVariant>
#include <QLinkedList>
class GuidePath;

qreal normalizeAngle(qreal angle);
class PathRunner : public QLinkedList<VectorNode>::iterator
{
    bool m_atVertex = true;
    qreal m_position = 0;
    qreal nodePos = 0;
    QPointF nodePoint;
    friend class GuidePath;
public:
    PathRunner();
    PathRunner(GuidePath *chain, qreal pos);
    inline VectorNode::NodeType currentNodeType() const { return (VectorNode::NodeType)i->t.type(); }
    PathRunner(QLinkedList<VectorNode>::iterator o) : QLinkedList<VectorNode>::iterator(o) {}
    PathRunner(QLinkedList<VectorNode>::const_iterator o) : QLinkedList<VectorNode>::iterator(o.i) {}
    bool atStart() const;
    bool atEnd() const;
    bool atNode() const;
    bool isNull() const;
    qreal position() const;
    void setPoint(const QPointF &pt);
    QPointF point() const;
//    qreal nextAngularRatio(qreal legPos = NAN) const;
//    qreal prevAngularRatio(qreal legPos = NAN) const;
    qreal angle() const;
    QPointF normal() const;

    qreal dotProduct() const;
    qreal turnAngle() const;

    bool hasNext() const;
    bool hasPrev() const;
    inline QPointF unitVector() const { if(!atVector()) return QPointF(); return node().unitVector(); }
    QLineF segment() const;
    qreal hopForward();

    void setAtNode(bool arg);

    inline bool atVector() const { return i->t.type() == VectorNode::Vector; }
    inline VectorNode next() const { return i->n->t; }
    inline VectorNode prev() const { return i->p->t; }
    inline VectorNode node() const { return i->t; }

    qreal moveForward(qreal len);
    qreal lengthAhead() const;
    qreal hopBack();
private:
    inline VertexNode pnode(const VectorNode &node) const { return *static_cast<const VertexNode*>(&node); }
    inline VertexNode pnode() const { return *static_cast<VertexNode*>(&i->t); }
    void nearMove(qreal dst);

    GuidePath negVP(qreal length, qreal *remaining) const;
    GuidePath posVP(qreal length, qreal *remaining) const;
    QPointF roundingDelta(qreal range, qreal *aheadCurvature, qreal *behindCurvature) const;
};


#endif // PATHRUNNER_H
