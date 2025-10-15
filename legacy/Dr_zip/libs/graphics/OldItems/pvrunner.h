#ifndef PVRUNNER_H
#define PVRUNNER_H

#include <QObject>
#include <QPointF>
#include <QtMath>
#include "pvnode.h"
#include "pvchain.h"
class GraphEdge;
class PVRunner
        //        : public QObject
{
    //    Q_OBJECT
    //    Q_PROPERTY(qreal position READ position WRITE setPosition NOTIFY positionChanged)

    //    Q_PROPERTY(GraphEdge* edge READ edge WRITE setEdge NOTIFY edgeChanged)

    //    Q_PROPERTY(bool atEnd READ atEnd NOTIFY atEndChanged)
    //    Q_PROPERTY(bool atStart READ atStart NOTIFY atStartChanged)
    //    Q_PROPERTY(bool atNode READ atNode NOTIFY atNodeChanged)

    //    Q_PROPERTY(QPointF point READ point NOTIFY pointChanged)
    //    Q_PROPERTY(qreal angle READ angle NOTIFY angleChanged)
    //    Q_PROPERTY(QPointF normal READ normal NOTIFY normalChanged)

    //    Q_PROPERTY(qreal lengthBehind READ lengthBehind NOTIFY lengthBehindChanged)
    //    Q_PROPERTY(qreal lengthAhead READ lengthAhead NOTIFY lengthAheadChanged)

    qreal m_position;
    PVChain m_edge;
    bool m_backwards;
    bool m_atEnd;
    bool m_atStart;
    bool m_atVertex;
    QPointF nodePoint;
    qreal nodePos;

    PVNode m_node;
public:
    PVRunner();
    ~PVRunner();

    inline qreal position() const;
    inline PVChain edge() const;
    inline PVChain *edgePtr() const;
    inline bool atEnd() const;
    inline bool atStart() const;
    inline bool atVertex() const;
    inline bool backwards() const;
    inline QPointF point() const;
    inline qreal angle() const;
    inline QPointF normal() const;
    inline qreal lengthBehind() const;
    inline qreal lengthAhead() const;

    void setEdge(PVChain arg, bool backwards = false);
    void setBackwards(bool back = true);
    void setPosition(qreal arg);
    QPointF toClosestPosition(const QPointF &pos);
    void moveToHead();
    void moveToTail();

    qreal moveBy(qreal len);
    qreal moveForward(qreal len);
    qreal moveBack(qreal len);

    QPointF backVector() const;
    QPointF forwardVector() const;
    QLineF segment() const;

    qreal hopForward();
    qreal hopBack();
    PVRunner &operator +=(qreal distance);
    PVRunner &operator -=(qreal distance);

private:
    PVNode aheadNode();
    PVNode behindNode();
    void nearMove(qreal dst);
    void _toNext();
    void _toPrev();
    void _toBegin();
    void _toEnd();
    //    void notifyChanges();
    //signals:
    //    void positionChanged();
    //    void edgeChanged();
    //    void atEndChanged();
    //    void atStartChanged();
    //    void atNodeChanged();
    //    void pointChanged();
    //    void angleChanged();
    //    void normalChanged();
    //    void lengthBehindChanged();
    //    void lengthAheadChanged();

};

qreal PVRunner::position() const
{
    return m_position;
}

PVChain PVRunner::edge() const
{
    return m_edge;
}

PVChain *PVRunner::edgePtr() const
{
    return const_cast<PVChain*>(&m_edge);
}

bool PVRunner::atEnd() const
{
    return m_atEnd;
}

bool PVRunner::atStart() const
{
    return m_atStart;
}

bool PVRunner::atVertex() const
{
    return m_atVertex;
}

bool PVRunner::backwards() const
{
    return m_backwards;
}

QPointF PVRunner::point() const
{
    if (!m_node.isNull())
        return nodePoint + m_node.unitVector() * nodePos;
    return QPointF();
}

qreal PVRunner::angle() const
{
    if (m_node.isNull())
        return 0;
    const QPointF &v = m_node.unitVector();
    return qAtan2(v.y(), v.x()) * 180 / M_PI;
}

QPointF PVRunner::normal() const
{
    return m_node.normal();
}

qreal PVRunner::lengthBehind() const
{
    if (!m_backwards) {
        if (m_atVertex) {
            if (m_node.hasPrev())
                return m_node.prev().length();
            else return 0;
        }
        return nodePos;
    }
    return m_node.length() - nodePos;

}

qreal PVRunner::lengthAhead() const
{
    if (m_backwards) {
        if (m_atVertex) {
            if (m_node.hasPrev())
                return m_node.prev().length();
            else return 0;
        }
        return nodePos;
    }
    return m_node.length() - nodePos;
}


#endif // PVRUNNER_H
