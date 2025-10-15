#include "node.h"
#include "pvchain.h"
#include <QLineF>
#include <QDebug>


class PVChainData : public QSharedData
{
public:
    Node node1;
    Node node2;
    PVNode first;
    PVNode last;
    bool directional;
    QHash<int, QVariant> data;
    qreal length;

    ~PVChainData() {
        qDebug() << "edge data destroyed";
    }
};


bool PVChain::directional() const
{
    return d->directional;
}

QVariant PVChain::data(int role) const
{
    return d->data.value(role, QVariant());
}

void PVChain::setDirectional(bool directional)
{
    d->directional = directional;
}

PVNode PVChain::first() const
{
    return d->first;
}

int PVChain::count()
{
    int i = 0;
    PVNode n = d->first;
    while (!n.isNull()) {
        ++i;
        n = n.next();
    }
    return ++i;
}

int PVChain::refCount()
{
    return d->ref;
}

void PVChain::append(const PVChain &next)
{
    d->last.setNext(next.d->first);
    d->node2 = next.d->node2;
    d->last = next.d->last;
    d->length += next.d->length;
}



Node PVChain::node1() const
{
    return d->node1;
}

void PVChain::setNode1(const Node &node1)
{
    d->node1 = node1;
    d->node1.addOutEdge(*this);
}

Node PVChain::node2() const
{
    return d->node2;
}

void PVChain::setNode2(const Node &node2)
{
    d->node2 = node2;
    d->node2.addInEdge(*this);
}

bool PVChain::isNull() const
{
    return d.constData() == nullptr || d->first.isNull() || d->last.isNull();
}

QPolygonF PVChain::polygon(bool reverse) const
{
    PVNode n = reverse ? d->last : d->first;
    QPointF pt = reverse ? d->node2.point() : d->node1.point();
    QPolygonF ret;
    do {
        ret.append(pt);
        pt += n.unitVector() * n.length();
        if (reverse)
            n = n.prev();
        else
            n = n.next();
    } while (!n.isNull());
    ret.append(pt);
    return ret;
}

PVNode PVChain::last() const
{
    return d->last;
}


qreal PVChain::length() const
{
    return d->length;
}

void PVChain::destroy()
{
    d->node1.removeOutEdge(*this);
    d->node2.removeInEdge(*this);
    d->node1 = Node();
    d->node2 = Node();
    d->first = PVNode();
    d->last= PVNode();
    d->length = 0;
    d.reset();
    cleared();
}

bool PVChain::operator ==(const PVChain &other) const
{
    return d == other.d;
}





void PVChain::calcLength()
{
    PVNode n = d->first;
    d->length = d->first.length();
    while (n != d->last) {
        n = n.next();
        if (n.isNull())
            break;
        d->length += n.length();
    }
}

void PVChain::checkShared()
{
    PVNode n = d->first;
    do {
        PVNode next = n.next();
        if (!n.isShared(2)) {
            n.resetNext();
            n.resetPrev();
        }
        n = next;
    } while (n != d->last && !n.isNull());
}

PVChain::PVChain() :
    d(new PVChainData)
{
    d->directional = false;
    d->length = 0;
}

PVChain::~PVChain()
{
//    qDebug() << "Destroy PVEdge, ref =" << (d ? (int)d->ref : 0);
}

PVChain::PVChain(const PVChain &other) :
    d(other.d)
{}

PVChain &PVChain::operator =(const PVChain &other)
{
    d = other.d;
    return *this;
}

void PVChain::setEdge(const QPolygonF &polygon, bool reverse)
{
    if(polygon.size() < 2)
        return;
    d->first.init();
    PVNode n = d->first;

    d->length = 0;
    for (int i = 1; i < polygon.size(); ++i) {
        const QPointF &p1 = polygon[i-1];
        const QPointF &p2 = polygon[i];
        QLineF l(p1, p2);
        QLineF uv(l.unitVector());
        qreal len = l.length();
        QPointF unitVector = reverse ? QPointF(-uv.dx(), -uv.dy()) : QPointF(uv.dx(), uv.dy());
        n.setLength(len);
        n.setUnitVector(unitVector);
        d->length += len;
        PVNode next;
        next.init();
        if (reverse) {
            next.setNext(n);
        } else {
            n.setNext(next);
        }
        n = next;
    }
    if (reverse) {
        n = n.next();
        n.resetPrev();
        d->last = d->first;
        d->first = n;
    } else {
        d->last = n.prev();
        n.resetPrev();
    }
}

void PVChain::setData(QVariant data, int role)
{
    d->data.insert(role, data);
}

void PVChain::clear()
{
    PVNode n = d->first;
    while (true) {
        PVNode next = n.next();
        if (!n.isShared(2)) {
            n.setLength(0);
            if (n.hasNext())
                n.resetNext();
            if (n.hasPrev())
                n.resetPrev();
            n = PVNode();
        } else {
            qDebug() << "avoid removing shared PVNode";
        }
        if (next.isNull())
            break;
    };
    d->first = PVNode();
    d->last = PVNode();
}


