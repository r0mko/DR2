#include "instantiatorcollection.h"
#include "smartobject.h"
#include <QDebug>
#include <QMetaProperty>
#include <QQmlProperty>
#include "private/qqmlinstantiator_p.h"

SmartObject::SmartObject(QObject *parent) : QObject(parent)
{
}

SmartObject::~SmartObject()
{
}

void SmartObject::classBegin()
{
//    qDebug() << "created" << metaObject()->className();
    scan(this);
}

void SmartObject::componentComplete()
{
//    qDebug() << "complete" << metaObject()->className();
    scan(this);
    setComplete(true);
}


QQmlListProperty<QObject> SmartObject::qmlData() {
    return QQmlListProperty<QObject>(this, 0,
                                     &SmartObject::qmlDataAppend,
                                     &SmartObject::qmlDataCount,
                                     &SmartObject::qmlDataAt,
                                     QQmlListProperty<QObject>::ClearFunction());
}

void SmartObject::qmlDataAppend(QQmlListProperty<QObject> *list, QObject *object)
{
    SmartObject *o = qobject_cast<SmartObject*>(list->object);
    o->appendChild(object);
}

int SmartObject::qmlDataCount(QQmlListProperty<QObject> *list)
{
    SmartObject *o = qobject_cast<SmartObject*>(list->object);
    return o->childCount();
}

QObject *SmartObject::qmlDataAt(QQmlListProperty<QObject> *list, int index)
{
    SmartObject *o = qobject_cast<SmartObject*>(list->object);
    return o->childAt(index);
}

AutoList::AutoList() :
    m_complete(false),
    m_defaultPropertyHandlerAccessible(false)
{}

AutoList::~AutoList()
{
    QHash<const QMetaObject *, QQmlListReference>::iterator it = m_lists.begin();
    while (it != m_lists.end()) {
        if (handled.contains(it.key())) {
            if (it.value().clear())
                qDebug() << "cleared list for type" << it.key()->className();
            else
                qDebug() << "failed to clear list for type" << it.key()->className();
        }
        ++it;
    }
}
bool AutoList::complete() const
{
    return m_complete;
}

void AutoList::setComplete(bool complete)
{
    m_complete = complete;
    if (!complete)
        return;
//    if (m_defaultPropertyHandlerAccessible) {
//        for (QObject *o : m_data) {
//            QQmlInstantiator *inst = qobject_cast<QQmlInstantiator*>(o);
//            if (inst) {
//                inst->setActive(true);
//                continue;
//            }
//            handleAdd(o);
//        }
//        return;
//    }

    for (QObject *o : instance->children()) {
        handleAdd(o);
    }
}

void AutoList::appendChild(QObject *child)
{
    m_data.append(child);
    //    emit dataChanged();
    //    emit dataChanged();
    handleAdd(child);
}

bool AutoList::removeChild(QObject *child)
{
    if (m_data.removeAll(child)) {
        //        emit dataChanged();
        handleRemove(child);
        return true;
    }
    return false;
}

void AutoList::scan(QObject *object, bool defaultPropertyHandlerAccessible)
{
    instance = object;
//    QByteArray defProp;
//    for (int i = 0; i < object->metaObject()->classInfoCount(); ++i) {
//        QMetaClassInfo ci = object->metaObject()->classInfo(i);
//        if (!qstrcmp(ci.name(), "DefaultProperty")) {
//            defProp = ci.value();
//            m_defaultList = QQmlListReference(object, defProp);
//            if (!defaultPropertyHandlerAccessible) {
//                QQmlProperty list(object, defProp);
//                qDebug() << "found default property" << defProp << "type" << (m_defaultList.isValid() ? m_defaultList.listElementType()->className() : "");
//                qDebug() << "canAt" << m_defaultList.canAt()
//                         << "canAppend" << m_defaultList.canAppend()
//                         << "canClear" << m_defaultList.canClear()
//                         << "canCount" << m_defaultList.canCount()
//                         << "notifyable" << list.hasNotifySignal();
//            }
//        }
//    }

//    for (int i = 0; i < object->metaObject()->propertyCount(); ++i) {
//        QMetaProperty prp = object->metaObject()->property(i);
//        if (QByteArray(prp.typeName()).startsWith("QQmlListProperty")) {
//            if (!qstrcmp(prp.name(), defProp))
//                continue;
//            QQmlListReference ref(object, prp.name());
//            if (ref.isValid()) {
////                qDebug() << "detected list property" << prp.name() << "list element" << ref.listElementType()->className();
//                m_lists.insert(ref.listElementType(), ref);
//            }
//        }
//    }
}

void AutoList::handleAdd(QObject *child)
{
//    QQmlInstantiator *instantiator = qobject_cast<QQmlInstantiator*>(child);
    if (child->inherits("QQmlInstantiator")) {
        qDebug() << "implement QQmlInstantiator binding" << child;
        m_instantiators.insert(child, qMakePair(QMetaObject::Connection(), QMetaObject::Connection()));
    }
//    if (instantiator) {
//        //        qDebug() << "added instantiator" << instantiator;
//        QMetaObject::Connection h_add = QObject::connect(instantiator, &QQmlInstantiator::objectAdded, [&](int i, QObject *o) { insertObject(i, o); } );
//        QMetaObject::Connection h_rem = QObject::connect(instantiator, &QQmlInstantiator::objectRemoved, [&](int i, QObject *o) { removeObject(i, o); });
//        if (!m_complete)
//            instantiator->setActive(false);
//        else {
//            for (QObject *o : instantiator->children()) {
//                handleAdd(o);
//            }
//        }
//        return;
//    }
    const QMetaObject* mo = child->metaObject();
    if (m_lists.contains(mo)) {
        qDebug() << "forwarding child" << child << "to typed list. Child's parent =" << child->parent();
        m_lists[mo].append(child);
        handled.insert(mo);
    }
}

void AutoList::handleRemove(QObject *child)
{
//    QQmlInstantiator *instantiator = qobject_cast<QQmlInstantiator*>(child);
//    if (instantiator) {
//        qDebug() << "removed instantiator";
//        m_instantiators.remove(instantiator);
//        return;
//    }
//    if (m_lists.contains(child->metaObject())) {
//        QQmlListReference ref = m_lists.value(child->metaObject());
//        qDebug() << "removing child" << child << "from typed list";
//        QList<QObject*> children;
//        for (int i = 0; i < ref.count(); ++i) {
//            if (ref.at(i) != child)
//                children.append(ref.at(i));
//        }
//        ref.clear();
//        for (QObject *object : children) {
//            ref.append(object);
//        }
//    }
}

void AutoList::insertObject(int index, QObject *object)
{
    Q_ASSERT(m_data.size() >= index);
    m_data.insert(index, object);
    handleAdd(object);
}

void AutoList::removeObject(int index, QObject *object)
{
    m_data.removeAt(index);
    //    emit dataChanged();
    handleRemove(object);
}

void AutoList::bind(QVector<InstantiatorCollection *> list)
{
    for (InstantiatorCollection *c : list) {
        if (c->scan())
            continue;

    }

}

