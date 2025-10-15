#include "graphgenerator.h"

GraphGenerator::GraphGenerator(QObject *parent) : QObject(parent)
{

}

SpatialQuery *GraphGenerator::queryData() const
{
    return m_queryData;
}

void GraphGenerator::setQueryData(SpatialQuery *queryData)
{
    if (m_queryData == queryData)
        return;

    m_queryData = queryData;
    emit queryDataChanged(queryData);
}

QVariantList GraphGenerator::edges() const
{
    return m_edges;
}

void GraphGenerator::setEdges(QVariantList edges)
{
    if (m_edges == edges)
        return;

    m_edges = edges;
    emit edgesChanged(edges);
}

QVariantList GraphGenerator::nodes() const
{
    return m_nodes;
}

void GraphGenerator::setNodes(QVariantList nodes)
{
    if (m_nodes == nodes)
        return;

    m_nodes = nodes;
    emit nodesChanged(nodes);
}

void GraphGenerator::buildGraph()
{

}

