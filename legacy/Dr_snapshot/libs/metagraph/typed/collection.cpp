#include "collection.h"
#include "graphmodel.h"
#include "edge.h"
#include "QMetaType"
#include <QJSValueIterator>

int CollectionHandle::id_isStored = CollectionHandle::staticMetaObject.indexOfProperty("isStored") - CollectionHandle::staticMetaObject.propertyOffset();
int CollectionHandle::id_className = CollectionHandle::staticMetaObject.indexOfProperty("className") - CollectionHandle::staticMetaObject.propertyOffset();
int CollectionHandle::id_metaTypeId = CollectionHandle::staticMetaObject.indexOfProperty("metaTypeId") - CollectionHandle::staticMetaObject.propertyOffset();
int CollectionHandle::id_roleModel = CollectionHandle::staticMetaObject.indexOfProperty("roleModel") - CollectionHandle::staticMetaObject.propertyOffset();
int CollectionHandle::id_elements = CollectionHandle::staticMetaObject.indexOfProperty("elements") - CollectionHandle::staticMetaObject.propertyOffset();



CollectionFactory::CollectionFactory()
{
//    connect( model(), SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT( onRowsRemoved(QModelIndex,int,int) ) );
//    connect( model(), SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(onRowsMoved(QModelIndex,int,int,QModelIndex,int)));
//    connect( model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onDataChanged(QModelIndex,QModelIndex)));

}

int CollectionFactory::nodeMetatypeId() const
{
    return qMetaTypeId<Collection>();
}

void CollectionFactory::importNode(QDataStream &in, Node *node)
{
    Collection *n = static_cast<Collection*>(node);

    // TODO: get previous m_stored state; send it to requestInitCollection function and restore there if required;
    bool stored{false};
    in >> stored;
    NodeFactory::importNode(in, node);
    model()->requestInitCollection(n, stored);
//    model()->requestInitCollection(n);
}

void CollectionFactory::exportNode(QDataStream &out, Node *node) const
{
    // TODO: save m_stored state; store(); then export;
    auto c = static_cast<Collection*>(node);
    out << c->m_stored;
    if( !c->m_stored ) c->store();
    NodeFactory::exportNode(out, node);
}


void CollectionFactory::createTypeConnection(CollectionHandle ch, TypeHandle th )
{
    Collection *c = ch.node();
    if (c->m_connectedType == th.node()) {
        qDebug() << "that type already bound here - leaving";
        return;
    }
    bool wasStored = c->m_stored;


    removeTypeConnection(ch);

    qDebug() << "storing type* for quick access and adding edge";

    c->m_connectedType = th.node();
    model()->addEdge( ch, th, "type|collection", QVariant() );
    model()->notify( ch.modelIndex(), { GraphModel::NodeRoles::HandleRole } );

    if( th.isRegistered() ) {
        qDebug() << "type is registered thus we setting metaobject";
        c->m_metaobject = const_cast<QMetaObject*>( QMetaType::metaObjectForType( th.metaTypeId() ) );
        model()->notify(QPersistentModelIndex(ch.modelIndex()), { GraphModel::HandleRole, getRole(qMetaTypeId<Collection>(), CollectionHandle::id_metaTypeId) });
        model()->notify(QPersistentModelIndex(ch.modelIndex()), { GraphModel::HandleRole, getRole(qMetaTypeId<Collection>(), CollectionHandle::id_className) });
        model()->notify(QPersistentModelIndex(ch.modelIndex()), { GraphModel::HandleRole, getRole(qMetaTypeId<Collection>(), CollectionHandle::id_roleModel) });
    }else{
        qDebug() << "type is unregistered thus we done";
        return;
    }

    if( !wasStored ) {
        qDebug() << "collection was restored thus initing model and restoreing";
        c->m_cmodel = QSharedPointer<CollectionModel>( new CollectionModel( ch.modelIndex(), c->m_metaobject ), &CollectionModel::deleteLater );
        ch.restore();
    }
}

void CollectionFactory::removeTypeConnection(CollectionHandle ch)
{
    if( !ch.isStored() ) {
        qDebug() << "collection is restored we have to store it before removing type edge";
        ch.store();
    }
    for( const QVariant& e: ch.outEdges() ) {
        EdgeHandle eh = e.value<EdgeHandle>();
        if( eh.hasTag( "type" ) && eh.hasTag( "collection" ) ) {
            qDebug() << "found edge removing";
            eh.destroy();
            break;
        }
    }
    ch.node()->m_connectedType = nullptr;
    ch.node()->m_cmodel.clear();
    ch.node()->m_metaobject = nullptr;
    model()->notify( ch.modelIndex(), { GraphModel::NodeRoles::HandleRole } );
    model()->notify(QPersistentModelIndex(ch.modelIndex()), { GraphModel::HandleRole, getRole(qMetaTypeId<Collection>(), CollectionHandle::id_metaTypeId) });
    model()->notify(QPersistentModelIndex(ch.modelIndex()), { GraphModel::HandleRole, getRole(qMetaTypeId<Collection>(), CollectionHandle::id_className) });
    model()->notify(QPersistentModelIndex(ch.modelIndex()), { GraphModel::HandleRole, getRole(qMetaTypeId<Collection>(), CollectionHandle::id_roleModel) });
}

void CollectionFactory::setupNode(Node *)
{
    // TODO: do somewhat initialization of Collection node
    qDebug() << Q_FUNC_INFO << "Кирюша, узлы инициализируем теперь в этом методе.";
}

CollectionModel::CollectionModel(const QModelIndex &rootIndex, const QMetaObject *type, QObject *parent) :
    NodeChildrenModel(parent),
    m_elementType(type)
{
    setRootIndex(rootIndex);

    // TODO: restore all children
    if (m_elementType)
        m_metaTypeId = QMetaType::type(QByteArray(m_elementType->className()).append("*"));
}

QHash<int, QByteArray> CollectionModel::roleNames() const
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

QVariant CollectionModel::data(const QModelIndex &index, int role) const
{
    int tid = role >> 16;
    if (!tid)
        return NodeChildrenModel::data(index, role);
    if (tid != m_metaTypeId) {
        qWarning() << "wrong role! Role's type is" << QMetaType::typeName(tid) << "current is" << m_elementType->className();
        return QVariant();
    }
    int pidx = role & 0xffff;
    Element *inst = static_cast<Element*>(sourceModel()->getNode(mapToSource(index)));
    if (!inst) {
        qWarning() << "Error! Null node at index" << index;
        return QVariant();
    }
    return inst->getProperty(pidx);
}

int CollectionModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return sourceModel()->rowCount(rootIndex());
}

QString CollectionModel::typeName() const
{
    if (!m_elementType)
        return QString();
    return m_elementType->className();
}

QVariantList CollectionModel::roleModel() const{ // TODO: generate model with levels of inheritance and so on
    QVariantList ret;
    if( this == nullptr ) return ret;
    if (m_elementType == nullptr) return ret;
    for( int i = 0; i < m_elementType->propertyCount(); ++i)
        ret.append(m_elementType->property( i ).name());
    return ret;
}

QModelIndex CollectionModel::addElement(const QJSValue &init)
{
    //    qDebug() << "[ CM ] ==> creating new element" << sourceModel();
    Element* node = new Element;
    //    qDebug() << "[ CM ] ==> creating new element";
    //    Instance *node = new Instance;
    /**/
    if( m_elementType != nullptr ) {
        //        qDebug() << "creating new element itititi";
        node->resize(m_elementType->propertyCount());
//        qDebug() << "creating new element itititi";
        QJSValueIterator it(init);
        while (it.hasNext()) {
            it.next();
            // TODO: set data accordingly if collection is stored or not
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
    }/**/
//    qDebug() << "[ CM ] ==> adding new element" << sourceModel();

    return sourceModel()->appendNode(node, rootIndex());
}

QObject *CollectionModel::objectAt(int row)
{
    return nullptr;
//        Instance *inst = static_cast<Instance*>(sourceModel()->getNode(sourceModel()->index(row, 0, rootIndex())));
    //    return inst->wrapper();
}

void CollectionModel::printRoles() const
{
    qDebug() << "Model" << typeName() << "roles:" << roleNames().values();
}

void Collection::typeRegistered(bool s)
{
    qDebug() << "Collection node informed of type assosiated with it was registered:" << true;
}

void Collection::store()
{
    if( m_stored ) {
        qDebug() << "CollectionHandle store()  already stored";
        return;
    }
    qDebug() << "CollectionHandle store()";
    for( auto c : childNodes() ) {
        auto element = dynamic_cast<Element*>(c);
        if( element != nullptr )
            element->store();
    }
    m_stored = true;
//    model()->notify(d, { GraphModel::HandleRole });
//    model()->notify(QPersistentModelIndex(modelIndex()), { GraphModel::HandleRole, getRole(qMetaTypeId<Collection>(), id_isStored) });
}

void Collection::restore()
{

}


bool CollectionHandle::isStored() const
{
//    qDebug() << "stored" << node()->m_stored;
    return node()->m_stored;
}

CollectionModel *CollectionHandle::elements() const
{
    if( node()->m_cmodel.isNull() ) return nullptr;
    return node()->m_cmodel.data();
}

ElementHandle CollectionHandle::addElement(QJSValue init)
{
    if( isStored() ) {
//        qDebug() << "[ CH ] ==> fu! you cannot add elements while collection is in stored mode";
        return ElementHandle();
    }
    auto m = node()->m_cmodel;
    QModelIndex idx = node()->m_cmodel->addElement(init);
    auto e = model()->getHandle(idx).value<ElementHandle>();
    e.restore();
    model()->notify(d, { GraphModel::HandleRole });
    model()->notify(QPersistentModelIndex(modelIndex()), { GraphModel::HandleRole, getRole(qMetaTypeId<Collection>(), id_elements) });
    return e;
}

void CollectionHandle::store()
{
    if( isStored() ) {
        qDebug() << "CollectionHandle store()  already stored";
        return;
    }
    qDebug() << "CollectionHandle store()";
    for( auto c : node()->childNodes() ) {
        auto element = dynamic_cast<Element*>(c);
        if( element != nullptr )
            element->store();
    }

    node()->m_stored = true;
    model()->notify(d, { GraphModel::HandleRole });
    model()->notify(QPersistentModelIndex(modelIndex()), { GraphModel::HandleRole, getRole(qMetaTypeId<Collection>(), id_isStored) });
//    model()->notify(QPersistentModelIndex(modelIndex()), { GraphModel::HandleRole, getRole(qMetaTypeId<Collection>(), CollectionHandle::id_className) });
//    model()->notify(QPersistentModelIndex(modelIndex()), { GraphModel::HandleRole, getRole(qMetaTypeId<Collection>(), CollectionHandle::id_metaTypeId) });
}

void CollectionHandle::restore()
{
    if( !isStored() ) {
        qDebug() << "CollectionHandle restore() already restored";
        return;
    }
    qDebug() << "CollectionHandle restore()";
    if( node()->m_metaobject == nullptr ) {
        qDebug() << "CollectionHandle restore() no registered type provided";
        return;
    }
    if( node()->m_cmodel.isNull() ){
        node()->m_cmodel = QSharedPointer<CollectionModel>( new CollectionModel( modelIndex(), node()->m_metaobject ), &CollectionModel::deleteLater );
    }
    // or if has no type
    // or if type isn't registered

    for( auto c : node()->childNodes() ) {
        auto element = dynamic_cast<Element*>(c);
        if( element != nullptr )
            element->restore();
    }
    node()->m_stored = false;
    model()->notify(d, { GraphModel::HandleRole });
    model()->notify(QPersistentModelIndex(modelIndex()), { GraphModel::HandleRole, getRole(qMetaTypeId<Collection>(), id_isStored) });
//    model()->notify(QPersistentModelIndex(modelIndex()), { GraphModel::HandleRole, getRole(qMetaTypeId<Collection>(), id_roleModel) });
//    model()->notify(QPersistentModelIndex(modelIndex()), { GraphModel::HandleRole, getRole(qMetaTypeId<Collection>(), CollectionHandle::id_className) });
//    model()->notify(QPersistentModelIndex(modelIndex()), { GraphModel::HandleRole, getRole(qMetaTypeId<Collection>(), CollectionHandle::id_metaTypeId) });
}

void CollectionHandle::typeRegistered(bool registered, TypeHandle th)
{
    if( registered ){
        node()->m_metaobject = const_cast<QMetaObject*>( QMetaType::metaObjectForType( th.metaTypeId() ) );
        node()->m_cmodel = QSharedPointer<CollectionModel>( new CollectionModel( modelIndex(), node()->m_metaobject ), &CollectionModel::deleteLater );
    //    c->m_stored = false;
        model()->notify( modelIndex(), { GraphModel::NodeRoles::HandleRole } );
        model()->notify(QPersistentModelIndex(modelIndex()), { GraphModel::HandleRole, getRole(qMetaTypeId<Collection>(), CollectionHandle::id_className) });
        model()->notify(QPersistentModelIndex(modelIndex()), { GraphModel::HandleRole, getRole(qMetaTypeId<Collection>(), CollectionHandle::id_metaTypeId) });
//        restore();
    } else {
        store();
        node()->m_metaobject = nullptr;
        node()->m_cmodel.clear();
    //    c->m_stored = false;
        model()->notify( modelIndex(), { GraphModel::NodeRoles::HandleRole } );
        model()->notify(QPersistentModelIndex(modelIndex()), { GraphModel::HandleRole, getRole(qMetaTypeId<Collection>(), CollectionHandle::id_className) });
        model()->notify(QPersistentModelIndex(modelIndex()), { GraphModel::HandleRole, getRole(qMetaTypeId<Collection>(), CollectionHandle::id_metaTypeId) });
    }
}
