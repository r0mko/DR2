#include "simulant.h"
#include <QDebug>

Simulant::Simulant(QObject *parent) :
    QObject(parent)
{
    if (!parent)
        setMetaobject(&QObject::staticMetaObject);
    else
        setProto(parent);
}

void Simulant::_staticMetacall(QObject *object, QMetaObject::Call call, int id, void **arguments)
{
    qDebug() << "qt_staticMetacall" << (int)call << id << arguments;
}

int Simulant::qt_metacall(QMetaObject::Call call, int id, void **arguments)
{
    qDebug() << "qt_metacall" << (int)call << id << arguments;
}

const QMetaObject *Simulant::metaObject() const
{
    return &m_metaobject;
}

void Simulant::setMetaobject(const QMetaObject *mo)
{
    m_metaobject = *const_cast<QMetaObject*>(mo);
    m_metaobject.d.static_metacall = _staticMetacall;
}

void Simulant::setProto(QObject *object)
{
    setMetaobject(const_cast<QMetaObject*>(object->metaObject()));
}
