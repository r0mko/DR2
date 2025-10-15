#ifndef GRAPH_H
#define GRAPH_H

#include <QObject>
#include <QList>
#include <QLinkedList>
#include <QVariant>
#include "defs.h"
#include <QTransform>
#include <QStandardItemModel>
class TreeNode
{
    friend class Graph;
    QPointF m_point;
    QList<int> m_incomingEdgeList;
    QList<int> m_outgoingEdgeList;

public:
    QPointF point() const;
    QList<int> incomingEdgeList() const;
    QList<int> outgoingEdgeList() const;

    void appendIncomingEdge(int edgeId);
    void appendOutgoingEdge(int edgeId);

    void setPoint(const QPointF &point);
    void setIncomingEdgeList(const QList<int> &edgeList);
    void setOutgoingEdgeList(const QList<int> &edgeList);
    void changeEdge( int from, int to );
};

class Edge
{
    friend class Graph;
    QVariant m_userData;
    int m_startNode;
    QVariant m_beginGroupingFeature;
    int m_endNode;
    QVariant m_endGroupingFeature;
    bool m_oneway;
    qreal m_polygonLength;
    QPolygonF m_polygon;

public:
    int startNode() const;
    int endNode() const;
    bool oneway() const;

    void setStartNode(const int &startNode);
    void setEndNode(const int &endNode);
    void setOneway(bool oneway);
    void mergePolygon( const QPolygonF& p );

//    QPointF pointAtLength(qreal pos, int *nodeNo = nullptr) const;
//    QPointF pointAtPercent(qreal percent) const;

    void setBeginGroupingFeature(const QVariant &beginGroupingFeature);
    void setEndGroupingFeature(const QVariant &endGroupingFeature);
    void setPolygon(const QPolygonF &polygon);
    void setUserData(const QVariant &userData);

    QVariant beginGroupingFeature() const;
    QVariant endGroupingFeature() const;
    QPolygonF polygon() const;
    QVariant userData() const;
};

class Graph : public QObject
{
    Q_OBJECT
    QHash< int, TreeNode > m_nodeHash;
    QHash< int, Edge > m_edgeHash;
    QHash< QPair< QPointF, QVariant >, int > m_nodeTempHash;
    Q_PROPERTY(QVariantList list READ list WRITE setList NOTIFY listChanged)

public:
    explicit Graph( QObject* parent = nullptr );


    QVariantList list() const
    {
        return m_list;
    }

public slots:
    void addEdge(QVariant geometryBlob, QVariant data, bool oneway, const QTransform& transform = QTransform(), const QVariant& beginGroupingFeature = QVariant::Invalid, const QVariant& endGroupingFeature = QVariant::Invalid);

    void simplify();


    void setList(QVariantList arg)
    {
        if (m_list == arg)
            return;

        m_list = arg;
        emit listChanged(arg);
    }

signals:
    void listChanged(QVariantList arg);

private:
    void appendEdge(const QPolygonF &lineString, bool oneway, QVariant data, const QVariant& beginGroupingFeature, const QVariant& endGroupingFeature);

    QVariantList m_list;
};

#endif // GRAPH_H
