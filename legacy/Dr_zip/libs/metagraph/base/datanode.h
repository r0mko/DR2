#ifndef JSNODE_H
#define JSNODE_H
#include <QJsonValue>

#include <QQmlPropertyMap>
#include "node.h"

class DataNode : public Node
{
    QJsonValue m_data;
    METAGRAPH_NODE(DataNode)
public:
    using Node::Node;
    inline const QJsonValue &json() const { return m_data; }
    inline void setJson(const QJsonValue &value) { m_data = value; }
    void setData(const QVariant &data);
    QVariant data() const;
};

class DataNodeHandle : public NodeHandle
{
    NODE_FN(DataNode)
    Q_GADGET
    Q_PROPERTY(QJsonValue data READ data WRITE setData)
    Q_PROPERTY(QQmlPropertyMap* dataHandle READ dataHandle)

    QQmlPropertyMap* m_dataHandle;

public:
    using NodeHandle::NodeHandle;
    QJsonValue data() const;
    void setData(QJsonValue arg);
    QQmlPropertyMap* dataHandle() const;
};

class DataNodeFactory : public NodeFactory
{
    Q_OBJECT
    Q_PROPERTY(QJsonValue data READ data WRITE setData NOTIFY dataChanged)
    QJsonValue m_data;
public:
    using NodeFactory::NodeFactory;
    inline QJsonValue data() const { return m_data; }
    void setData(QJsonValue data);
    int nodeMetatypeId() const;
    void importNode(QDataStream &in, Node *node);
    void exportNode(QDataStream &out, Node *node) const;

signals:
    void dataChanged();
protected:
    void setupNode(Node *node);

};
Q_DECLARE_METATYPE(QJsonValue)
DECLARE_NODE(DataNode)
#endif // JSNODE_H
