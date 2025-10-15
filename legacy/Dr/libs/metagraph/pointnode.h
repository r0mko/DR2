#ifndef POINTNODE_H
#define POINTNODE_H

#include "node.h"

#include <QPointF>
#include <QHash>
#include <QPolygonF>
#include <QTransform>

uint qHash(const QPointF &key, uint seed);

class PointNode : public Node
{
    METAGRAPH_NODE(PointNode)
public:
    using Node::Node;
    int typeId() const;

    QPointF m_point;
};


class PointNodeHandle : public NodeHandle
{
    Q_GADGET
    NODE_FN(PointNode)

    Q_PROPERTY(QPointF point READ point)

public:
    using NodeHandle::NodeHandle;
    QPointF point() const;
};

class PointNodeFactory : public NodeFactory
{
    Q_OBJECT

    friend class PointNodeHandle;

    QHash<QPointF, PointNode*> m_nodeHash;

public:
    using NodeFactory::NodeFactory;

    int nodeMetatypeId() const;
    void importNode(QDataStream &in, Node *node);
    void exportNode(QDataStream &out, Node *node) const;

public slots:
    void addPolygon(const QPolygonF& polygon, QVariantMap attributes = QVariantMap(), const QTransform& transform = QTransform() );
    void test();
    void test2();

protected:
    void setupNode(Node *node);

};

DECLARE_NODE(PointNode)
Q_DECLARE_METATYPE(QPainterPath)


#endif // POINTNODE_H
