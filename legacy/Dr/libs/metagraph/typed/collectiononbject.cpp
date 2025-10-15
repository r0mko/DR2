#include "collectionobject.h"
#include <QDebug>
#include "instance.h"
#include "graphmodel.h"
#include <QMetaProperty>


//int CollectionObject::qt_metacall(QMetaObject::Call c, int id, void **argv)
//{
//    id = QObject::qt_metacall(c, id, argv);
//    if (id < 0)
//        return id;
//    QDebug dbg = qDebug();
//    switch (c) {
//    case QMetaObject::InvokeMetaMethod:
//        if (id < m_metaObject->methodCount())
//            invokeMethod(id + m_metaObject->methodOffset(), argv);
//        id -= m_metaObject->methodCount();
//        break;
//    case QMetaObject::ReadProperty:
//        if (id < m_metaObject->propertyCount())
//            readProperty(id + m_metaObject->propertyOffset(), argv);
//        id -= m_metaObject->propertyCount();
//        break;
//    case QMetaObject::WriteProperty:
//        if (id < m_metaObject->propertyCount())
//            writeProperty(id + m_metaObject->propertyOffset(), argv);
//        id -= m_metaObject->propertyCount();
//        break;
//    case QMetaObject::ResetProperty:
//        dbg << "ResetProperty";
//    case QMetaObject::QueryPropertyDesignable:
//        dbg << "QueryPropertyDesignable";
//    case QMetaObject::QueryPropertyScriptable:
//        dbg << "QueryPropertyScriptable";
//    case QMetaObject::QueryPropertyEditable:
//        dbg << "QueryPropertyEditable";
//    case QMetaObject::QueryPropertyStored:
//        dbg << "QueryPropertyStored";
//    case QMetaObject::QueryPropertyUser:
//        dbg << "QueryPropertyUser";
//        id -= m_metaObject->propertyCount();
//        break;
//    case QMetaObject::CreateInstance:
//        dbg << "CreateInstance";
//        break;
//    case QMetaObject::IndexOfMethod:
//        dbg << "IndexOfMethod";
//        break;
//    case QMetaObject::RegisterPropertyMetaType:
//        dbg << "RegisterPropertyMetaType";
//        break;
//    case QMetaObject::RegisterMethodArgumentMetaType:
//        dbg << "RegisterMethodArgumentMetaType";
//        break;
//    }
//    return id;
//}

ElementAgent::ElementAgent(QObject *parent) :
    DynamicObjectExtension(parent)
{}

ElementAgent::ElementAgent(const QMetaObject *metaObject, Element *node, QObject *parent) :
    DynamicObjectExtension(parent),
    d(node)
{
    setMetaObject(metaObject);
}

void ElementAgent::setProperty(int index, QVariant value)
{
//    qDebug() << "trying to set property" << index << d->getProperty(index) << value;
    d->setProperty(index, value);


}

QVariant ElementAgent::getProperty(int index) const
{
//    qDebug() << "trying to read property" << index << d->getProperty(index);
//    for( int i = 0; i < metaObject()->propertyCount(); ++i )
//        qDebug() << metaObject()->property( i ).name();
    return d->getProperty(index);

}

QVariant ElementAgent::callMethod(int , QVariantList )
{
    qDebug() << "Calling methods not supported";
    return QVariant();
}


//void CollectionObject::readProperty(int id, void **args) const
//{
//    qDebug() << "ReadProperty" << id << m_metaObject->property(id).typeName() << m_metaObject->property(id).name();
//    QVariant pval = d->getProperty(id);
//    if( !pval.isValid() ) {
//        QMetaType::construct(m_metaObject->property(id).userType(), args[0], nullptr);
//        return;
//    }
//    if (pval.userType() == m_metaObject->property(id).userType()) {
//        QMetaType::construct(m_metaObject->property(id).userType(), args[0], pval.data());
//    } else {
//        qDebug() << "property returns different type" << pval.typeName() << "property type" << m_metaObject->property(id).typeName();
//        QMetaType::convert(pval.data(), pval.userType(), args[0], (m_metaObject->property(id).userType()));
//    }
//}

//void CollectionObject::invokeMethod(int, void **) const
//{
//    qDebug() << "invoking methods is not implemented";
//}

ElementAgent::~ElementAgent()
{}
