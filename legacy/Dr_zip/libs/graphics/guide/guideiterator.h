#ifndef GUIDEITERATOR_H
#define GUIDEITERATOR_H

#include "guide_nodes.h"
#include <QLinkedList>

class GuideIterator : public QLinkedList<GuideNode>::iterator
{
public:
    GuideIterator() { i = nullptr; }
    GuideIterator(QLinkedList<GuideNode>::iterator o) : QLinkedList<GuideNode>::iterator(o) {}
    GuideIterator(QLinkedListNode<GuideNode> *n) : QLinkedList<GuideNode>::iterator(n) {}
    GuideIterator(QLinkedList<GuideNode>::const_iterator o) : QLinkedList<GuideNode>::iterator(o.i) {}

    inline GuideNode::NodeType currentNodeType() const { return (GuideNode::NodeType)i->t.type(); }
    inline bool atHead() const { return (c()->type() & VectorNode::HeadType) == VectorNode::HeadType; }
    inline bool atTail() const { return (c()->type() & VectorNode::TailType) == VectorNode::TailType; }
    inline bool atPoint() const { return i->t.type() & VectorNode::PointType; }
    inline bool isNull() const { return i == nullptr; }
    inline bool valid() const { return !isNull(); }
    inline bool atVector() const { return i->t.type() == VectorNode::VectorType; }
    inline const VertexNode *pnode(const GuideNode *node) const { return reinterpret_cast<const VertexNode*>(node); }
    inline const VectorNode *vnode(const GuideNode *node) const { return reinterpret_cast<const VectorNode*>(node); }
    inline const VertexNode *pnode() const { return reinterpret_cast<VertexNode*>(&i->t); }
    inline const VectorNode *vnode() const { return reinterpret_cast<VectorNode*>(&i->t); }

    inline VertexNode *pnode(GuideNode *node) { return reinterpret_cast<VertexNode*>(node); }
    inline VertexNode *pnode() { return reinterpret_cast<VertexNode*>(&i->t); }

    inline VectorNode *vnode(GuideNode *node) { return reinterpret_cast<VectorNode*>(node); }
    inline VectorNode *vnode() { return reinterpret_cast<VectorNode*>(&i->t); }

    inline GuideNode *next() const { return &i->n->t; }
    inline GuideNode *prev() const { return &i->p->t; }

    inline GuideNode *c() const { return &i->t; }
    inline bool hasNext() const { return !atTail() && next()->isVector(); }
    inline bool hasPrev() const { return !atHead() && prev()->isVector(); }

//    const VectorNode &node() { return
};

Q_DECLARE_METATYPE(GuideIterator)

#endif // GUIDEITERATOR_H
