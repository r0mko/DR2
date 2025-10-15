#include "type.h"
#include <QDebug>
#include <QMetaProperty>
#include <graphmodel.h>
#include "cxxabi.h"
#include "collection.h"
#include "collection.h"
int TypeHandle::id_metaTypeId = TypeHandle::staticMetaObject.indexOfProperty("metaTypeId") - TypeHandle::staticMetaObject.propertyOffset();
int TypeHandle::id_isRegistered = TypeHandle::staticMetaObject.indexOfProperty("isRegistered") - TypeHandle::staticMetaObject.propertyOffset();

void Type::setClassName(const QString &value)
{
    m_className = value;
}

//void Type::setRawData(const QByteArray &value)
//{
//    qDebug() << "set raw data"<< value.size();
//    m_rawData = value;
//    rawdata = value;
//}

void Type::setMetaTypeId(int value)
{
    m_metaTypeId = value;
}

QString Type::className() const
{
    return m_className;
}

QByteArray Type::rawData() const
{
//    return m_rawData;
    return rawdata;
}

int Type::metaTypeId() const
{
    return m_metaTypeId;
}

int Type::typeId() const { return SELF_TYPEID; }

QVariantList Type::getProperties() const
{
    QVariantList ret;
    QMetaObjectBuilder b;
//    QDataStream ds( m_rawData );
    QDataStream ds( rawdata );
    b.deserialize(ds, TypeFactory::references);

    for (int i = 0; i < b.propertyCount(); ++i) {
        QVariantMap prop;
        QMetaPropertyBuilder mp = b.property(i);
        prop["name"] = mp.name();
        prop["type"] = mp.type();//.typeName();
        prop["isWritable"] = mp.isWritable();
        prop["isReadable"] = mp.isReadable();
        prop["isConstant"] = mp.isConstant();
        prop["isResettable"] = mp.isResettable();
        prop["hasNotifySignal"] = mp.hasNotifySignal();
        ret.append(prop);
    }
//    qDebug() << " get properties" << ret;
    return ret;
}

bool TypeHandle::isRegistered() const
{
    idxCheck(false);
    return node()->metaTypeId() != QMetaType::UnknownType && QMetaType::isRegistered(node()->metaTypeId()) && QMetaType::metaObjectForType(node()->metaTypeId()) != nullptr;
}

void TypeHandle::notifyCollections(bool registered)
{
//    return;
    qDebug() << "node has inedges of" << node()->inEdges().count();
    for( auto e: node()->inEdges()){
        auto re = e.data();
        if( (re->hasTag("type") && re->hasTag("collection")) || re->hasTag("type|collection")) {
            auto c = dynamic_cast<Collection*>(re->startNode());
            if( c != nullptr ) {
                auto ch = model()->getHandle(c).value<CollectionHandle>();
                ch.typeRegistered( registered, *this );
            }
            model()->getHandle(re->startNode()).value<CollectionHandle>().typeRegistered( registered, *this );
        }
    }
}

int TypeHandle::rawDataSize() const
{
    idxCheck(0);
    return node()->rawdata.size();
}

