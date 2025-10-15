#ifndef SQLBRIDGE_H
#define SQLBRIDGE_H

#include "node.h"
#include "graphmodel.h"

#include <QVector2D>


class SqlBridgeNode : public Node
{
    METAGRAPH_NODE(SqlBridgeNode)
public:
    using Node::Node;


};


class SqlBridgeNodeHandle : public NodeHandle
{
    Q_GADGET
    NODE_FN(SqlBridgeNode)



public:
    using NodeHandle::NodeHandle;


    Q_INVOKABLE void test( NodeHandle h ){
        qDebug() << "MYTEST" << h;
        QVector<QVector2D> data;
        data << QVector2D( 1.0, 2.0 )
             << QVector2D( 3.0, 2.0 )
             << QVector2D( 4.0, 4.0 );
        model()->addEdge( modelIndex(), h.modelIndex(), "route", QVariant::fromValue( data ) );
    }


};

class SqlBridgeNodeFactory : public NodeFactory
{
    Q_OBJECT

    friend class SqlBridgeNodeHandle;
public:
    using NodeFactory::NodeFactory;

    int nodeMetatypeId() const;
    void importNode(QDataStream &in, Node *node);
    void exportNode(QDataStream &out, Node *node) const;

protected:
    void setupNode(Node *) {}

};
DECLARE_NODE(SqlBridgeNode)

#endif // SQLBRIDGE_H
