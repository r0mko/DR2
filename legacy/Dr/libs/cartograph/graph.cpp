#include "graph.h"
#include "wkt.h"
#include "mapview.h"
#include <QPair>
#include <QDebug>
#include <QVariantMap>

uint qHash( const QVariant & var )
{
    if ( !var.isValid() || var.isNull() )
        //        return -1;
        Q_ASSERT(0);

    switch ( var.type() )
    {
    case QVariant::Int:
        return qHash( var.toInt() );
        break;
    case QVariant::UInt:
        return qHash( var.toUInt() );
        break;
    case QVariant::Bool:
        return qHash( var.toUInt() );
        break;
    case QVariant::Double:
        return qHash( var.toUInt() );
        break;
    case QVariant::LongLong:
        return qHash( var.toLongLong() );
        break;
    case QVariant::ULongLong:
        return qHash( var.toULongLong() );
        break;
    case QVariant::String:
        return qHash( var.toString() );
        break;
    case QVariant::Char:
        return qHash( var.toChar() );
        break;
    case QVariant::StringList:
        return qHash( var.toString() );
        break;
    case QVariant::ByteArray:
        return qHash( var.toByteArray() );
        break;
    case QVariant::Date:
    case QVariant::Time:
    case QVariant::DateTime:
    case QVariant::Url:
    case QVariant::Locale:
    case QVariant::RegExp:
        return qHash( var.toString() );
        break;
    case QVariant::Map:
    case QVariant::List:
    case QVariant::BitArray:
    case QVariant::Size:
    case QVariant::SizeF:
    case QVariant::Rect:
    case QVariant::LineF:
    case QVariant::Line:
    case QVariant::RectF:
    case QVariant::Point:
    case QVariant::PointF:
        // not supported yet
        break;
    case QVariant::UserType:
    case QVariant::Invalid:
    default:
        return -1;
    }

    // could not generate a hash for the given variant
    return -1;
}

Graph::Graph(QObject *parent):
    QObject( parent )
{

}

void Graph::addEdge(QVariant geometryBlob, QVariant data, bool oneway, const QTransform &transform, const QVariant &beginGroupingFeature, const QVariant &endGroupingFeature)
{
    QScopedPointer<Geometry> geom(new Geometry);
    QByteArray ba = geometryBlob.toByteArray();
    if (ba.isEmpty())
        return;
    geom->parseBlob(ba);
    for (GeoEntity *e : geom->data())
        for( const QPolygonF& p: e->getPath( transform ).toSubpathPolygons() )
            appendEdge(p, oneway, data, beginGroupingFeature, endGroupingFeature );
}



void Graph::simplify()
{
    m_nodeTempHash.clear();

    QList< int > nodes;
    QList< int > edges;
    int x = -1;
    for( int i: m_nodeHash.keys() ){
        if( m_nodeHash.value( i ).incomingEdgeList().size() == 1 && m_nodeHash.value( i ).outgoingEdgeList().size() == 1 ){


            TreeNode n = m_nodeHash.value( i );

            int eidxin = n.incomingEdgeList().first();
            int eidxout = n.outgoingEdgeList().first();
            if( eidxin == eidxout )
                continue;
            nodes.append( i );
            edges.append( eidxout );
            Edge& e1 = m_edgeHash[ eidxin ];
            const Edge& e2 = m_edgeHash[ eidxout ];
            //            if( x == -1 ){
            //                qDebug() << "\nbefore" << "oneway" << e1.polygon().size() << e2.polygon().size() << e1.polygon() << e2.polygon();
            //                x = eidxin;
            //            }
            e1.mergePolygon( e2.polygon() );
            e1.setEndNode( e2.endNode() );
            m_nodeHash[ e2.endNode() ].changeEdge( eidxout, eidxin );
            //            if( x == eidxin ){
            //                qDebug() << "\nafter" << "oneway" << e1.polygon().size() << e2.polygon().size() << e1.polygon() << e2.polygon();
            //                x = eidxin;
            //            }
            //            if( x == eidxout ){
            //                qDebug() << "out" << "oneway" << e1.polygon().size() << e2.polygon().size() << e1.polygon() << e2.polygon();
            //                x = eidxin;
            //            }
        }
        else if( m_nodeHash.value( i ).incomingEdgeList().size() == 2 && m_nodeHash.value( i ).outgoingEdgeList().size() == 2 )
        {
            if( (m_nodeHash.value( i ).incomingEdgeList().at( 0 ) == m_nodeHash.value( i ).outgoingEdgeList().at( 1 ) || m_nodeHash.value( i ).incomingEdgeList().at( 0 ) == m_nodeHash.value( i ).outgoingEdgeList().at( 0 )) && (m_nodeHash.value( i ).incomingEdgeList().at( 1 ) == m_nodeHash.value( i ).outgoingEdgeList().at( 1 ) || m_nodeHash.value( i ).incomingEdgeList().at( 1 ) == m_nodeHash.value( i ).outgoingEdgeList().at( 0 )))
            {
                TreeNode n = m_nodeHash.value( i );
                int eidxin = n.incomingEdgeList().first();
                int eidxout = n.incomingEdgeList().last();
                nodes.append( i );
                edges.append( eidxout );
                Edge& e1 = m_edgeHash[ eidxin ];
                const Edge& e2 = m_edgeHash[ eidxout ];
                if( x == -1 ){
                    qDebug() << "\nbefore" << "twoway" << e1.polygon().size() << e2.polygon().size() << e1.polygon() << e2.polygon();
                    x = eidxin;
                }
                e1.mergePolygon( e2.polygon() );
                e1.setEndNode( e2.endNode() );
                m_nodeHash[ e2.endNode() ].changeEdge( eidxout, eidxin );
                if( x == eidxin ){
                    qDebug() << "\nafter" << "twoway" << e1.polygon().size() << e2.polygon().size() << e1.polygon() << e2.polygon();
                    x = eidxin;
                }
                if( x == eidxout ){
                    qDebug() << "out" << "twoway" << e1.polygon().size() << e2.polygon().size() << e1.polygon() << e2.polygon();
                }
            }
        }
    }
    qDebug() << nodes.size() << edges.size();
    for( int n: nodes )
        m_nodeHash.remove( n );
    for( int e: edges )
        m_edgeHash.remove( e );

    qDebug() << m_nodeHash.size() << m_edgeHash.size();

    for( int i: m_edgeHash.keys() ){
        const Edge& e = m_edgeHash.value( i );
        QVariantMap m;
        m.insert( "polygonData", e.polygon() );
        m.insert( "oneway", e.oneway() );
        m.insert( "length", e.userData().toMap().value( "length" ).toReal() );
        m.insert( "testp", QString( "edge %1" ).arg( i ) );
        m_list.append( m );
    }
    //    qDebug() << "after" << m_edgeHash.value( x ).polygon();

}
void Graph::appendEdge(const QPolygonF &lineString, bool oneway, QVariant data, const QVariant &beginGroupingFeature, const QVariant &endGroupingFeature)
{
    QPair< QPointF, QVariant > pair1 = qMakePair( lineString.first(), beginGroupingFeature );
    int idx1 = m_nodeTempHash.value( pair1, -1 );
    if( idx1 == -1 )//if no such node;
    {
        idx1 = m_nodeTempHash.size();
        TreeNode n;
        n.setPoint( lineString.first() );
        m_nodeTempHash.insert( pair1, idx1 );
        m_nodeHash.insert( idx1, n );
    }
    QPair< QPointF, QVariant > pair2 = qMakePair( lineString.last(), endGroupingFeature );
    int idx2 = m_nodeTempHash.value( pair2, -1 );
    if( idx2 == -1 )//if no such node;
    {
        idx2 = m_nodeTempHash.size();
        TreeNode n;
        n.setPoint( lineString.first() );
        m_nodeTempHash.insert( pair2, idx2 );
        m_nodeHash.insert( idx2, n );
    }

    Edge e;
    int eidx = m_edgeHash.size();
    e.setStartNode( idx1 );
    e.setEndNode( idx2 );
    e.setOneway( oneway );
    e.setPolygon( lineString );
    e.setUserData( data );
    m_edgeHash.insert( eidx, e );

    m_nodeHash[ idx1 ].appendOutgoingEdge( eidx );
    m_nodeHash[ idx2 ].appendIncomingEdge( eidx );
    if (!oneway) {
        m_nodeHash[ idx1 ].appendIncomingEdge( eidx );
        m_nodeHash[ idx2 ].appendOutgoingEdge( eidx );
    }

}



QPointF TreeNode::point() const
{
    return m_point;
}

QList<int> TreeNode::incomingEdgeList() const
{
    return m_incomingEdgeList;
}

QList<int> TreeNode::outgoingEdgeList() const
{
    return m_outgoingEdgeList;
}

void TreeNode::appendIncomingEdge(int edgeId)
{
    m_incomingEdgeList.append( edgeId );
}

void TreeNode::appendOutgoingEdge(int edgeId)
{
    m_outgoingEdgeList.append( edgeId );
}

void TreeNode::setPoint(const QPointF &point)
{
    m_point = point;
}

void TreeNode::setIncomingEdgeList(const QList<int> &edgeList)
{
    m_incomingEdgeList = edgeList;
}

void TreeNode::setOutgoingEdgeList(const QList<int> &edgeList)
{
    m_outgoingEdgeList = edgeList;
}

void TreeNode::changeEdge(int from, int to)
{
    for( int& i: m_incomingEdgeList )
        if( i == from ) i = to;
    for( int& i: m_outgoingEdgeList )
        if( i == from ) i = to;
}



QVariant Edge::beginGroupingFeature() const
{
    return m_beginGroupingFeature;
}

void Edge::setBeginGroupingFeature(const QVariant &beginGroupingFeature)
{
    m_beginGroupingFeature = beginGroupingFeature;
}

QVariant Edge::endGroupingFeature() const
{
    return m_endGroupingFeature;
}

void Edge::setEndGroupingFeature(const QVariant &endGroupingFeature)
{
    m_endGroupingFeature = endGroupingFeature;
}

QPolygonF Edge::polygon() const
{
    return m_polygon;
}

void Edge::setPolygon(const QPolygonF &polygon)
{
    m_polygon = polygon;
}

QVariant Edge::userData() const
{
    return m_userData;
}

void Edge::setUserData(const QVariant &userData)
{
    m_userData = userData;
}
int Edge::startNode() const
{
    return m_startNode;
}

int Edge::endNode() const
{
    return m_endNode;
}

bool Edge::oneway() const
{
    return m_oneway;
}

void Edge::setStartNode(const int &startNode/*, const QVariant& abeginGroupingFeature*/)
{
    m_startNode = startNode;
    //    m_beginGroupingFeature = abeginGroupingFeature;
}

void Edge::setEndNode(const int &endNode/*, const QVariant &anendGroupingFeature*/)
{
    m_endNode = endNode;
    //    m_endGroupingFeature = anendGroupingFeature;
}

void Edge::setOneway(bool oneway)
{
    m_oneway = oneway;
}

void Edge::mergePolygon(const QPolygonF &p)
{
    for( int i = 1; i < p.size(); ++i )
        m_polygon << p.at( i );
}
