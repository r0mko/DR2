#ifndef PATHRUNNER_H
#define PATHRUNNER_H
#include "guide_nodes.h"

#include <QVector2D>
#include <QtMath>
#include <QVariant>
#include <QLinkedList>
#include <QPolygonF>
#include "guideiterator.h"
class Guide;

qreal normalizeAngle(qreal angle);

class Runner : public GuideIterator
{
    Q_GADGET

    Q_PROPERTY(qreal position READ position WRITE setPosition)
    Q_PROPERTY(QPointF point READ point)
    Q_PROPERTY(bool atVertex READ atVertex)
    Q_PROPERTY(bool atStart READ atStart)
    Q_PROPERTY(bool atEnd READ atEnd)
    Q_PROPERTY(bool atPoint READ atPoint)
    Q_PROPERTY(bool valid READ valid)
    Q_PROPERTY(qreal angle READ angle)
    Q_PROPERTY(qreal turnAngle READ turnAngle)
    Q_PROPERTY(qreal vectorLengthAhead READ vectorLengthAhead)
    Q_PROPERTY(qreal vectorLengthBehind READ vectorLengthBehind)
//    Q_PROPERTY(QVariant currentNode READ currentNode)

    bool        m_atVertex = true;
    qreal       m_position = 0;
    qreal       nodePos = 0;
    QPointF     nodePoint;
    friend class Guide;
    friend class GuideSmoother;

public:
//    using GuideIterator::GuideIterator;
    Runner() : GuideIterator() {}
    Runner(GuideIterator iterator, QPointF point = QPointF());
    Runner(Guide path, qreal pos = 0);
    Runner(QLinkedListNode<GuideNode> *node, QPointF pos = QPointF());

    qreal position() const;
    void setPosition(qreal position);

    QPointF point() const;
    void setPoint(const QPointF &pt);

    inline bool atStart() const { return atHead() || (m_position == 0 && nodePos == 0 && prev()->isPoint()); }
    inline bool atEnd() const { return atTail(); }
    inline bool atVertex() const { return m_atVertex; }
    inline bool valid() const { return !isNull(); }

//    qreal nextAngularRatio(qreal legPos = NAN) const;
//    qreal prevAngularRatio(qreal legPos = NAN) const;
    qreal angle() const;
    QPointF normal() const;
    Q_INVOKABLE QPointF averageNormal(qreal range) const;
    Q_INVOKABLE QPointF offsetPoint(qreal distance) const;

    qreal dotProduct() const;
    qreal turnAngle(const VectorNode *previousVector = nullptr) const;
    float fastTurn(const VectorNode *other = nullptr) const;

    inline QPointF unitVector() const { if(!atVector()) return QPointF(); return vnode()->unitVector(); }
    QLineF segment() const;

    void toNearestVector();

    Q_INVOKABLE QVariant currentNode() const { return QVariant::fromValue(i->t); }

    Q_INVOKABLE qreal vectorLengthAhead() const;
    Q_INVOKABLE qreal vectorLengthBehind() const;

    Q_INVOKABLE qreal hopForward();
    Q_INVOKABLE qreal hopBack();

    Q_INVOKABLE qreal moveForward(qreal len);
    Q_INVOKABLE qreal moveBack(qreal len);

    Q_INVOKABLE qreal strictMoveForward(qreal len, GuideNode::NodeType nodeType = GuideNode::VectorType);
    Q_INVOKABLE qreal strictMoveBack(qreal len, GuideNode::NodeType nodeType = GuideNode::VectorType);

    Q_INVOKABLE QPolygonF negativePart(qreal length) const;
    Q_INVOKABLE QPolygonF positivePart(qreal length) const;

    QPointF roundingDelta(qreal range, QPointF *negPoint = nullptr, QPointF *posPoint = nullptr) const;
    Q_INVOKABLE QVariantMap debugRounding(qreal range) const;
protected:
    void nearMove(qreal dst);
    Guide windowBehind(qreal length, qreal *remaining) const;
    Guide windowAhead(qreal length, qreal *remaining) const;

};

Q_DECLARE_METATYPE(Runner)

#endif // PATHRUNNER_H
