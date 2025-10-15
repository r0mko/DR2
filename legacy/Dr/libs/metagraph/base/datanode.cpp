#include "datanode.h"
#include <QDebug>
#include "graphmodel.h"
#include "qqml.h"

#include <QQmlEngine>

/*!
    \class DataNode
    \inmodule MetaGraph
    \brief DataNode
    \ingroup Nodes

    Fix me
*/

void DataNode::setData(const QVariant &data)
{
//    qjsEngine(
//    qDebug() << "set data" << data;
    if (data.userType() == qMetaTypeId<QJSValue>())
        m_data = QJsonValue::fromVariant(data.value<QJSValue>().toVariant());
    else
        m_data = QJsonValue::fromVariant(data);
}

QVariant DataNode::data() const
{
//    qDebug() << "get vdata" << m_data.toVariant();
    return m_data;
//    return m_data.toVariant();
}

int DataNode::typeId() const { return SELF_TYPEID; }

QJsonValue DataNodeHandle::data() const
{
    auto n = node();
    if (!n)
        return QJsonValue();
    return node()->json();
}

void DataNodeHandle::setData(QJsonValue arg)
{
    auto n = node();
    if (!n)
        return;
    n->setJson(arg);
    model()->notify(d, { GraphModel::HandleRole, GraphModel::DataRole });
}

QQmlPropertyMap *DataNodeHandle::dataHandle() const
{
    idxCheck(nullptr);

    QJsonValue v = node()->json();
    QJsonObject dmap = v.toObject();

    QQmlPropertyMap* map = new QQmlPropertyMap();
    qmlEngine(model())->setObjectOwnership(map, QQmlEngine::JavaScriptOwnership);

    QJsonObject::const_iterator it = dmap.constBegin();
    while (it != dmap.constEnd()) {
        map->insert(it.key(), it.value().toVariant());
        it++;
    };

    QObject::connect(map, &QQmlPropertyMap::destroyed, [this]() { qDebug() << "map destroyed"; });
    QObject::connect(map, &QQmlPropertyMap::valueChanged, [this](const QString &key, const QVariant &value) {
        idxCheck();
//        qDebug() << "before" << node()->json().toVariant();
        QJsonObject obj = node()->m_data.toObject();
        obj[key] = QJsonValue::fromVariant(value);
        node()->m_data = obj;
//        qDebug() << "after" << node()->json().toVariant();
//        dmap[key] = value;
        model()->notify(d, { GraphModel::HandleRole, GraphModel::DataRole });
    });
    return map;
}


void DataNodeFactory::setData(QJsonValue data)
{
    m_data = data;
    emit dataChanged();
}


void DataNodeFactory::setupNode(Node *node)
{
    DataNode *dn = static_cast<DataNode*>(node);
    dn->setJson(m_data);

}

int DataNodeFactory::nodeMetatypeId() const
{
    return qMetaTypeId<DataNode>();
}

void DataNodeFactory::importNode(QDataStream &in, Node *node)
{
    DataNode *n = static_cast<DataNode*>(node);
    QVariant data;
    in >> data;
    n->m_data = QJsonValue::fromVariant(data);
    NodeFactory::importNode(in, node);
}

void DataNodeFactory::exportNode(QDataStream &out, Node *node) const
{
    DataNode *n = static_cast<DataNode*>(node);
    QVariant v = n->m_data.toVariant();
    out << v;
    NodeFactory::exportNode(out, node);
}
