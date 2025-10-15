#include "rootnode.h"
#include "graphmodel.h"


int RootNode::typeId() const
{
    return SELF_TYPEID;
}


int RootNodeFactory::nodeMetatypeId() const
{
    return qMetaTypeId<RootNode>();
}

void RootNodeFactory::importNode(QDataStream &in, Node *node)
{
    NodeFactory::importNode(in, node);
}

void RootNodeFactory::exportNode(QDataStream &out, Node *node) const
{
    NodeFactory::exportNode(out, node);
}

void RootNodeFactory::setupNode(Node *node)
{
    RootNode *n = static_cast<RootNode*>(node);
    n->model = model();
}
