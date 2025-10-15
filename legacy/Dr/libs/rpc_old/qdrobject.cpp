#include "qdrobject.h"

DRObject::DRObject(QObject *parent) :
    QObject(parent)
{
}

QQmlListProperty<QObject> DRObject::qmlChildren()
{
    return QQmlListProperty<QObject>(this, 0, &DRObject::qmlAppendChild,
                                       &DRObject::qmlChildCount,
                                       &DRObject::qmlChildAt,
                                       QQmlListProperty<QObject>::ClearFunction());
}

void DRObject::appendChild(QObject *object)
{
    m_children.append(object);
    emit childrenChanged();
}

int DRObject::removeChild(QObject *object)
{
    int ret = m_children.removeAll(object);
    if (ret > 0)
        emit childrenChanged();
    return ret;
}

void DRObject::qmlAppendChild(QQmlListProperty<QObject> *list, QObject *child)
{
    DRObject *f = qobject_cast<DRObject *>(list->object);
    f->appendChild(child);
}

int DRObject::qmlChildCount(QQmlListProperty<QObject> *list)
{
    DRObject *f = qobject_cast<DRObject *>(list->object);
    return f->m_children.size();

}

QObject *DRObject::qmlChildAt(QQmlListProperty<QObject> *list, int index)
{
    DRObject *f = qobject_cast<DRObject *>(list->object);
    return f->m_children.at(index);
}
