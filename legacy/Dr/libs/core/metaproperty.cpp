#include "metaproperty.h"
#include <QMetaProperty>
#include <QDebug>

MetaProperty::MetaProperty(QObject *parent) :
    QObject(parent)
{
}

MetaProperty::MetaProperty(QMetaProperty prop, QObject *object, MetaMethod *notifySignal, MetaEnum *metaEnum, QObject *parent) :
    QObject(parent),
    p(prop),
    m_enumerator(metaEnum),
    m_notifySignal(notifySignal),
    m_object(object)
{
    if (object && prop.hasNotifySignal()) {
        valuePropertyIdx = this->metaObject()->indexOfProperty("value");
        Q_ASSERT(valuePropertyIdx > 0);
        int notifierIdx = this->metaObject()->indexOfSlot("updateValue()");
//        qDebug() << "Connecting" << prop.name() << prop.notifySignalIndex()
//                 << object->metaObject()->method(prop.notifySignalIndex()).name()
//                 << "to" << this->metaObject()->method(notifierIdx).name()
//                 << object->metaObject()->methodOffset() << this->metaObject()->methodOffset();
        QMetaObject::connect(object, prop.notifySignal().methodIndex(), this, notifierIdx);
    }
    m_inheritLevel = 0;
    const QMetaObject *mo = m_object->metaObject();
    QByteArray cname = p.enclosingMetaObject()->className();
    while (cname != QByteArray(mo->className())) {
        mo = mo->superClass();
        Q_ASSERT(mo != nullptr);
        ++m_inheritLevel;
    }
    m_inheritClassName = mo->className();
}

QObject *MetaProperty::object() const
{
    return m_object;
}

QVariant MetaProperty::value() const
{
    if (!m_object)
        return QVariant();
    return p.read(m_object);
}

int MetaProperty::inheritLevel() const
{
    return m_inheritLevel;
}

QString MetaProperty::inheritClassName() const
{
    return p.enclosingMetaObject()->className();
}

bool MetaProperty::setValue(QVariant arg)
{
    if (!m_object)
        return false;
    return p.write(m_object, arg);
}

void MetaProperty::updateValue()
{
    emit valueChanged();
}
