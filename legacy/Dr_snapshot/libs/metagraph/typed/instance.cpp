#include "collectionobject.h"
#include "instance.h"
#include "collection2.h"
#include <QDataStream>
#include <QDebug>
#include "qqml.h"
#include "qqmlengine.h"
#include "graphmodel.h"
#include "edge.h"



Collection2 *Instance::collection() const { return const_cast<Collection2*>(static_cast<const Collection2*>(parent())); }

QVariant Instance::getProperty(int index) const
{
    return m_propertyValues.at(index);
}

void Instance::setProperty(int index, const QVariant &value)
{
    m_propertyValues[index] = value;
}

void Instance::resize(int count)
{
    m_propertyValues.resize(count);
}

ElementAgent *Instance::wrapper()
{
    if (i.isNull()) {
//        i = new ElementAgent(collection()->elementMetaObject(), this, collection()->cmodel());
    }
    qmlEngine(collection()->cmodel())->setObjectOwnership(i.data(), QQmlEngine::JavaScriptOwnership);
    return i;
}

QDataStream &operator <<(QDataStream &out, const QVector<QVariant> &v)
{
    out << v.size();
    for (const QVariant &var : v) {
        out << var;
    }
    return out;
}

QDataStream &operator >>(QDataStream &in, QVector<QVariant> &v)
{
    int s;
    in >> s;
    v.clear();
    v.reserve(s);
    while (s--) {
        QVariant var;
        in >> var;
        v.append(var);
    }
    return in;
}

ElementAgent *InstanceHandle::instance() const
{
    return node()->wrapper();
}

TypeHandle InstanceHandle::elementType() const
{
    QVariantList tnodes = model()->getHandle(modelIndex().parent()).value<NodeHandle>().outNodes("type");
    if (tnodes.isEmpty())
        return TypeHandle();
    if (tnodes.size() > 1) {
        qDebug() << "instance" << model()->getNodeId(d) << "has more than one link tagged 'type':";
        for (QVariant t : tnodes) {
            qDebug() << t;
        }
        qDebug() << "the first one will be used";
    }
    return tnodes.first().value<TypeHandle>();
}


int InstanceFactory::nodeMetatypeId() const
{
    return qMetaTypeId<Instance>();
}

void InstanceFactory::importNode(QDataStream &in, Node *node)
{
    Instance *n = static_cast<Instance*>(node);
    in >> n->m_propertyValues;
    NodeFactory::importNode(in, node);
}

void InstanceFactory::exportNode(QDataStream &out, Node *node) const
{
    Instance *n = static_cast<Instance*>(node);
    out << n->m_propertyValues;
    NodeFactory::exportNode(out, node);
}

