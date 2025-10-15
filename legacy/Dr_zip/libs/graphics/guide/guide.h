#ifndef GUIDE_H
#define GUIDE_H
#include <QObject>
#include <QPolygonF>
#include <QVector2D>
#include <QtMath>
#include <QVariant>
#include <QLineF>
#include <QSharedData>
#include <QPainterPath>
#include <QJSValue>
#include <QJsonValue>
#include "runner.h"
#include <QDebug>
#include "guide_nodes.h"


class Guide
{
    Q_GADGET
    Q_PROPERTY(qreal length READ length)
    Q_PROPERTY(int count READ count)
    Q_PROPERTY(bool empty READ isEmpty)
    Q_PROPERTY(QPointF headPoint READ headPoint)
    Q_PROPERTY(QPointF tailPoint READ tailPoint)
    Q_PROPERTY(QJsonValue data READ data WRITE setData)

    struct GuideData : public QSharedData
    {
        qreal                            length = 0;
        QLinkedList<GuideNode>           nodes;
        GuideIterator                    iHead;
        GuideIterator                    iTail;
        QVariant                         pathData;

    };

    friend class Runner;
    friend class GuideSmoother;
    friend QDataStream &operator<<(QDataStream &out, const Guide &p);
    friend QDataStream &operator>>(QDataStream &in, Guide &p);

public:
    class Link
    {
        struct data : public QSharedData {
            ~data();
            QLinkedListNode<GuideNode> * from = nullptr;
            QLinkedListNode<GuideNode> * to = nullptr;
            QLinkedListNode<GuideNode> * oldTo = nullptr;
            QLinkedListNode<GuideNode> * oldFrom = nullptr;
            QLinkedListNode<GuideNode> connector { QLinkedListNode<GuideNode>(GuideNode()) };
        };
        friend class GuideNet;
        QExplicitlySharedDataPointer<data> d;
    public:
        Link();
        Link(QLinkedListNode<GuideNode> * from, QLinkedListNode<GuideNode> * to, VectorNode via = VectorNode());
        Link(GuideIterator &from, GuideIterator &to, VectorNode via = VectorNode());
        void lock();
        void updateConnector(GuideNode node);
        void unlock();
        bool unlocked() const;
        GuideIterator connector() const;

    };

    Guide();
    Guide(const Guide &other) : d (other.d) { }
    Guide(const QPolygonF &polygon) { init(); setPolygon(polygon); }
    ~Guide();

    Q_INVOKABLE inline Runner setRunner(qreal pos) const { return Runner(*this, pos); }
    Q_INVOKABLE inline Runner headRunner() const { return Runner(d->nodes.begin()); }
    Q_INVOKABLE inline Runner tailRunner() const { return Runner(d->iTail); }
//    Q_INVOKABLE

    inline bool isEmpty() const { return d->nodes.size() < 3; }
    inline bool isNull() const { return d == nullptr; }
    inline qreal length() const { return d->length; }

    inline int size() const { return d->nodes.size(); }
    inline int count() const { return d->nodes.size() - 2; }

    void trim(const GuideIterator &after);

    Link link();

    Q_INVOKABLE void appendPoint(const QPointF &point);
    Q_INVOKABLE void appendGuide(Guide other);
    Q_INVOKABLE void prependGuide(Guide other);

    qreal weight(const VectorNode &node) const;

    void appendNode(const VectorNode &node);
    VectorNode createNextNode(const QPointF &nextPt) const;

    Q_INVOKABLE void setPolygon(const QPolygonF &polygon);
    Q_INVOKABLE Guide clone();
    Q_INVOKABLE Guide part(qreal start, qreal end) const;
    Q_INVOKABLE QVariantList splitted(qreal position) const;
    Q_INVOKABLE void trimStart(qreal length);
    Q_INVOKABLE void trimEnd(qreal length);
    Q_INVOKABLE Runner findNearestPos(const QPointF &pos, qreal *distance = 0);
    Q_INVOKABLE void clear();
    Q_INVOKABLE Guide::Link linkPrev(Guide previous, GuideNode::NodeType type);
    Q_INVOKABLE Guide::Link linkNext(Guide next, GuideNode::NodeType type);

    Q_INVOKABLE QPainterPath wavefront_debug(qreal offset) const;
    Q_INVOKABLE Guide smoothed(qreal radius, qreal precision, qreal startPos = 0, qreal endPos = -1) const;
    Q_INVOKABLE Guide simplified(qreal threshold, qreal range) const;
    Q_INVOKABLE Guide reversed() const;
    Q_INVOKABLE Guide offseted(qreal offset, bool fast = false) const;
    Q_INVOKABLE QPainterPath outline(qreal width, Qt::PenCapStyle caps) const;

//    static QPainterPath createOutline(Guide left, Guide right, Qt::PenCapStyle caps);

    Q_INVOKABLE QPainterPath toPath() const;
    Q_INVOKABLE QPolygonF toPolygon() const;

    Q_INVOKABLE QRectF boundingRect() const;

    const VectorNode &firstVNode() const;
    VectorNode &firstVNode();
    const VectorNode &lastVNode() const;
    VectorNode &lastVNode();

    QPointF headPoint() const;
    QPointF tailPoint() const;

    Q_INVOKABLE VectorNode takeFirstVectorNode();
    Q_INVOKABLE VectorNode takeLastVectorNode();

    inline VertexNode &headNode() { return *static_cast<VertexNode*>(&d->nodes.first()); }
    inline VertexNode &tailNode() { return *static_cast<VertexNode*>(&d->nodes.last()); }

    Q_INVOKABLE inline const VertexNode &headNode() const { return *static_cast<const VertexNode*>(&d->nodes.first()); }
    Q_INVOKABLE inline const VertexNode &tailNode() const { return *static_cast<const VertexNode*>(&d->nodes.last()); }

    Q_INVOKABLE inline GuideIterator begin() const { return d->nodes.begin(); }
    Q_INVOKABLE inline GuideIterator head() const { return d->nodes.begin(); }
    Q_INVOKABLE inline GuideIterator first() const { return d->iHead; }

    Q_INVOKABLE inline GuideIterator last() const { return d->iTail - 1; }
    Q_INVOKABLE inline GuideIterator tail() const { return d->iTail; }
    Q_INVOKABLE inline GuideIterator end() const { return d->nodes.end(); }

    Q_INVOKABLE inline QVariant pathData() const { return d->pathData; }
    Q_INVOKABLE void setPathData(const QVariant &pathData);

    inline QJsonValue data() const { return QJsonValue::fromVariant(d->pathData); }
    void setData(const QJsonValue &pathData);



    void dumpNodes() const;

    inline bool operator ==(Guide other) const { return d == other.d; }
    inline bool operator !=(Guide other) const { return d != other.d; }
    QPointF weightedAverage(qreal range, const QPointF &origin);
    inline operator QVariant() const { return QVariant::fromValue(*this); }

public slots:

private:
    qreal dotToPoint(const QPointF &point) const;
    QExplicitlySharedDataPointer<GuideData> d;

    Guide(GuideData *data);
    void init();

    qreal checkLength();
    void prependNode(const VectorNode &node);

    QVariant m_userData;
};

QDataStream &operator<<(QDataStream &out, const Guide &p);
QDataStream &operator>>(QDataStream &in, Guide &p);

#endif // GUIDE_H
