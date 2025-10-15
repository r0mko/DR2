#include "pointnode.h"
#include "graphmodel.h"

#include <QJSValueIterator>

uint qHash(const QPointF &key, uint seed)
{
    uint h1 = qHash((quint64)key.x(), seed);
    uint h2 = qHash((quint64)key.y(), seed);
    return ((h1 << 16) | (h1 >> 16)) ^ h2 ^ seed;
}

int PointNode::typeId() const
{
    return SELF_TYPEID;
}

QPointF PointNodeHandle::point() const
{
    return node()->m_point;
}

int PointNodeFactory::nodeMetatypeId() const
{
    return qMetaTypeId<PointNode>();
}

void PointNodeFactory::importNode(QDataStream &in, Node *node)
{
    in >> static_cast<PointNode*>(node)->m_point;
    NodeFactory::importNode(in, node);
}

void PointNodeFactory::exportNode(QDataStream &out, Node *node) const
{

    out << static_cast<PointNode*>(node)->m_point;
    NodeFactory::exportNode(out, node);
}

void PointNodeFactory::addPolygon(const QPolygonF &polygon, QVariantMap attributes, const QTransform &transform)
{
    PointNode* beginNode{nullptr};
    QPolygonF transformedPolygon = transform.map( polygon );

    auto iterator = m_nodeHash.find( transformedPolygon.first() );
    if( iterator == m_nodeHash.end() ) {
        beginNode = static_cast<PointNode*>( createNode() );
        beginNode->m_point = transformedPolygon.first();
        model()->appendNode( beginNode, rootIndex() );
        m_nodeHash.insert( transformedPolygon.first(), beginNode );
    }else{
        beginNode = iterator.value();
    }

    PointNode* endNode{nullptr};
    iterator = m_nodeHash.find( transformedPolygon.last() );
    if( iterator == m_nodeHash.end() ) {
        endNode = static_cast<PointNode*>( createNode() );
        endNode->m_point = transformedPolygon.last();
        model()->appendNode( endNode, rootIndex() );
        m_nodeHash.insert( transformedPolygon.last(), endNode );
    }else{
        endNode = iterator.value();
    }

    if( beginNode == nullptr || endNode == nullptr ) {
        qWarning() << "something terrible happened";
        exit(1);
    }

    auto beginNodeIndex = model()->getNodeIndex( beginNode );
    auto endNodeIndex = model()->getNodeIndex( endNode );
    QVariantMap edgeData;
    edgeData["polygon"] = transformedPolygon;
    edgeData["attributes"] = attributes;

//    edgeData["attributes"] =

    model()->addEdge( beginNodeIndex, endNodeIndex, "polygon", edgeData );
}

void PointNodeFactory::test()
{
    qDebug() << "MY TEST IS HERE";
    QPolygonF p;
    p << QPointF( 0, 5 ) << QPointF( 1, 5 ) << QPointF( 2, 5 ) << QPointF( 3, 5 ) << QPointF( 4, 5 ) << QPointF( 5, 5 ) << QPointF( 6, 5 );
    addPolygon( p );
}

void PointNodeFactory::test2()
{
    qDebug() << "MY TEST2 IS HERE";
    QPolygonF p;
    p << QPointF( 6, 5 ) << QPointF( 1, 6 ) << QPointF( 2, 6 ) << QPointF( 3, 7 ) << QPointF( 4, 8 ) << QPointF( 5, 9 ) << QPointF( 12, 9 );
    addPolygon( p );
}

void PointNodeFactory::setupNode(Node *node)
{
//    PointNode *b = static_cast<PointNode*>(node);
}


