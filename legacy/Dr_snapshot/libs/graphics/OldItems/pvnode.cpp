#include "pvnode.h"
#include <QDebug>

class PVNodeData : public QSharedData
{
public:
    QPointF     unitVector;
    qreal       length;

    mutable PVNodeData *prev;
    mutable PVNodeData *next;

    PVNodeData() :
        length(0),
        prev(nullptr),
        next(nullptr)
    {}
    ~PVNodeData() {}

};

PVNode::PVNode() : d(nullptr)
{}

PVNode::PVNode(const QPointF &unitVector, qreal length) : d(new PVNodeData)
{
    d->unitVector = unitVector;
    d->length = length;
}

PVNode::PVNode(const PVNode &rhs) : d(rhs.d)
{}

QPointF PVNode::unitVector() const
{
    return d->unitVector;
}

QPointF PVNode::vector() const
{
    return d->unitVector * d->length;
}

QPointF PVNode::normal() const
{
    return QPointF(-d->unitVector.y(), d->unitVector.x());
}

qreal PVNode::length() const
{
    return d->length;
}
void PVNode::setUnitVector(const QPointF &unitVector)
{
    d->unitVector = unitVector;
}

void PVNode::setLength(qreal length)
{
    d->length = length;
}

void PVNode::setNext(PVNode next, bool reciprocate) const
{
    if (d->next) {
        d->next->ref.deref();
        if (reciprocate) {
            d->ref.deref();
            d->next->prev = nullptr;
        }
    }
    d->next = next.d.data();
    next.d->ref.ref();
    if (reciprocate) {
        next.d->prev = d.data();
        d->ref.ref();
    }
}

void PVNode::setPrev(PVNode prev, bool reciprocate) const
{
    if (d->prev) {
        d->prev->ref.deref();
        if (reciprocate) {
            d->ref.deref();
            d->prev->next = nullptr;
        }
    }
    d->prev = prev.d.data();
    prev.d->ref.ref();
    if (reciprocate) {
        prev.d->next = d.data();
        d->ref.ref();
    }
}

void PVNode::resetNext(bool reciprocate) const
{
    if (d->next) {
        d->next->ref.deref();
        next().checkDeleted();
        if (reciprocate) {
            d->ref.deref();
            d->next->prev = nullptr;
        }
    }
    d->next = nullptr;
}

void PVNode::resetPrev(bool reciprocate) const
{
    if (d->prev) {
        d->prev->ref.deref();
        prev().checkDeleted();
        if (reciprocate) {
            d->ref.deref();
            d->prev->next = nullptr;
        }
    }
    d->prev = nullptr;
}

bool PVNode::hasNext() const
{
    return d->next != nullptr;
}

bool PVNode::hasPrev() const
{
    return d->prev != nullptr;
}

bool PVNode::isolated() const
{
    return d->prev == nullptr && d->next == nullptr;
}

PVNode PVNode::next() const
{
    return PVNode(d->next);
}

PVNode PVNode::prev() const
{
    return PVNode(d->prev);

}

void PVNode::init()
{
    d = new PVNodeData;
}

void PVNode::destroy()
{
    if (hasNext()) {
        next().destroy();
        if (d && d->ref.deref())
            checkDeleted();
    }
}

bool PVNode::isShared(int shareCount) const
{
    return d->ref > shareCount;
}

bool PVNode::operator ==(const PVNode &other) const
{
    return d == other.d;
}

bool PVNode::operator !=(const PVNode &other) const
{
    return d != other.d;
}



PVNode::PVNode(PVNodeData *dataPtr) :
    d(dataPtr)
{

}

PVNode &PVNode::operator=(const PVNode &rhs)
{
    if (this != &rhs)
        d.operator=(rhs.d);
    return *this;
}

PVNode::~PVNode()
{

}

