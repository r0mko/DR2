#include "instantiatorcollection.h"


#include <QQmlEngine>
#include <qqmlcontext.h>
#include <QDebug>

InstantiatorCollection::InstantiatorCollection(QObject *parent) :
    QObject(parent)
{}

int InstantiatorCollection::size() const
{
    if (!i)
        return 0;

    return readProperty(iMeta.p_count).toInt();
}

bool InstantiatorCollection::introspect(QObject *instantiator, const char *classname)
{
    qDebug() << "trying to bind instantiator" << instantiator << classname;

    iMeta.mo = instantiator->metaObject();
    iMeta.p_count = iMeta.mo->indexOfProperty("count");
    iMeta.p_active = iMeta.mo->indexOfProperty("active");
    iMeta.s_objectAt = iMeta.mo->indexOfMethod("objectAt(int)");
    if (!iMeta.isValid()) {
        qWarning() << "failed to introspect Instantiator" << instantiator << iMeta.p_count << iMeta.p_active << iMeta.s_objectAt;
        return false;
    }
    if (iMeta.mo->property(iMeta.p_count).read(instantiator).toInt() > 0) {
        QObject *obj = instantiator->property("object").value<QObject*>();
        qDebug() << "ok, instantiator is active, checking classname" << obj->metaObject()->className() << obj->inherits(classname);
        if(obj->inherits(classname))
            bind(instantiator);
        return true;
    }
    qDebug() << "instantiator is not active, trying to construct an object";
    QQmlComponent *comp = instantiator->property("delegate").value<QQmlComponent*>();
    if (!comp)
        return false;
    QScopedPointer<QObject> obj(comp->create());
    qDebug() << "got instantiator object type:" << obj->metaObject()->className();
    if (obj->inherits(classname)) {
        bind(instantiator);
        return true;
    }
    return false;
}

void InstantiatorCollection::handleObjectAdded(int index, QObject *object)
{
    Q_UNUSED(index);
    Q_UNUSED(object);
}

void InstantiatorCollection::handleObjectRemoved(int index, QObject *object)
{
    Q_UNUSED(index);
    Q_UNUSED(object);
}

QObject *InstantiatorCollection::objectAt(int index) const
{
    if (!i)
        return nullptr;
//    return readProperty()
    return callMethod(iMeta.s_objectAt, index).value<QObject*>();
//    return i->objectAt(index);
}

void InstantiatorCollection::onObjectAdded(int index, QObject *object)
{
    emit objectAdded(index, object);
    emit sizeChanged();
    handleObjectAdded(index, object);
}

void InstantiatorCollection::onObjectRemoved(int index, QObject *object)
{
    emit objectRemoved(index, object);
    emit sizeChanged();
    handleObjectRemoved(index, object);
}

void InstantiatorCollection::onActiveChanged()
{
    bool a = readProperty(iMeta.p_active).toBool();
    emit activeChanged(a);
    if (!a)
        emit sizeChanged();
    handleActiveChanged();
}

bool InstantiatorCollection::scan(QObject *object)
{
    if (!object)
        object = parent();
    qDebug() << "scan" << this << object;
    if (!object)
        return false;
    for (QObject *obj:object->children()) {

//        QQmlInstantiator *itr = qobject_cast<QQmlInstantiator*>(obj);
        if (obj->inherits("QQmlInstantiator")) {
            if (tryBind(obj))
                return true;
        }
    }
    return false;
}

void InstantiatorCollection::bind(QObject *itor)
{
    i = itor;
    connect(itor, SIGNAL(objectAdded(int,QObject*)), this, SLOT(onObjectAdded(int,QObject*)));
    connect(itor, SIGNAL(objectRemoved(int,QObject*)), this, SLOT(onObjectRemoved(int,QObject*)));
    connect(itor, SIGNAL(activeChanged()), this, SLOT(onActiveChanged()));
//    connect(itor, &QQmlInstantiator::objectAdded, this, &InstantiatorCollection::onObjectAdded);
//    connect(itor, &QQmlInstantiator::objectRemoved, this, &InstantiatorCollection::onObjectRemoved);
    //    connect(itor, &QQmlInstantiator::activeChanged, this, &InstantiatorCollection::onActiveChanged);
}

QVariant InstantiatorCollection::readProperty(int idx) const
{
    int t = iMeta.mo->property(idx).type();
    int status = -1;
    QVariant value;

    void *argv[] = { 0, &value, &status };
    if (t == QMetaType::QVariant) {
        argv[0] = &value;
    } else {
        value = QVariant(t, (void*)0);
        argv[0] = value.data();
    }
    QMetaObject::metacall(const_cast<QObject*>(i), QMetaObject::ReadProperty, idx, argv);
    if (status != -1)
        return value;
    if (t != QMetaType::QVariant && argv[0] != value.data())
        return QVariant((QVariant::Type)t, argv[0]);
    return value;
}
typedef void (*StaticMetaCallFunction)(QObject *, QMetaObject::Call, int, void **);
QVariant InstantiatorCollection::callMethod(int idx, QVariant val0) const
{
    int t = iMeta.mo->method(idx).returnType();
    QVariant ret(t, nullptr);
    void *param[] = {
        ret.data(),
        val0.data()
    };
    StaticMetaCallFunction callFunction = iMeta.mo->d.static_metacall;
    callFunction(i, QMetaObject::InvokeMetaMethod, idx, param);
    return ret;
}




