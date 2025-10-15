#ifndef NODE_H
#define NODE_H

#include <QSharedData>
#include <QVariant>
#include <QPointF>
#include "node.h"
#include "graphdefs.h"

class NodeData;
class PVChain;
class DrGraph;
class Node
{
public:
    Node();
    Node(const QPointF &pt, DrGraph *graph = 0);
    Node(const Node &);
    Node &operator =(const Node &);
    ~Node();

    const QPointF &point() const;

    void addOutEdge(PVChain edge);
    void addInEdge(PVChain edge);
    void removeOutEdge(PVChain edge);
    void removeInEdge(PVChain edge);
    void destroy();

    void setPoint(const QPointF &point);

    QList<PVChain> outEdges() const;
    QList<PVChain> inEdges() const;

    int refCount() const;

    inline bool isNull() const { return d.constData() == nullptr; }
    bool operator ==(const Node &n2) const;

    void setData(QVariant data, int role = UserRole);
    QVariant data(int role = UserRole) const;

    QList<PVChain> &outEdgesRef();
    QList<PVChain> &inEdgesRef();
private:
    friend class NodeData;
    QExplicitlySharedDataPointer<NodeData> d;

};

uint qHash(const Node &key, uint seed);
#endif // NODE_H
