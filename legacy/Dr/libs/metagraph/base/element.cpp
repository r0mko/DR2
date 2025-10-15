#include "element.h"
#include "graphmodel.h"
#include "collection.h"
#include "qqml.h"
#include "qqmlengine.h"
//#include "instance.h"
Collection *Element::collection() const
{
    return dynamic_cast<Collection*>( parent() );
}

ElementAgent *Element::wrapper()
{
    if( m_isStored ) return nullptr;
    if (i.isNull()) {
        i = new ElementAgent(collection()->m_metaobject, this, collection()->m_cmodel.data());
    }
    qmlEngine(collection()->m_cmodel.data())->setObjectOwnership(i.data(), QQmlEngine::JavaScriptOwnership);
    return i;
}

int Element::typeId() const
{
    return SELF_TYPEID;
}

QVariant Element::getProperty(int index) const
{
    if( !m_isStored )
        return m_propertyValues.at(index);
    return QVariant::Invalid;
}

void Element::setProperty(int index, const QVariant &value)
{
    if( !m_isStored )
        m_propertyValues[index] = value;
}

void Element::store()
{
    if( m_isStored ) {
        qDebug() << "Element store()  stored already";
        return;
    }
//    qDebug() << "Element store()";
    auto mo = collection()->m_metaobject;
    if( mo == nullptr ) return;
    for( int i = 0; i < mo->propertyCount(); ++i ) {
        auto key = mo->property( i ).name();
        m_storedPropertyValues[ key ] = m_propertyValues.value( i );
    }

    m_propertyValues.clear();
    //move data to stored map
    m_isStored = true;
}

void Element::restore()
{
    if( !m_isStored ) {
        qDebug() << "Element store()  stored already";
        return;
    }
//    qDebug() << "Element restore()";
    auto mo = collection()->m_metaobject;
    if( mo == nullptr ) return;
    m_propertyValues.resize(mo->propertyCount());
    for( int i = 0; i < mo->propertyCount(); ++i ) {
        auto key = mo->property( i ).name();
        if( m_storedPropertyValues.contains( key ) ) {
            m_propertyValues[ i ] = m_storedPropertyValues.value( key );
        }
    }
    m_storedPropertyValues.clear();
    m_isStored = false;
}

void Element::resize(int count )
{
    m_propertyValues.resize(count);
}


int ElementFactory::nodeMetatypeId() const
{
    return qMetaTypeId<Element>();
}

void ElementFactory::importNode(QDataStream &in, Node *node)
{
    Element *n = static_cast<Element*>(node);
//    bool wasStored = n->isStored();
//    if( !wasStored ) n->store();
    in >> n->m_storedPropertyValues;
//    if( !wasStored ) n->restore();
    NodeFactory::importNode(in, node);
}

void ElementFactory::exportNode(QDataStream &out, Node *node) const
{
    Element *n = static_cast<Element*>(node);
    n->collection()->store();
    if( !n->m_isStored ) n->store();
    out << n->m_storedPropertyValues;
    NodeFactory::exportNode(out, node);
}


void ElementFactory::setupNode(Node *)
{
    // TODO: Initialize Elenent node here
}


void ElementHandle::store()
{
//    qDebug() << "ElementHandle store()";
    node()->store();
}

void ElementHandle::restore()
{
//    qDebug() << "ElementHandle restore()";
    node()->restore();
}

ElementAgent *ElementHandle::instance() const
{
    return node()->wrapper();
}

QVariantList ElementHandle::properties() const
{

    QVariantList ret;
    auto mo = node()->collection()->m_metaobject;
    if( mo == nullptr ) return ret;
    for( int i = 0; i < mo->propertyCount(); ++i )
        ret << mo->property( i ).name();
    return ret;
}




