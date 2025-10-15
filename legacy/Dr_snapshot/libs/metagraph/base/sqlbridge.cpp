#include "sqlbridge.h"
#include "graphmodel.h"
//#include "qqml.h"
//#include <QQmlEngine>


int SqlBridgeNodeFactory::nodeMetatypeId() const
{
    return qMetaTypeId<SqlBridgeNode>();
}

void SqlBridgeNodeFactory::importNode(QDataStream &in, Node *node)
{
//    SqlBridgeNode *n = static_cast<SqlBridgeNode*>(node);
//    QVariant data;
//    in >> data;
//    n->m_data = qmlEngine(this)->toScriptValue(data);
    NodeFactory::importNode(in, node);
}

void SqlBridgeNodeFactory::exportNode(QDataStream &out, Node *node) const
{
//    SqlBridgeNode *n = static_cast<SqlBridgeNode*>(node);
//    QVariant v = n->m_data.toVariant();
//    out << v;
    NodeFactory::exportNode(out, node);
}


