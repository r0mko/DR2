#ifndef ROOTNODE_H
#define ROOTNODE_H

#include "node.h"

class GraphModel;
class RootNode : public Node
{
    METAGRAPH_NODE(RootNode)
public:
    GraphModel* model{nullptr};
    using Node::Node;
};


class RootNodeHandle : public NodeHandle
{
    NODE_FN(RootNode)
    Q_GADGET
    Q_PROPERTY(GraphModel* model READ model)

    GraphModel* m_model;

public:
    using NodeHandle::NodeHandle;

    GraphModel* model() const;
};

class RootNodeFactory : public NodeFactory
{
    Q_OBJECT

    friend class RootNodeHandle;
public:
    using NodeFactory::NodeFactory;

    int nodeMetatypeId() const;
    void importNode(QDataStream &in, Node *node);
    void exportNode(QDataStream &out, Node *node) const;

protected:
    void setupNode(Node *);
};
DECLARE_NODE(RootNode)

#endif // ROOTNODE_H
