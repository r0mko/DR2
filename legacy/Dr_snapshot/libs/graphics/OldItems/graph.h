#ifndef GRAPH_H
#define GRAPH_H


#include <QObject>
#include <qqml.h>
#include "pvchain.h"
#include "pvrunner.h"

#include "node.h"
#include <QObject>

class DrGraph
{
    QVector<Node> m_nodes;
    QVector<PVChain> m_edges;
    QHash<QPointF, int> m_table;
    friend class Graph;

public:
    DrGraph();
    virtual ~DrGraph();

    void addEdgesFromPolygon(const QPolygonF &edges, bool directional);
    void addPolygonEdge(const QPolygonF &polygon, bool directional);
    void removeTransient();
    void dump() const;
    int checkEdgeList();
    inline int edgesCount() const { return m_edges.size(); }
    inline int nodeCount() const { return m_nodes.size(); }
    PVChain edgeAt(int index) const;

protected:
    virtual void edgeAdded(PVChain *edge) { Q_UNUSED(edge); }
    virtual void nodeAdded(Node *node) { Q_UNUSED(node); }

private:
    int nodeIdx(Node n);
    Node getNode(const QPointF &point);
    void scanNodes();
};



class GraphQml;
class GraphEdge : public QObject, public PVChain
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<GraphEdge> siblingsNode1 READ qmlSiblingsNode1 NOTIFY siblingsNode1Changed)
    Q_PROPERTY(QQmlListProperty<GraphEdge> siblingsNode2 READ qmlSiblingsNode2 NOTIFY siblingsNode2Changed)
    Q_PROPERTY(QPolygonF polygonData READ polygon NOTIFY polygonChanged)
    Q_PROPERTY(bool reversed READ reversed WRITE setReversed NOTIFY reversedChanged)
    Q_PROPERTY(GraphQml* graph READ graph WRITE setGraph NOTIFY graphChanged)

    Q_PROPERTY(QPointF node1Point READ node1Point NOTIFY node1PointChanged)
    Q_PROPERTY(QPointF node2Point READ node2Point NOTIFY node2PointChanged)

public:
    GraphEdge(PVChain edge, GraphQml *graph = 0);

    QPolygonF polygon() const;
    bool reversed() const;
    GraphQml* graph() const;
    void setReversed(bool arg);
    void setGraph(GraphQml* arg);
    QPointF node1Point() const;
    QPointF node2Point() const;

signals:
    void siblingsNode1Changed();
    void siblingsNode2Changed();
    void polygonChanged(QPolygonF arg);
    void reversedChanged(bool arg);
    void node1PointChanged(QPointF arg);
    void node2PointChanged(QPointF arg);
    void graphChanged(GraphQml* arg);

protected:
    void cleared();

private:
    QQmlListProperty<GraphEdge> qmlSiblingsNode1();
    QQmlListProperty<GraphEdge> qmlSiblingsNode2();

    static void qmlSiblingsNode1Append(QQmlListProperty<GraphEdge> *list, GraphEdge *object);
    static int qmlSiblingsNode1Count(QQmlListProperty<GraphEdge> *list);
    static GraphEdge *qmlSiblingsNode1At(QQmlListProperty<GraphEdge> *list, int index);

    static void qmlSiblingsNode2Append(QQmlListProperty<GraphEdge> *list, GraphEdge *object);
    static int qmlSiblingsNode2Count(QQmlListProperty<GraphEdge> *list);
    static GraphEdge *qmlSiblingsNode2At(QQmlListProperty<GraphEdge> *list, int index);

    bool m_reversed;
    GraphQml* m_graph;
};

Q_DECLARE_METATYPE(GraphEdge*)

class GraphQml : public QObject, public DrGraph
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<GraphEdge> edges READ qmlEdges NOTIFY edgesChanged)

    static void qmlEdgesAppend(QQmlListProperty<GraphEdge> *list, GraphEdge *object);
    static int qmlEdgesCount(QQmlListProperty<GraphEdge> *list);
    static GraphEdge *qmlEdgesAt(QQmlListProperty<GraphEdge> *list, int index);

public:
    explicit GraphQml(QObject *parent = 0);
    ~GraphQml();
    QQmlListProperty<GraphEdge> qmlEdges();

signals:
    void edgesChanged();

public slots:
    void addPolyline(QVariantList, bool directional = false);
    void simplify();

protected:
    void edgeAdded(PVChain *edge);
};

class EdgeRunner : public QObject, public PVRunner
{
    Q_OBJECT
    Q_PROPERTY(qreal position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(qreal angle READ angle NOTIFY angleChanged)
    Q_PROPERTY(bool atStart READ atStart NOTIFY atStartChanged)
    Q_PROPERTY(bool atEnd READ atEnd NOTIFY atEndChanged)
    Q_PROPERTY(bool atVertex READ atVertex NOTIFY atVertexChanged)
    Q_PROPERTY(QPointF point READ point NOTIFY pointChanged)
    Q_PROPERTY(GraphEdge* edge READ edge WRITE setEdge NOTIFY edgeChanged)
    Q_PROPERTY(bool backwards READ backwards WRITE setBackwards NOTIFY backwardsChanged)


public:
    EdgeRunner(QObject *parent = 0) : QObject(parent)
    {

    }
    virtual ~EdgeRunner() {}
    GraphEdge* edge() const;

    void setPosition(qreal arg);
    void setEdge(GraphEdge* arg);
    void setBackwards(bool arg);

public slots:
    void moveToNearest(QPointF pos);

signals:
    void positionChanged();
    void angleChanged();
    void edgeChanged();
    void pointChanged();
    void atStartChanged();
    void atEndChanged();
    void atVertexChanged();
    void backwardsChanged();

private:
    void notifyChanges();

    bool m_backwards;
};

#endif // GRAPH_H
