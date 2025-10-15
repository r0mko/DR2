#ifndef GRAPHGENERATOR_H
#define GRAPHGENERATOR_H

#include <QObject>
#include <QVariantList>
class GeomBuffer;
class SpatialQuery;
class GraphGenerator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList nodes READ nodes WRITE setNodes NOTIFY nodesChanged)
    Q_PROPERTY(QVariantList edges READ edges WRITE setEdges NOTIFY edgesChanged)
    Q_PROPERTY(SpatialQuery* queryData READ queryData WRITE setQueryData NOTIFY queryDataChanged)

    SpatialQuery* m_queryData;
    QVariantList m_edges;
    QVariantList m_nodes;

public:
    explicit GraphGenerator(QObject *parent = 0);

    SpatialQuery* queryData() const;
    void setQueryData(SpatialQuery* queryData);

    QVariantList edges() const;
    void setEdges(QVariantList edges);

    QVariantList nodes() const;
    void setNodes(QVariantList nodes);

signals:
    void queryDataChanged(SpatialQuery* queryData);
    void edgesChanged(QVariantList edges);
    void nodesChanged(QVariantList nodes);

public slots:
    void buildGraph();
};

#endif // GRAPHGENERATOR_H
