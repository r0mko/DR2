#ifndef GUIDENETITEM_H
#define GUIDENETITEM_H

#include "visualinstance.h"

#include <gpen.h>
#include <guidenet.h>
#include <guide.h>

class GuideNet : public GraphicsObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList joints READ joints NOTIFY jointsChanged)
    Q_PROPERTY(QVariantList bounds READ bounds NOTIFY boundsChanged)
    Q_PROPERTY(QVariantList segments READ segments NOTIFY segmentsChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(GPen pen READ pen WRITE setPen)
    Q_PROPERTY(qreal precision READ precision WRITE setPrecision)
    Q_PROPERTY(qreal smooth READ smooth WRITE setSmooth)


//    QList<StartVertex>      m_entryPoints;
//    QList<EndVertex>        m_exitPoints;
//    QList<BoundaryVertex>   m_boundaries;
//    QList<JointVertex>      m_joints;
//    QList<JunctionVertex>   m_junctions;

    struct Batch
    {
        QVector<Guide::Link>    couplers;
        Runner                  entryRunner;
        qreal                       limit = 0;
        QVector<uint>               segments;
        NetVertex                   startElement;
        NetVertex                   endElement;
        Guide exec(qreal radius, qreal precision, qreal scale);
    };

    struct Edge
    {
        Edge() = default;
        Edge(uint source, uint destination, NetVertex vertex) : sourceUid(source), destUid(destination), vertex(vertex) {}
        uint        sourceUid = -1;
        uint        destUid = -1;
        NetVertex   vertex;

    };
    friend QDebug operator<<(QDebug, const GuideNet::Edge &);

    QHash<uint, NetSegment> m_segments;
    QMultiHash<uint, Edge>  links;

    QList<Batch>            batches;

    GPen                    m_pen;
    qreal                   m_precision = 0;
    qreal                   m_smooth = 0;

    QList<Guide>        paths;

//    QHash<uint, Batch*>      f_Entrys;
//    QSet<uint, Batch*>       f_Exits;

    QPainterPath            outline;
    QRectF                  m_boundingRect;
    bool                    debugNodes = false;

    NetSegment              pressedSegment;

    Q_CLASSINFO("DefaultProperty", "children")

    QVariantList findVertices(NetVertex::ElementType type) const;
    template <class T>
    QList<T> findVertices(NetVertex::ElementType type) const
    {
        QSet<T> ret;
        for (QHash<uint, Edge>::const_iterator ii = links.begin(); ii != links.end(); ++ii) {
            if (ii->vertex.type() == type)
                ret.insert(T::fromVertex(ii->vertex));
        }
    //    qDebug() << "Return" << ret.size() << "vertices of type" << type;
        return ret.toList();
    }
    QQmlListProperty<QObject> m_children;

public:
    GuideNet(QGraphicsItem *parent = nullptr);

    void componentComplete();
    void classBegin();

    QVariantList joints() const;
    QVariantList segments() const;
    Q_INVOKABLE NetSegment segment(uint uid) const;
    Q_INVOKABLE bool hasSegment(uint uid) const;

    StartVertex toStart(NetVertex other) const;
    EndVertex toEnd(NetVertex other) const;
    JointVertex toJoint(NetVertex other) const;

    QVariantList startVertices() const;
    QVariantList endVertices() const;

    QRectF boundingRect() const;
    void buildPaths(qreal scale);

    GPen pen() const;
    qreal precision() const;
    qreal smooth() const;

    void setPen(GPen pen);
    void setPrecision(qreal precision);
    void setSmooth(qreal smooth);

    void setupScene(Scene *scene);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPainterPath shape() const;

    Q_INVOKABLE Runner findNearestPoint(QPointF pos) const;
    Q_INVOKABLE NetSegment segmentAt(QPointF pos) const;

    QString name() const;
    void setName(QString name);

    QVariantList bounds() const;

public slots:

    void splitAtPoint(QPointF pos);
    void appendSegment(uint uid, QString name, Guide path, qreal entry, qreal exit, qreal offset);
    void removeSegment(uint uid);
    void addBoundary(NetSegment from, NetSegment to, bool isStart);
    void clearLinks();
    void clearSegments();
    void removeLink(uint from, uint to);
    void addJoint(uint uidFrom, uint uidTo);
    void addStart(uint uidTo);
    void addEnd(uint uidFrom);
    void updateParts();
    void build(qreal scale);
    void update();
    void setDebugNodes(bool enable);
    void pushSegment(NetSegment segment);
    void merge(GuideNet *other);
    void updateSegment(uint uid, qreal entryPos, qreal exitPos, qreal offset);

signals:
    void jointsChanged();
    void segmentChanged(QString name, qreal entryPos, qreal exitPos, qreal offset);
    void segmentDetached(NetSegment segment);
    void segmentRemoved(NetSegment segment);
    void linkRemoved(uint uidFrom, uint uidTo, QString type);
    void splitComplete(Runner splitRunner, NetSegment newSource, NetSegment newDestination);
    void segmentsChanged();
    void netChanged();
    void sceneChanged();
    void childrenChanged();
    void nameChanged(QString name);
    void boundsChanged();

private:

    QPair<QVariantList, QVariantList> extractBranch(uint edgeUid);
    QList<Edge> detachBranch(uint edgeUid, bool erase = true);
    QHash<uint, Edge>::iterator findInEdge(uint uid);
    QString m_name;
    Guide buildPath(Runner runner, qreal scale);
    Batch &findBatch(uint from, uint to);
//    void buildBatches();

    // QGraphicsItem interface

protected:

    NetSegment findSegment(const QPointF &point, Runner *runner = nullptr) const;
};

inline QDebug operator<<(QDebug dbg, const GuideNet::Edge &e)
{
    dbg = qDebug().nospace();
    dbg << "GuideLink::Edge src: " << e.sourceUid << " dst: " << e.destUid << " vertex: " << (NetVertex::ElementType) e.vertex.type() ;
    return dbg;
}


#endif // GUIDENETITEM_H
