#include "metaobject.h"
#include <QMetaObject>
#include <QDebug>
#include <QtQml>

MetaObject::MetaObject(QObject *parent) :
    QObject(parent),
    m_mobj(this->metaObject()),
    m_object(this),
    m_superClass(this)
{
}

MetaObject::MetaObject(const QMetaObject *metaObject, QObject *parent) :
    QObject(parent),
    m_mobj(metaObject),
    m_object(nullptr),
    m_superClass(nullptr)
{
    init(metaObject);
}

MetaObject *MetaObject::superClass()
{
    if (metaObject()->superClass() != nullptr && m_superClass == nullptr) {
        m_superClass = new MetaObject(m_mobj->superClass(), this);
    }
    return m_superClass;
}

QQmlListProperty<MetaProperty> MetaObject::qmlListProperties()
{
    return QQmlListProperty<MetaProperty>(this, 0, &MetaObject::qmlMetaPropertyCount, &MetaObject::qmlMetaPropertyAt);
}

QQmlListProperty<MetaEnum> MetaObject::qmlListEnums()
{
    return QQmlListProperty<MetaEnum>(this, 0, &MetaObject::qmlMetaEnumCount, &MetaObject::qmlMetaEnumAt);
}

void MetaObject::componentComplete()
{
    init(m_mobj);
}

QObject *MetaObject::object() const
{
    return m_object;
}

QQmlListProperty<MetaMethod> MetaObject::qmlListMethods()
{
    return QQmlListProperty<MetaMethod>(this, 0, &MetaObject::qmlMetaMethodCount, &MetaObject::qmlMetaMethodAt);
}


void MetaObject::setObject(QObject *arg)
{
    if (m_object == arg)
        return;
    m_mobj = arg->metaObject();
    m_object = arg;
    emit objectChanged();
    init(m_mobj);
}

void MetaObject::test()
{
    qDebug() << "Foo";
}

void MetaObject::init(const QMetaObject *mo)
{
    qDeleteAll(m_properties);
    m_properties.clear();

    qDeleteAll(m_methods);
    m_methods.clear();

    qDeleteAll(m_enums);
    m_enums.clear();

    QMap<QByteArray, MetaEnum*> enumsMap;
    for (int i = 0; i < mo->enumeratorCount(); ++i) {
        MetaEnum *e = new MetaEnum(mo->enumerator(i));
        enumsMap[mo->enumerator(i).name()] = e;
        m_enums.append(e);
    }
    for (int i = 0; i < mo->methodCount(); ++i) {
        m_methods.append(new MetaMethod(mo->method(i), this));
    }

    for (int i = 0; i < mo->propertyCount(); ++i) {
        QMetaProperty mp = mo->property(i);
        MetaEnum *menum = 0;
        if (mp.isEnumType()) {
            menum = enumsMap.value(mp.enumerator().name());
        }
        MetaMethod *notifier = 0;
        if (mp.hasNotifySignal()) {
            notifier = m_methods.at(mp.notifySignal().methodIndex());
        }
        MetaProperty *prop = new MetaProperty(mp, m_object, notifier, menum, this);
        m_properties.append(prop);
    }

    emit methodsChanged();
    emit classNameChanged();
    emit superClassChanged();
    emit methodOffsetChanged();
    emit enumeratorOffsetChanged();
    emit propertyOffsetChanged();
    emit propertiesChanged();
    emit enumsChanged();
}

MetaMethod *MetaObject::qmlMetaMethodAt(QQmlListProperty<MetaMethod> *list, int index)
{
    MetaObject *e = qobject_cast<MetaObject *>(list->object);
    return e->m_methods.at(index);
}

int MetaObject::qmlMetaMethodCount(QQmlListProperty<MetaMethod> *list)
{
    MetaObject *e = qobject_cast<MetaObject *>(list->object);
    return e->m_methods.size();
}

MetaProperty *MetaObject::qmlMetaPropertyAt(QQmlListProperty<MetaProperty> *list, int index)
{
    MetaObject *e = qobject_cast<MetaObject *>(list->object);
    return e->m_properties.at(index);
}

int MetaObject::qmlMetaPropertyCount(QQmlListProperty<MetaProperty> *list)
{
    MetaObject *e = qobject_cast<MetaObject *>(list->object);
    return e->m_properties.size();
}

MetaEnum *MetaObject::qmlMetaEnumAt(QQmlListProperty<MetaEnum> *list, int index)
{
    MetaObject *e = qobject_cast<MetaObject *>(list->object);
    return e->m_enums.at(index);
}

int MetaObject::qmlMetaEnumCount(QQmlListProperty<MetaEnum> *list)
{
    MetaObject *e = qobject_cast<MetaObject *>(list->object);
    return e->m_enums.size();
}
