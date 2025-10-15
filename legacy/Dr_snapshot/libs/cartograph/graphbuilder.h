#ifndef GRAPHBUILDER_H
#define GRAPHBUILDER_H

#include <QObject>
#include "defs.h"
#include <QAbstractListModel>
#include <QUrl>
#include <QQmlComponent>

class MapView;
class NodeModel;
class RibModel;
class GraphBuilder;

class GraphNode
{
    friend class GraphBuilder;
    QPointF m_point;
    QList<int> m_incomingRibs;
    QList<int> m_outgoingRibs;
public:
    GraphNode() {}

    QPointF point() const;
    QList<int> incomingRibs() const;
    QList<int> outgoingRibs() const;

    void appendIncomingRib(int ribId);
    void appendOutgoingRib(int ribId);

    void setPoint(const QPointF &point);
    void setIncomingRibs(const QList<int> &incomingRibs);
    void setOutgoingRibs(const QList<int> &outgoingRibs);
};

class GraphRib
{
    friend class GraphBuilder;
    int m_startNode;
    int m_endNode;
    bool m_oneway;
    QString m_name;
    qreal m_featlen;
    qreal m_z;
    QPolygonF m_polygon;
    qreal m_polygonLength;
    struct PolNode {
        PolNode() : segLength(0), position(0), angle(0)
        { }
        qreal   segLength;
        qreal   position;
        qreal   angle;
    };
    QVector<PolNode> nodes;

public:
    GraphRib() {}

    inline int startNode() const;
    inline int endNode() const;

    inline bool oneway() const;
    inline QString name() const;
    inline qreal length() const;
    inline qreal z() const;
    inline QPolygonF polygon() const;
    inline qreal polygonLength() const;

    inline void setStartNode(const int &startNode);
    inline void setEndNode(const int &endNode);
    inline void setOneway(bool oneway);
    inline void setName(const QString &name);
    inline void setLength(const qreal &length);
    inline void setZ(const qreal &z);
    inline void setPolygon(const QPolygonF &polygon);

    inline QPointF pointAtLength(qreal pos, int *nodeNo = nullptr) const;
    inline QPointF pointAtPercent(qreal percent) const;
};

class NodeModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)

    VertexBuffer buffer;
    QList<GraphNode> m_nodes;
    friend class GraphBuilder;

public:
    enum NodeRoles {
        IncomingRibsRole = Qt::UserRole + 1,
        OutgoingRibsRole,
        XRole,
        YRole,
        PointRole,
        LastNodeRole = PointRole
    };
    NodeModel(QObject *parent = 0) : QAbstractListModel(parent) {}
    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    GraphNode *node(int index);
    QVariant data(const QModelIndex &index, int role) const;
    Q_INVOKABLE int addNode(const QPointF &pt);
    Q_INVOKABLE QVariantMap nodeAt(int index) const;
    int count() const;

public slots:
    void dumpNodes(QUrl filename);
    void loadDump(QUrl filename);

signals:
    void countChanged();
};

class RibModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

    QList<GraphRib> m_ribs;
    friend class GraphBuilder;
    int m_count;

public:
    enum RibRoles {
        StartNodeRole = NodeModel::LastNodeRole + 1,
        EndNodeRole,
        OnewayRole,
        NameRole,
        LengthRole,
        PolygonLengthRole,
        PolygonRole,
        ZRole
    };
    RibModel(QObject *parent = 0) : QAbstractListModel(parent) {}
    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    Q_INVOKABLE QVariantMap ribAt(int index) const;
    int count() const;

public slots:
    int addRib(int nodeFrom, int nodeTo, const QPolygonF &polygon, qreal length, bool oneway, qreal z = 0, QString name = QString());

signals:
    void countChanged();
};


class GraphBuilder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(NodeModel* nodes READ nodes WRITE setNodes NOTIFY nodesChanged)
    Q_PROPERTY(RibModel* ribs READ ribs WRITE setRibs NOTIFY ribsChanged)
    Q_PROPERTY(QQmlComponent* ribDelegate READ ribDelegate WRITE setRibDelegate NOTIFY ribDelegateChanged)

    NodeModel* m_nodes = nullptr;
    RibModel* m_ribs = nullptr;
    QQmlComponent* m_ribDelegate = nullptr;
    QList<QObject*> constructedItems;


public:

    GraphBuilder(QObject *parent = 0);
    virtual ~GraphBuilder() {}
    NodeModel *nodes() const;
    RibModel *ribs() const;

    QList<int> getNeighborNodes(int nodeId) const;
    QList<int> getReverseNeighborNodes(int nodeId) const;

    QList<QPair<int, qreal>> getForwardNodes(int nodeId) const;
    QList<QPair<int, qreal>> getReverseNodes(int nodeId) const;

    QList<int> getOutgoingRibs(int nodeId) const;
    QList<int> getIncomingRibs(int nodeId) const;

    const GraphNode &node(int nodeId) const;
    const GraphRib &rib(int ribId) const;

    QQmlComponent* ribDelegate() const
    {
        return m_ribDelegate;
    }

public slots:
    void addRib(QVariant geometryBlob, uint id, qreal length, bool oneway, qreal z = 0, QString name = QString(), MapView *view = nullptr);
    void addRibFromPoints(QVariantList pol, uint id, qreal length, bool oneway, qreal z = 0, QString name = QString(), MapView *view = nullptr);
    void setNodes(NodeModel* arg);
    void setRibs(RibModel* arg);

    void setRibDelegate(QQmlComponent* arg)
    {
        if (m_ribDelegate == arg)
            return;

        m_ribDelegate = arg;
        emit ribDelegateChanged(arg);
    }

signals:
    void nodesChanged(NodeModel* arg);
    void ribsChanged(RibModel* arg);
    void ribDelegateChanged(QQmlComponent* arg);

private:
    void appendRib(const QPolygonF &lineString, uint id, qreal length, bool oneway, qreal z = 0, QString name = QString());
};


inline qreal fixAngle(qreal angle)
{
    qreal m = fmod(angle, 360);
    if (m < 0)
        m += 360;
    if (m > 180)
        m -= 360;
    return m;
}

inline QPointF segPointAtPercent(const QPointF &p1, const QPointF &p2, qreal percent)
{
    return p1 + percent * (p2 - p1);
}
inline QPointF segPointAtLength(const QPointF &p1, const QPointF &p2, qreal length)
{
    qreal l = QLineF(p1, p2).length();
    return segPointAtPercent(p1, p2, length / l);
}

int GraphRib::startNode() const
{
    return m_startNode;
}

int GraphRib::endNode() const
{
    return m_endNode;
}

bool GraphRib::oneway() const
{
    return m_oneway;
}

QString GraphRib::name() const
{
    return m_name;
}

qreal GraphRib::length() const
{
    return m_featlen;
}

qreal GraphRib::z() const
{
    return m_z;
}

QPolygonF GraphRib::polygon() const
{
    return m_polygon;
}

qreal GraphRib::polygonLength() const
{
    return m_polygonLength;
}

void GraphRib::setStartNode(const int &startNode)
{
    m_startNode = startNode;
}

void GraphRib::setEndNode(const int &endNode)
{
    m_endNode = endNode;
}

void GraphRib::setOneway(bool oneway)
{
    m_oneway = oneway;
}

void GraphRib::setName(const QString &name)
{
    m_name = name;
}

void GraphRib::setLength(const qreal &length)
{
    m_featlen = length;
}

void GraphRib::setZ(const qreal &z)
{
    m_z = z;
}

void GraphRib::setPolygon(const QPolygonF &polygon)
{
    m_polygon = polygon;
    m_polygonLength = 0;
    QLineF seg1;
    nodes.clear();
    nodes.reserve(polygon.size());
    for (int i = 0; i < polygon.size() - 1; ++i) {
        QLineF seg2(polygon.at(i), polygon.at(i + 1));
        PolNode n;
        n.segLength = seg2.length();
        n.position = m_polygonLength;
        m_polygonLength += n.segLength;
        if (!seg1.isNull()) {
            n.angle = fixAngle(seg2.angle() - seg1.angle());
        }
        seg1 = seg2;
        nodes.append(n);
    }
    PolNode n;
    n.position = m_polygonLength;
    nodes.append(n);
}

QPointF GraphRib::pointAtLength(qreal pos, int *nodeNo) const
{
    QPolygonF p = m_polygon;
    if (pos < 0)
        return p.first();
    int i = 0;
    for (const PolNode n:nodes) {
        if (n.position + n.segLength < pos)
            ++i;
        else
            break;
    }
    if (nodeNo)
        *nodeNo = i;
    if (i < nodes.length() - 1)
        return segPointAtPercent(p.at(i), p.at(i + 1), (pos - nodes.at(i).position) / nodes.at(i).segLength);
    return p.last();
}

QPointF GraphRib::pointAtPercent(qreal percent) const
{
    return pointAtLength(m_polygonLength * percent);
}


#endif // GRAPHBUILDER_H
