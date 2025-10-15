#ifndef INSTANCENODE_H
#define INSTANCENODE_H
#include "node.h"
#include "type.h"
#include <qpointer.h>

class ElementAgent;
class Collection2;

class Instance : public Node
{
    QVector<QVariant> m_propertyValues;

    mutable QPointer<ElementAgent> i;
    Collection2 *collection() const;
    METAGRAPH_NODE(Instance)
public:
    Instance() {}

    QVariant getProperty(int index) const;
    void setProperty(int index, const QVariant &value);
    void resize(int count);
    virtual int typeId() const;
    ElementAgent *wrapper();
};

class InstanceHandle : public NodeHandle
{
    Q_GADGET
    Q_PROPERTY(ElementAgent* i READ instance)
    Q_PROPERTY(TypeHandle elementType READ elementType)
    NODE_FN(Instance)

public:
    using NodeHandle::NodeHandle;
    ElementAgent* instance() const;
    TypeHandle elementType() const;
};

class InstanceFactory : public NodeFactory
{
    Q_OBJECT
public:
    using NodeFactory::NodeFactory;

    int nodeMetatypeId() const;
    void importNode(QDataStream &in, Node *node);
    void exportNode(QDataStream &out, Node *node) const;


};

DECLARE_NODE(Instance)


#endif // INSTANCENODE_H
