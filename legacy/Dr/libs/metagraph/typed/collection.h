#ifndef COLLECTION_H
#define COLLECTION_H

#include "node.h"
#include "type.h"
#include "nodechildrenmodel.h"
#include "element.h"
#include <QMetaObject>
#include <QDebug>

class CollectionModel : public NodeChildrenModel
{
    Q_OBJECT

    Q_PROPERTY(QString typeName READ typeName CONSTANT)

    const QMetaObject *m_elementType{ nullptr };
    int m_metaTypeId{ QMetaType::UnknownType };

public:
    friend class Collection;
    explicit CollectionModel(const QModelIndex &rootIndex, const QMetaObject *type, QObject *parent = nullptr);

    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;
    QString typeName() const;



public slots:
    QVariantList roleModel() const;
    QModelIndex addElement(const QJSValue &init = QJSValue());
    QObject *objectAt(int row);
    void printRoles() const;

};


class Collection: public Node
{
    METAGRAPH_NODE(Collection)
public:
    using Node::Node;
    virtual int typeId() const;

    Type* m_connectedType{nullptr};
    QSharedPointer<CollectionModel> m_cmodel;
    QMetaObject* m_metaobject{nullptr}; // pointer to current metaobject can be null, if null - elements are stored
    bool m_stored{true};
    void typeRegistered( bool s);
    void store();
    void restore();
};


class CollectionHandle : public NodeHandle
{
    NODE_FN(Collection)
    Q_GADGET

    Q_PROPERTY(bool isStored READ isStored)
    Q_PROPERTY(CollectionModel* elements READ elements)
    Q_PROPERTY(QString className READ className)
    Q_PROPERTY(int metaTypeId READ metaTypeId)
    Q_PROPERTY(QVariantList roleModel READ roleModel)

public:
    static int id_isStored;
    static int id_className;
    static int id_metaTypeId;
    static int id_roleModel;
    static int id_elements;
    using NodeHandle::NodeHandle;
    bool isStored() const;
    CollectionModel* elements() const;
    Q_INVOKABLE ElementHandle addElement(QJSValue init = QJSValue());
    Q_INVOKABLE void store();
    Q_INVOKABLE void restore();
    QString className() const
    {
//        if( isStored() ) return QString();
        if( node()->m_metaobject == nullptr ) return QString();
        return node()->m_metaobject->className();
    }
    int metaTypeId() const
    {
//        if( isStored() ) return QMetaType::UnknownType;
        if( node()->m_metaobject == nullptr ) return QMetaType::UnknownType;
        return QMetaType::type(QByteArray(node()->m_metaobject->className()).append("*"));
    }
    void typeRegistered( bool registered, TypeHandle th );
    QVariantList roleModel() const
    {
        if( elements() == nullptr ) return QVariantList();
        return elements()->roleModel();
    }
};

class CollectionFactory : public NodeFactory
{
    Q_OBJECT

    friend class CollectionHandle;


    QList< Collection* > m_collections;
    QList< Type* > m_types;


public:
    using NodeFactory::NodeFactory;

    CollectionFactory();
    int nodeMetatypeId() const;
    void importNode(QDataStream &in, Node *node);
    void exportNode(QDataStream &out, Node *node) const;
public slots:

    void createTypeConnection( CollectionHandle ch, TypeHandle typeHandle );
    void removeTypeConnection( CollectionHandle ch );

protected:
    void setupNode(Node *);
};

DECLARE_NODE(Collection)
#endif // COLLECTION_H
