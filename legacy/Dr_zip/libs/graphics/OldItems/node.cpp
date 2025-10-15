#include "node.h"
#include <QVector>
#include "pvchain.h"
#include <QSet>
#include "graph.h"
#define D_CHECK if (!d) d = new NodeData
class NodeData : public QSharedData
{
public:
    NodeData() {}

    QList<PVChain> inEdges;
    QList<PVChain> outEdges;
    QPointF point;
    QHash<int, QVariant> data;
    DrGraph *graph;
};

Node::Node() :
    d(nullptr)
{}

Node::Node(const QPointF &pt, DrGraph *graph) :
    d(new NodeData)
{
    d->point = pt;
    d->graph = graph;
}

Node::Node(const Node &other) :
    d(other.d)
{}

Node &Node::operator =(const Node &other)
{
    d = other.d;
    return *this;
}

Node::~Node()
{}

const QPointF &Node::point() const
{
    return d->point;
}

void Node::addOutEdge(PVChain edge)
{
    D_CHECK;
    d->outEdges.append(edge);
}

void Node::addInEdge(PVChain edge)
{
    D_CHECK;
    d->inEdges.append(edge);
}

void Node::removeOutEdge(PVChain edge)
{
    d->outEdges.removeAll(edge);
}

void Node::removeInEdge(PVChain edge)
{
    d->inEdges.removeAll(edge);
}

void Node::destroy()
{
    d->inEdges.clear();
    d->outEdges.clear();
    d.reset();
}

void Node::setPoint(const QPointF &point)
{
    D_CHECK;
    d->point = point;
}

QList<PVChain> &Node::outEdgesRef()
{
    return d->outEdges;
}

QList<PVChain> &Node::inEdgesRef()
{
    return d->inEdges;
}
QList<PVChain> Node::outEdges() const
{
    return d->outEdges;
}

QList<PVChain> Node::inEdges() const
{
    return d->inEdges;
}

int Node::refCount() const
{
    if (d.constData())
        return d->ref;
    return 0;
}

uint qHash(const Node &key, uint seed)
{
    return qHash(qMakePair(key.point().x(), key.point().y()), seed);
}



void Node::setData(QVariant data, int role)
{
    D_CHECK;
    d->data.insert(role, data);
}

QVariant Node::data(int role) const
{
//    D_CHECK;
    return d->data.value(role, QVariant());
}



bool Node::operator ==(const Node &n2) const {
    return d == n2.d;
}
