#ifndef CATEGORYNODE_H
#define CATEGORYNODE_H
#include "node.h"
#include "type.h"
#include <qobject.h>
#include "instance.h"
#include "nodechildrenmodel.h"
#include <QDebug>
#include <QJSValue>
class ElementAgent;
class Collection2Model : public NodeChildrenModel
{
    Q_OBJECT

    Q_PROPERTY(QString typeName READ typeName CONSTANT)
    const QMetaObject *m_elementType = nullptr;
    int m_metaTypeId = QMetaType::UnknownType;

public:
    friend class Collection2;
    explicit Collection2Model(const QModelIndex &rootIndex, const QMetaObject *type, QObject *parent = 0);
    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;
    QString typeName() const;

public slots:
    QModelIndex addInstance(const QJSValue &init = QJSValue());
    QObject *objectAt(int row);
    void printRoles() const;
};


class Collection2 : public Node
{
    METAGRAPH_NODE(Collection2)
    QSharedPointer<Collection2Model> m_model;
public:
    Collection2();
    virtual int typeId() const;

    void initModel(const QModelIndex &rootIndex, const QMetaObject *elementType, GraphModel *source);
    Collection2Model* cmodel() const;


    int collectionSize() const;
    bool isActive() const;

    const QMetaObject *elementMetaObject() const;

};

class Collection2Handle : public NodeHandle
{
    Q_GADGET
    NODE_FN(Collection2)
    Q_PROPERTY(TypeHandle elementType READ elementType WRITE setElemetType )
    Q_PROPERTY(Collection2Model* items READ items)


public:
    using NodeHandle::NodeHandle;
    TypeHandle elementType() const;
    Q_INVOKABLE InstanceHandle addInstance(QJSValue init = QJSValue());
    Collection2Model* items() const;

public slots:
    void setElemetType(TypeHandle arg);
};

class Collection2Factory : public NodeFactory
{
    Q_OBJECT
    Q_PROPERTY(TypeHandle elementType READ elementType WRITE setElementType NOTIFY elementTypeChanged)
    TypeHandle m_elementType;

public:
    using NodeFactory::NodeFactory;

    TypeHandle elementType() const;
    int nodeMetatypeId() const;
    void importNode(QDataStream &in, Node *node);
    void exportNode(QDataStream &out, Node *node) const;

public slots:
    Collection2Handle create(const QString &nodeId = QString());
    void setElementType(TypeHandle elementType);
signals:
    void elementTypeChanged(TypeHandle elementType);
};

DECLARE_NODE(Collection2)
#endif // CATEGORYNODE_H
