#ifndef PVCHAIN_H
#define PVCHAIN_H
#include "pvnode.h"
#include "node.h"
#include <QVariant>
#include <QPolygonF>
#include "graphdefs.h"

class PVChainData;
class PVChain
{

public:
    PVChain();
    PVChain(const PVChain &other);
    PVChain &operator =(const PVChain &other);
    virtual ~PVChain();

    void setEdge(const QPolygonF &polygon, bool reverse = false);
    void setData(QVariant data, int role = UserRole);
    void setDirectional(bool directional);
    void clear();
    void append(const PVChain &next);
    void setNode1(const Node &node1);
    void setNode2(const Node &node2);

    bool isNull() const;
    QPolygonF polygon(bool reverse = false) const;
    bool directional() const;
    QVariant data(int role = UserRole) const;
    PVNode first() const;
    PVNode last() const;
    Node node1() const;
    Node node2() const;
    int count();
    int refCount();
    qreal length() const;

    void destroy();
    bool operator ==(const PVChain &other) const;

protected:
    virtual void cleared() { }

private:
    QExplicitlySharedDataPointer<PVChainData> d;
    void calcLength();
    void checkShared();
};


#endif // PVCHAIN_H
