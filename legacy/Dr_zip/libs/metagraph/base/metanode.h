#ifndef METANODEHANDLE_H
#define METANODEHANDLE_H
#include "node.h"

class MetaNode : public Node
{
    METAGRAPH_NODE(MetaNode)
//    QList<Node*> m_members;
    QList<QPersistentModelIndex> m_members;

public:
//    void appendMember(Node* m){ m_members.append(m);}
    void appendMember(QPersistentModelIndex m){ m_members.append(m);}
    using Node::Node;
};

class MetaNodeHandle : public NodeHandle
{
    Q_GADGET
    NODE_FN(MetaNode)
    Q_PROPERTY(QVariantList members READ members)

public:
    using NodeHandle::NodeHandle;
    Q_INVOKABLE void addMember(QVariant member);
    QVariantList members() const;
private:
};

class MetaNodeFactory : public NodeFactory
{
    Q_OBJECT
    Q_PROPERTY(QVariantList members READ members WRITE setMembers NOTIFY membersChanged)

    QVariantList m_members;

public:
    MetaNodeFactory(QObject *parent = 0);
    virtual ~MetaNodeFactory() {}

    inline QVariantList members() const { return m_members; }
    void setMembers(QVariantList members);
    int nodeMetatypeId() const;
    void importNode(QDataStream &in, Node *node);
    void exportNode(QDataStream &out, Node *node) const;
public slots:
    void addMember(QVariant handle);

signals:
    void membersChanged();

protected:
    void setupNode(Node *);
};

DECLARE_NODE(MetaNode)
#endif // METANODEHANDLE_H
