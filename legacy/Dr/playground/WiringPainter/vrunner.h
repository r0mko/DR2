#ifndef VRUNNER_H
#define VRUNNER_H
#include "vnode.h"

#include <QVector2D>
#include <QtMath>
#include <QVariant>
#include <QLinkedList>

class VRunner : public QLinkedList<VNode*>::iterator
{
public:
    VRunner(QLinkedList<VNode*>::iterator o) : QLinkedList<VNode*>::iterator(o) {}
    VRunner(QLinkedList<VNode*>::const_iterator o) : QLinkedList<VNode*>::iterator(o.i) {}
    VRunner() :
//        m_node(nullptr),
        m_position(NAN),
        nodePos(NAN),
        m_atStart(false),
        m_atEnd(false),
        m_atNode(false)
    {}
//    Runner(const Runner &other) :
//        m_node(other.m_node),
//        m_position(other.m_position),
//        node_pos(other.node_pos),
//        m_atStart(other.m_atStart),
//        m_atEnd(other.m_atEnd),
//        m_atNode(other.m_atNode)
//    {}
    friend class VectorPath;
    bool atStart() const
    {
        return m_atStart;
    }
    bool atEnd() const
    {
        return m_atEnd;
    }
    bool atNode() const
    {
        return m_atNode;
    }
    bool isNull() const
    {
        return i->t == nullptr;
    }
    qreal position() const
    {
        return m_position;
    }
    QPointF point() const
    {
        if (i->t)
            return nodePoint + i->t->unitVector() * nodePos;
        return QPointF();
    }
//    qreal nextAngularRatio(qreal legPos = NAN) const;
//    qreal prevAngularRatio(qreal legPos = NAN) const;
    qreal angle() const
    {
        const QVector2D &v = i->t->unitVector();
        return qAtan2(v.y(), v.x()) * 180 / M_PI;
    }
    QVector2D normal() const
    {
        return i->t->normal();

    }
    bool hasNext() const
    {
        return i->n->t->type() == VNode::Vector;
    }
    bool hasPrev() const
    {
        return i->p->t->type() == VNode::Vector;
    }
    QVariant data() const;
    QVariant nextData() const;
    QVariant prevData() const;
    void setData(const QVariant &data);

    qreal moveBy(qreal len);
    qreal straightLengthAhead(qreal chamferRadius, qreal *ratio = nullptr) const;
    qreal straightLengthBehind(qreal chamferRadius, qreal *ratio = nullptr) const;
    QPointF getSmoothedPoint(qreal range) const;

    QVector2D toPrevNode(qreal travelLimit = 1e+127, qreal *factTravel = nullptr);
    QVector2D toNextNode(qreal travelLimit = 1e+127, qreal *factTravel = nullptr);

    qreal lengthAhead() const
    {
        return i->t->length() - nodePos;
    }
    qreal lengthBehind() const
    {
        if (m_atNode && hasPrev())
            return i->p->t->length();
        return nodePos;
    }


private:
    friend class VChain;
    VRunner (VNode *node, qreal pos);

    QPointF nodePoint;
    qreal m_position = -1;
    qreal nodePos = -1;
    bool m_atStart = false;
    bool m_atEnd = false;
    bool m_atNode = false;

    qreal moveForward(qreal len);
    qreal moveBack(qreal len);
};


#endif // VRUNNER_H
