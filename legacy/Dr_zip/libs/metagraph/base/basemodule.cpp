#include "basemodule.h"

BaseModule::BaseModule(QObject *parent) : AbstractModule(parent)
{

}

void BaseModule::setParentNode(NodeHandle parent)
{
    setRoot(parent.modelIndex());
    emit parentChanged(parent);
}

void BaseModule::setNodeId(QString nodeId)
{
    if (m_nodeId == nodeId)
        return;

    m_nodeId = nodeId;
    emit nodeIdChanged(nodeId);
}

QVector<int> BaseModule::nodeMetatypes() const
{
    QVector<int> ret;
    ret = { };
    return ret;
}

QVector<int> BaseModule::edgeMetatypes() const
{
    QVector<int> ret;
    return ret;
}

NodeHandle BaseModule::parentNode() const
{
    return NodeHandle(root());
}

QString BaseModule::nodeId() const
{
    return m_nodeId;
}

