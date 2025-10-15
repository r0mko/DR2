#include "vnode.h"



VectorNode::VectorNode(const QLineF &l)
{
    QLineF uv = l.unitVector();
    setLength(l.length());
    setUnitVector(QPointF(uv.dx(), uv.dy()));
}

VectorNode::VectorNode(const QPointF &p1, const QPointF &p2)
    : VectorNode(QLineF(p1, p2))
{ }
