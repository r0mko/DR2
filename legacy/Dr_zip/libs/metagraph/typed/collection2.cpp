#include "collection2.h"
#include "collectionobject.h"
#include "edge.h"
#include "graphmodel.h"
#include <QJSEngine>
#include <QJSValueIterator>
#include <QJsonDocument>
#include <qdebug.h>
#include <QMetaProperty>
#include "type.h"

Collection2Model::Collection2Model(const QModelIndex &rootIndex, const QMetaObject *type, QObject *parent) :
    NodeChildrenModel(parent),
    m_elementType(type)
{
    setRootIndex(rootIndex);
    if (m_elementType)
        m_metaTypeId = QMetaType::type(QByteArray(m_elementType->className()).append("*"));
}

QHash<int, QByteArray> Collection2Model::roleNames() const
{
    auto ret = NodeChildrenModel::roleNames();
    if (m_metaTypeId == QMetaType::UnknownType) {
        qWarning() << "type" << m_elementType->className() << "not registered";
        return ret;
    }
    int tid = m_metaTypeId << 16;
    for (int i = 0; i < m_elementType->propertyCount(); ++i) {
        ret[tid | i] = m_elementType->property(i).name();
    }
    return ret;
}

QVariant Collection2Model::data(const QModelIndex &index, int role) const
{
    int tid = role >> 16;
    if (!tid)
        return NodeChildrenModel::data(index, role);
    if (tid != m_metaTypeId) {
        qWarning() << "wrong role! Role's type is" << QMetaType::typeName(tid) << "current is" << m_elementType->className();
        return QVariant();
    }
    int pidx = role & 0xffff;
    Instance *inst = static_cast<Instance*>(sourceModel()->getNode(mapToSource(index)));
    if (!inst) {
        qWarning() << "Error! Null node at index" << index;
        return QVariant();
    }
    return inst->getProperty(pidx);
}

int Collection2Model::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return sourceModel()->rowCount(rootIndex());
}

QString Collection2Model::typeName() const
{
    if (!m_elementType)
        return QString();
    return m_elementType->className();
}

QModelIndex Collection2Model::addInstance(const QJSValue &init)
{
    Instance *node = new Instance;
    node->resize(m_elementType->propertyCount());
    QJSValueIterator it(init);
    while (it.hasNext()) {
        it.next();
        int id = m_elementType->indexOfProperty(it.name().toLocal8Bit().constData());
        if (id == -1) {
            debugPrint << ": property" << it.name() << "doesn't exist in class" << m_elementType->className();
            continue;
        }
        QVariant value = it.value().toVariant();
        int ptype = m_elementType->property(id).userType();

        if (!value.canConvert(ptype)) {
            debugPrint << ": unable to assign" << value.typeName() << "to property type" << QMetaType::typeName(ptype);
            continue;
        }
        value.convert(ptype);
        node->setProperty(id, value);
    }
    return sourceModel()->appendNode(node, rootIndex());
}


QObject *Collection2Model::objectAt(int row)
{
    Instance *inst = static_cast<Instance*>(sourceModel()->getNode(sourceModel()->index(row, 0, rootIndex())));
    return inst->wrapper();
}

void Collection2Model::printRoles() const
{
    qDebug() << "Model" << typeName() << "roles:" << roleNames().values();
}

Collection2Model *Collection2::cmodel() const
{
    return m_model.data();
}

int Collection2::collectionSize() const
{
    return m_model.data()->count();
}

bool Collection2::isActive() const
{
    return outNodes("type").size() == 1;
}


//int Collection::collectionSize() const
//{
//    return m_model.data()->count();
//}

//bool Collection::isActive() const
//{
//    return false;
//}

const QMetaObject *Collection2::elementMetaObject() const
{
    if (!m_model)
        return nullptr;
    return m_model->m_elementType;
}

//bool Collection::nodeChange(Node::NodeChange change, const Node::NodeChangeData &data)
//{
////    qDebug() << " =======> nodeChange";
//    switch (change) {
//    case AddOutEdge:
//        if( data.edgeValue()->tag() == "type" ) {
//            if( isActive() ){

////            qDebug() << " ==>> attempt to add type edge to active collection";
//                return false;
//            } else {
//                Type* t = dynamic_cast<Type*>( data.edgeValue()->endNode() );
//                if( t != nullptr ) {
//                    m_model = QSharedPointer<CollectionModel>( new CollectionModel(

//                                                                   i, TypeFactory::references.value(t->className().toLocal8Bit()) ), &CollectionModel::deleteLater);
//                }

//            }
//        }
//        break;
//    default:
//        break;
//    }
//    return Node::nodeChange( change, data );
//}

Collection2::Collection2()
{}


void Collection2::initModel(const QModelIndex &rootIndex, const QMetaObject *elementType, GraphModel *)
{
    m_model.reset(new Collection2Model(rootIndex, elementType, const_cast<GraphModel*>(qobject_cast<const GraphModel*>(rootIndex.model()))));
}


TypeHandle Collection2Handle::elementType() const
{
    for( const QVariant& e: outEdges() )
        if( e.value<EdgeHandle>().hasTag("collection") )
            return e.value<EdgeHandle>().endNode().toVariant().value<TypeHandle>();
    return TypeHandle();

//    QVariantList tnodes = outNodes("type");
//    if (tnodes.isEmpty())
//        return TypeHandle();
//    if (tnodes.size() > 1) {
////        qDebug() << "collection" << model()->getNodeId(d) << "has more than one link tagged 'type':";
//        for (QVariant t : tnodes) {
//            qDebug() << t;
//        }
//        qDebug() << "the first one will be used";
//    }
//    return tnodes.first().value<TypeHandle>();
}

InstanceHandle Collection2Handle::addInstance(QJSValue init)
{
//    if( node()->model() == nullptr )
//        node()->initModel(index(), node()->elementMetaObject(), nullptr );

    QModelIndex idx = node()->cmodel()->addInstance(init);
    return model()->getHandle(idx).value<InstanceHandle>();
}

Collection2Model *Collection2Handle::items() const
{
    return node()->cmodel();
}

void Collection2Handle::setElemetType(TypeHandle arg)
{
    // FIXME: change to flagged edges

    // chek if there is already an edge to type node with type flag
    for( const QVariant& e: outEdges() )
        if( e.value<EdgeHandle>().hasTag("collection") )
            return;
//    if( !outNodes("type").isEmpty() ) return;


    // if no then create flagged edge to arg
    model()->addEdge( *this, arg, "type", QVariant() );
    // construct collectionModel
    node()->m_model = QSharedPointer<Collection2Model>(
                new Collection2Model( modelIndex(), TypeFactory::references.value( arg.className().toLocal8Bit() ) ),
                &Collection2Model::deleteLater
                );
}

TypeHandle Collection2Factory::elementType() const
{
    return m_elementType;
}

Collection2Handle Collection2Factory::create(const QString &nodeId)
{
    if (!model())
        return Collection2Handle();
    Collection2Handle n = model()->createNodeHelper<Collection2>(parentHandle());
    if (m_elementType.isValid())
        model()->addEdge(n, m_elementType, "type");
    if (!nodeId.isEmpty())
        n.setNodeId(nodeId);
    else

        n.setNodeId(NodeFactory::nodeId());


    return n;
}

void Collection2Factory::setElementType(TypeHandle elementType)
{
    if (m_elementType == elementType)
        return;

    m_elementType = elementType;
    emit elementTypeChanged(elementType);
}


int Collection2Factory::nodeMetatypeId() const
{
    return qMetaTypeId<Collection2>();
}

void Collection2Factory::importNode(QDataStream &in, Node *node)
{
    Collection2 *n = static_cast<Collection2*>(node);
    NodeFactory::importNode(in, node);
//    model()->requestInitCollection(n);
}

void Collection2Factory::exportNode(QDataStream &out, Node *node) const
{
    NodeFactory::exportNode(out, node);
}



