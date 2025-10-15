#ifndef PVNODE_H
#define PVNODE_H

#include <QSharedData>
#include <QPointF>

class PVNodeData;

class PVNode
{
public:
    PVNode();
    PVNode(const QPointF &unitVector, qreal length);
    PVNode(const PVNode &);
    PVNode &operator =(const PVNode &);
    ~PVNode();

    QPointF unitVector() const;
    QPointF vector() const;
    QPointF normal() const;
    qreal length() const;

    void setUnitVector(const QPointF &unitVector);
    void setLength(qreal length);

    void setNext(PVNode next, bool reciprocate = true) const;
    void setPrev(PVNode prev, bool reciprocate = true) const;

    void resetNext(bool reciprocate = true) const;
    void resetPrev(bool reciprocate = true) const;

    inline bool isNull() const { return d.constData() == nullptr; }
    bool hasNext() const;
    bool hasPrev() const;
    bool isolated() const;

    PVNode next() const;
    PVNode prev() const;

    void init();
    void destroy();

    bool isShared(int shareCount = 1) const;
    bool operator ==(const PVNode &other) const;
    bool operator !=(const PVNode &other) const;
private:
    friend class PVNodeData;
    inline void checkDeleted() const {}
    PVNode(PVNodeData *dataPtr);
    QExplicitlySharedDataPointer<PVNodeData> d;
};




#endif // PVNODE_H
