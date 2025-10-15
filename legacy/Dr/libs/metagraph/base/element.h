#ifndef ELEMENT_H
#define ELEMENT_H

#include "node.h"
#include "collectionobject.h"
#include <QVariant>

class Collection;
class Element : public Node
{
    METAGRAPH_NODE(Element)
public:
    using Node::Node;
    int typeId() const;

    QVector<QVariant> m_propertyValues; // data when type is linked and registered to collection
    QVariantMap m_storedPropertyValues; // data when otherwise or saving
    Collection *collection() const;
    bool m_isStored{true};
    mutable QPointer<ElementAgent> i;
    ElementAgent *wrapper();
    bool isStored() const{return m_isStored;} // state of the element is it stored or not, defines data storage and access methods
    QVariant getProperty(int index) const;
    void setProperty(int index, const QVariant &value);
    void store();
    void restore();
    void resize(int count);
};


class ElementHandle : public NodeHandle
{
    NODE_FN(Element)
    Q_GADGET
    Q_PROPERTY(ElementAgent* i READ instance)
    Q_PROPERTY(QVariantList properties READ properties)


public:
    using NodeHandle::NodeHandle;
    void store();
    void restore();

//    Q_INVOKABLE ElementAgent* instance() const;


    ElementAgent* instance() const;
    QVariantList properties() const;
};

class ElementFactory : public NodeFactory
{
    Q_OBJECT

    friend class ElementHandle;
public:
    using NodeFactory::NodeFactory;

    int nodeMetatypeId() const;
    void importNode(QDataStream &in, Node *node);
    void exportNode(QDataStream &out, Node *node) const;

protected:
    void setupNode(Node *);
};
DECLARE_NODE(Element)

#endif // ELEMENT_H
