#include "graph.h"

#include <QDebug>

DrGraph::DrGraph()
{

}

void DrGraph::addEdgesFromPolygon(const QPolygonF &edges, bool directional)
{
    for (int i = 0; i < edges.size() - 1; ++i) {
        QPolygonF edge;
        edge.append(edges[i]);
        edge.append(edges[i + 1]);
        addPolygonEdge(edge, directional);
    }
}

void DrGraph::addPolygonEdge(const QPolygonF &polygon, bool directional)
{
    PVChain e;
    e.setEdge(polygon);
    e.setDirectional(directional);
    Node n1 = getNode(polygon.first());
    Node n2 = getNode(polygon.last());
    //    int edgeId = m_edges.size();
    e.setNode1(n1);
    e.setNode2(n2);
    edgeAdded(&e);
    m_edges.append(e);
}

void DrGraph::removeTransient()
{
//    qDebug() << "Node count" << m_nodes.size() << "edge count" << m_edges.size();
    auto it = m_nodes.begin();
    while (it != m_nodes.end()) {
        Node &n = *it;
        if (n.inEdges().size() == 1 && n.outEdges().size() == 1) {
            PVChain in = n.inEdgesRef().first();
            PVChain out = n.outEdgesRef().first();
//            qDebug() << "node" << n.point() << "is transient";
            in.append(out);
            in.node2().addInEdge(in);
            out.destroy();
            n.destroy();
        }
        if (n.isNull())
            it = m_nodes.erase(it);
        else
            ++it;
    }
    checkEdgeList();
//    qDebug() << "Node count" << m_nodes.size() << "edge count" << m_edges.size();

}

void DrGraph::dump() const
{
    qDebug() << "nodes:";
    int i = 1;
    for (Node n:m_nodes) {
        qDebug() << i++ << "Pos:" << n.point() << "Ref:" << n.refCount() << "Out edges" << n.outEdges().size() << "In edges" << n.inEdges().size();
    }
    i = 1;
    qDebug() << "edges:";
    for (PVChain e:m_edges) {
        qDebug() << i++ << "nodes:" << e.count() << "Node1:" << e.node1().point() << "Node2:" << e.node2().point() << "Polygon:" << e.polygon();
    }
}

int DrGraph::checkEdgeList()
{
    int ret = 0;
    auto it2 = m_edges.begin();
    while (it2 != m_edges.end()) {
        PVChain &e = *it2;
        if (e.isNull()) {
            it2 = m_edges.erase(it2);
            ++ret;
        } else
            ++it2;
    }
    return ret;
}

PVChain DrGraph::edgeAt(int index) const
{
    return m_edges.at(index);
}



DrGraph::~DrGraph()
{

}

Node DrGraph::getNode(const QPointF &point)
{
    auto it = m_table.find(point);
    uint idx;
    if (it == m_table.end()) {
        idx = m_nodes.size();
        Node n(point, this);
        m_table.insert(point, idx);
        nodeAdded(&n);
        m_nodes.append(n);
    } else {
        idx = it.value();
    }
    return m_nodes[idx];
}

void DrGraph::scanNodes()
{
    int i = 0;
    for (Node &m:m_nodes) {
        if (m.refCount() == 1) {
            m.destroy();
            ++i;
        }
    }
    if (i > 0)
        qDebug() << "Found" << i << "unused nodes";
}

uint qHash(const QPointF &key, uint seed)
{
    uint h1 = qHash((quint64)key.x(), seed);
    uint h2 = qHash((quint64)key.y(), seed);
    return ((h1 << 16) | (h1 >> 16)) ^ h2 ^ seed;
}



QQmlListProperty<GraphEdge> GraphQml::qmlEdges() {
    return QQmlListProperty<GraphEdge>(this, 0,
                                  &GraphQml::qmlEdgesAppend,
                                  &GraphQml::qmlEdgesCount,
                                  &GraphQml::qmlEdgesAt,
                                      QQmlListProperty<GraphEdge>::ClearFunction());
}

void GraphQml::qmlEdgesAppend(QQmlListProperty<GraphEdge> *list, GraphEdge *object)
{
    GraphQml *o = qobject_cast<GraphQml*>(list->object);
}

int GraphQml::qmlEdgesCount(QQmlListProperty<GraphEdge> *list)
{
    GraphQml *o = qobject_cast<GraphQml*>(list->object);
    return o->edgesCount();
}

GraphEdge *GraphQml::qmlEdgesAt(QQmlListProperty<GraphEdge> *list, int index)
{
    GraphQml *o = qobject_cast<GraphQml*>(list->object);
    QVariant w = o->edgeAt(index).data(WrapperObject);

//    qDebug() << w.value<GraphEdge*>();
    return w.value<GraphEdge*>();
}

GraphQml::GraphQml(QObject *parent) : QObject(parent)
{

}

GraphQml::~GraphQml()
{

}

void GraphQml::addPolyline(QVariantList pol, bool directional)
{
    QPolygonF lineString;
    for (QVariant v:pol) {
        QVariantList pt = v.toList();
        if (pt.size() == 2) {
            QPointF point(pt.first().toDouble(), pt.last().toDouble());
            lineString.append(point);
        }
    }
    addEdgesFromPolygon(lineString, directional);
}

void GraphQml::simplify()
{
    removeTransient();
    if (checkEdgeList())
        emit edgesChanged();
}


void GraphQml::edgeAdded(PVChain *edge)
{
    GraphEdge *e = new GraphEdge(*edge, this);
    *edge = *e;
    emit edgesChanged();
}


GraphEdge::GraphEdge(PVChain edge, GraphQml *graph) :
    QObject(graph),
    PVChain(edge),
    m_graph(graph),
    m_reversed(false)
{
//    qDebug() << "Edge refcount" << edge.refCount();
    QVariant wrap = QVariant::fromValue(this);
    setData(wrap, WrapperObject);
}

QPolygonF GraphEdge::polygon() const
{
    return PVChain::polygon(m_reversed);
}

bool GraphEdge::reversed() const
{
    return m_reversed;
}

GraphQml *GraphEdge::graph() const
{
    return m_graph;
}

void GraphEdge::setReversed(bool arg)
{
    if (m_reversed == arg)
        return;

    m_reversed = arg;
    emit reversedChanged(arg);
}

void GraphEdge::setGraph(GraphQml *arg)
{
    if (m_graph == arg)
        return;

    m_graph = arg;
    emit graphChanged(arg);
}

QPointF GraphEdge::node1Point() const
{
    return node1().point();
}

QPointF GraphEdge::node2Point() const
{
    return node2().point();
}

void GraphEdge::cleared()
{
    deleteLater();
    if (m_graph->checkEdgeList())
        emit m_graph->edgesChanged();
}

QQmlListProperty<GraphEdge> GraphEdge::qmlSiblingsNode1() {
    return QQmlListProperty<GraphEdge>(this, 0,
                                       &GraphEdge::qmlSiblingsNode1Count,
                                       &GraphEdge::qmlSiblingsNode1At);
}


int GraphEdge::qmlSiblingsNode1Count(QQmlListProperty<GraphEdge> *list)
{
    GraphEdge *o = qobject_cast<GraphEdge*>(list->object);
    return o->node1().inEdges().size() + o->node1().outEdges().size();
}

GraphEdge *GraphEdge::qmlSiblingsNode1At(QQmlListProperty<GraphEdge> *list, int index)
{
    GraphEdge *o = qobject_cast<GraphEdge*>(list->object);
    QVariant wrapper;
    if (index >= o->node1().inEdges().size()) {
        index -= o->node1().inEdges().size();
        wrapper = o->node1().outEdges().at(index).data(WrapperObject);
    } else {
        wrapper = o->node1().inEdges().at(index).data(WrapperObject);
    }
    return wrapper.value<GraphEdge*>();

}

QQmlListProperty<GraphEdge> GraphEdge::qmlSiblingsNode2() {
    return QQmlListProperty<GraphEdge>(this, 0,
                                       &GraphEdge::qmlSiblingsNode2Count,
                                       &GraphEdge::qmlSiblingsNode2At);
}

int GraphEdge::qmlSiblingsNode2Count(QQmlListProperty<GraphEdge> *list)
{
    GraphEdge *o = qobject_cast<GraphEdge*>(list->object);
    return o->node2().inEdges().size() + o->node2().outEdges().size();
}

GraphEdge *GraphEdge::qmlSiblingsNode2At(QQmlListProperty<GraphEdge> *list, int index)
{
    GraphEdge *o = qobject_cast<GraphEdge*>(list->object);
    QVariant wrapper;
    if (index >= o->node2().inEdges().size()) {
        index -= o->node2().inEdges().size();
        wrapper = o->node2().outEdges().at(index).data(WrapperObject);
    } else {
        wrapper = o->node2().inEdges().at(index).data(WrapperObject);
    }
    return wrapper.value<GraphEdge*>();
}


void EdgeRunner::setPosition(qreal arg)
{
    PVRunner::setPosition(arg);
    notifyChanges();
}

GraphEdge *EdgeRunner::edge() const
{
    return static_cast<GraphEdge*>(PVRunner::edgePtr());
}

void EdgeRunner::setEdge(GraphEdge *arg)
{
    PVRunner::setEdge(*arg);
    emit edgeChanged();
}

void EdgeRunner::setBackwards(bool arg)
{
    if (backwards() == arg)
        return;
    setBackwards(arg);
    emit backwardsChanged();
}

void EdgeRunner::moveToNearest(QPointF pos)
{
    PVRunner::toClosestPosition(pos);
    notifyChanges();
}


void EdgeRunner::notifyChanges()
{
    static bool s_atVtx;
    static bool s_atStart;
    static bool s_atEnd;
    static qreal s_pos;
    if (s_atVtx != atVertex()) {
        s_atVtx = atVertex();
        emit atVertexChanged();
    }
    if (s_atStart != atStart()) {
        s_atStart = atStart();
        emit atStartChanged();
    }
    if (s_atEnd != atEnd()) {
        s_atEnd = atEnd();
        emit atEndChanged();
    }
    if (s_pos != position()) {
        s_pos = position();
        emit positionChanged();
//        emit lengthAheadChanged();
//        emit lengthBehindChanged();
        emit pointChanged();
    }
}
