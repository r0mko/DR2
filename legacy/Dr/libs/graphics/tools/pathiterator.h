#ifndef GUIDEPATHITERATOR_H
#define GUIDEPATHITERATOR_H

#include "guidepath_nodes.h"
#include <QLinkedList>



class PathIterator : public QLinkedList<PathNode>::iterator
{
public:
    PathIterator() { i = nullptr; }
    PathIterator(QLinkedList<PathNode>::iterator o) : QLinkedList<PathNode>::iterator(o) {}
    PathIterator(QLinkedList<PathNode>::const_iterator o) : QLinkedList<PathNode>::iterator(o.i) {}

    inline PathNode::NodeType currentNodeType() const { return (PathNode::NodeType)i->t.type(); }
    inline bool atHead() const { return (c()->type() & VectorNode::HeadType) == VectorNode::HeadType; }
    inline bool atTail() const { return (c()->type() & VectorNode::TailType) == VectorNode::TailType; }
    inline bool atPoint() const { return i->t.type() & VectorNode::PointType; }
    inline bool isNull() const { return i == nullptr; }
    inline bool valid() const { return !isNull(); }
    inline bool atVector() const { return i->t.type() == VectorNode::VectorType; }
    inline const VertexNode *pnode(const PathNode *node) const { return reinterpret_cast<const VertexNode*>(node); }
    inline const VectorNode *vnode(const PathNode *node) const { return reinterpret_cast<const VectorNode*>(node); }
    inline const VertexNode *pnode() const { return reinterpret_cast<VertexNode*>(&i->t); }
    inline const VectorNode *vnode() const { return reinterpret_cast<VectorNode*>(&i->t); }

    inline VertexNode *pnode(PathNode *node) { return reinterpret_cast<VertexNode*>(node); }
    inline VertexNode *pnode() { return reinterpret_cast<VertexNode*>(&i->t); }

    inline VectorNode *vnode(PathNode *node) { return reinterpret_cast<VectorNode*>(node); }
    inline VectorNode *vnode() { return reinterpret_cast<VectorNode*>(&i->t); }

    inline PathNode *next() const { return &i->n->t; }
    inline PathNode *prev() const { return &i->p->t; }

    inline PathNode *c() const { return &i->t; }
    inline bool hasNext() const { return next()->type() & VectorNode::VectorType; }
    inline bool hasPrev() const { return prev()->type() & VectorNode::VectorType; }

//    const VectorNode &node() { return
};

Q_DECLARE_METATYPE(PathIterator)

#endif // GUIDEPATHITERATOR_H
