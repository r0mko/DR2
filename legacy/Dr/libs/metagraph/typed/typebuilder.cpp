#include "enumbuilder.h"
#include "methodbuilder.h"
#include "propertybuilder.h"
#include "type.h"
#include "private/qmetaobjectbuilder_p.h"
#include <QDebug>
#include <qqmlengine.h>
#include "dynamicobjectextension.h"
#include "collectionobject.h"
#include "type.h"
#include "graphmodel.h"

QMap<QByteArray, const QMetaObject *> TypeFactory::references = QMap<QByteArray, const QMetaObject *>();
QHash<QString, const QMetaObject*> TypeFactory::dynamicTypes = QHash<QString, const QMetaObject*>();

void TypeFactory::initReferences()
{
    int i = QMetaType::FirstCoreType;
    while (i <= QMetaType::User) {
        int skip = 100;
        while (skip) {
            if (!QMetaType::isRegistered(i)) {
                --skip;
            } else {
                const QMetaObject *mo = QMetaType::metaObjectForType(i);
                if (mo) {
                    QByteArray ptrName = QByteArray(mo->className()).append("*");
                    int mt = QMetaType::type(ptrName.constData());
                    if (mt != QMetaType::UnknownType && (QMetaType::typeFlags(mt) & QMetaType::PointerToQObject)) {
//                        qDebug() << "import reference metaObject" << mo->className();
                        importMetaObject(mo->className(), mo);
                    }
                }
            }
            ++i;
        }
        if (i < QMetaType::User)
            i = QMetaType::User;
    }
    //    debugPrint << "Imported initial set of" << references.size() << "metaobjects";
}

TypeFactory::TypeFactory(QObject *parent) :
    BlobFactory(parent),
    d(new QMetaObjectBuilder)
{
    clear();
}

TypeFactory::~TypeFactory()
{
    for( auto key: dynamicTypes.keys() )
        free( const_cast<QMetaObject*>(dynamicTypes[key]) );
    dynamicTypes.clear();
}

int TypeFactory::handlerId() const
{
    return m_handlerId;
}

QStringList TypeFactory::savedTypeNames() const
{
    return dynamicTypes.keys();
}

QStringList TypeFactory::baseTypeNames() const
{
    static QStringList ret;
    static int lastRegistered;
    if (!ret.isEmpty() && !QMetaType::isRegistered(lastRegistered + 1))
        return ret;
    int skip = 100;
    int i = QMetaType::FirstCoreType;
    while (skip) {
        if (!QMetaType::isRegistered(i)) {
            --skip;
        } else {
            lastRegistered = i;
            ret.append(QMetaType::typeName(i));
        }
        ++i;
    }
    return ret;
}

QStringList TypeFactory::allTypeNames() const
{
    static QStringList ret = baseTypeNames();
    static int lastRegistered;
    if (!ret.isEmpty() && !QMetaType::isRegistered(lastRegistered + 1))
        return ret;
    int skip = 100;
    int i = QMetaType::User;
    while (skip) {
        if (!QMetaType::isRegistered(i)) {
            --skip;
        } else {
            lastRegistered = i;
            ret.append(QMetaType::typeName(i));
        }
        ++i;
    }
    return ret;
}

QVariantList TypeFactory::typesInfo(QMetaType::TypeFlags filter) const
{
    static QVariantList ret;
    static int lastRegistered;
    if (!ret.isEmpty() && !QMetaType::isRegistered(lastRegistered + 1))
        return ret;
    int i = lastRegistered;
    forever {
        int skip = 100;
        while (skip) {
            if (!QMetaType::isRegistered(i)) {
                --skip;
            } else {
                lastRegistered = i;
                QVariantMap ti;
                QMetaType::TypeFlags f = QMetaType::typeFlags(i);
                if (!filter || (f & filter)) {
                    ti["typeName"] = QString(QMetaType::typeName(i));
                    ti["object"] = bool(f & QMetaType::PointerToQObject);
                    ti["movable"] = bool(f & QMetaType::MovableType);
                    ti["enum"] = bool(f & QMetaType::IsEnumeration);
                    ti["hasCtor"] = bool(f & QMetaType::NeedsConstruction);
                    ti["hasDtor"] = bool(f & QMetaType::NeedsDestruction);
                    ti["typeId"] = i;
                    ret.append(ti);
                }
            }
            ++i;
        }
        if (i < QMetaType::User)
            i = QMetaType::User;
        else
            break;
    }
    debugPrint<< "last" << lastRegistered;
    return ret;
}

QStringList TypeFactory::handlers() const
{
    QStringList ret;
    for (const HandlerData &d : handlers_imp) {
        ret.append(d.staticMetaObject->className());
    }
    return ret;
}

void TypeFactory::setHandlerId(int arg)
{
    if (m_handlerId == arg)
        return;
    m_handlerId = arg;
    emit handlerIdChanged();
}

void TypeFactory::setClassName(QString arg)
{
    if ((m_className == arg) || dynamicTypes.contains(arg))
        return;
    m_className = arg;
    emit classNameChanged();
}

void TypeFactory::setClassInfo(QVariantMap arg)
{
    if (arg == m_classInfos)
        return;
    m_classInfos = arg;
    emit classInfoChanged();
}

void TypeFactory::setBaseClassName(QString arg)
{
    if (m_baseClassName == arg)
        return;
    QByteArray ptrName = QString("%1*").arg(arg).toLocal8Bit();
    int tid = QMetaType::type(ptrName.constData());
    if (tid != QMetaType::UnknownType && QMetaType::metaObjectForType(tid) != nullptr) {
        m_baseClassName = arg;
        emit baseClassNameChanged(arg);
        return;
    }
    debugPrint << ptrName << "is unknown type!";
}

MethodBuilder *TypeFactory::addSlot(QString signature)
{
    d_chk();
    auto ret = new MethodBuilder(d->addSlot(signature.toLocal8Bit()), this);
    m_methods.append(ret);
    emit methodsChanged();
    return ret;
}

MethodBuilder *TypeFactory::addSlot(QString name, QStringList argtypes)
{
    QString signature = QString("%1(%2)").arg(name).arg(argtypes.join(","));
    return addSlot(signature);
}

MethodBuilder *TypeFactory::addSignal(QString signature)
{
    d_chk();
    auto ret = new MethodBuilder(d->addSignal(signature.toLocal8Bit()), this);
    m_methods.append(ret);
    emit methodsChanged();
    return ret;
}

MethodBuilder *TypeFactory::addSignal(QString name, QStringList argtypes)
{
    QString signature = QString("%1(%2)").arg(name).arg(argtypes.join(","));
    return addSignal(signature);
}

MethodBuilder *TypeFactory::addConstructor(QString signature)
{
    d_chk();
    auto ret = new MethodBuilder(d->addConstructor(signature.toLocal8Bit()), this);
    m_constructors.append(ret);
    emit constructorsChanged();
    return ret;
}

MethodBuilder *TypeFactory::addConstructor(QStringList argtypes)
{
    QString signature = QString("%1(%2)").arg(className()).arg(argtypes.join(","));
    return addConstructor(signature);
}

MethodBuilder *TypeFactory::addMethod(QString returnType, QString signature)
{
    auto m = new MethodBuilder(d->addMethod(signature.toLocal8Bit(), returnType.toLocal8Bit()), this);
    m_methods.append(m);
    emit methodsChanged();
    return m;
}

MethodBuilder *TypeFactory::addMethod(QString returnType, QString name, QStringList argtypes)
{
    QString signature = QString("%1(%2)").arg(name).arg(argtypes.join(","));
    return addMethod(returnType, signature);
}

PropertyBuilder *TypeFactory::addProperty(QString name, QString type, bool notifyable)
{
    d_chk();
    PropertyBuilder *pb = new PropertyBuilder(d->addProperty(name.toLocal8Bit(), type.toLocal8Bit()), this);
    if (notifyable) {
        MethodBuilder *s = addSignal(pb->getNotifierSignature());
        pb->setNotifySignal(s);
    }
    m_properties.append(pb);
    emit propertiesChanged();
    return pb;
}

EnumBuilder *TypeFactory::addEnum(QString name)
{
    auto m = new EnumBuilder(d->addEnumerator(name.toLocal8Bit()), this);
    m_enums.append(m);
    emit enumsChanged();
    return m;
}

bool TypeFactory::isValidType(QString typeName) const
{
    return QMetaType::type(typeName.toLocal8Bit().constData()) != QMetaType::UnknownType;
}

void TypeFactory::compileType()
{
    d_chk();
    //    d->setStaticMetacallFunction(handlerFns[m_handlerId]);
    d->addClassInfo("MetaCallHandlerId", QString::number(m_handlerId).toLocal8Bit());
    d->setStaticMetacallFunction(handlers_imp.at(m_handlerId).metaCall);
    d->setClassName(m_className.toLocal8Bit());
    QList<Method*> mlist = m_methods;
    for (Property *p : m_properties) {
        qDebug() << "saving property" << p->name() << p->type();
        QMetaPropertyBuilder mp = d->addProperty(p->name().toLocal8Bit(), p->type().toLocal8Bit());
        mp.setReadable(p->isReadable());
        mp.setWritable(p->isWritable());
        mp.setResettable(p->isResettable());
        mp.setDesignable(p->isDesignable());
        mp.setScriptable(p->isScriptable());
        mp.setStored(p->isStored());
        mp.setEditable(p->isEditable());
        mp.setUser(p->isUser());
        mp.setStdCppSet(false);
        mp.setEnumOrFlag(p->isEnumOrFlag());
        mp.setConstant(p->isConstant());
        mp.setFinal(p->isFinal());
        mp.setRevision(p->revision());
        if (p->notifyable()) {
            Method *sig = p->notifySignal();
            mp.setNotifySignal(d->addSignal(sig->signature().toLocal8Bit()));
            mlist.removeAll(sig);
        }
    }

    for (Method *m : mlist) {
        QMetaMethodBuilder mb;
        switch (m->methodType()) {
        case MethodBuilder::Slot:
            mb = d->addSlot(m->signature().toLocal8Bit());
            break;

        case MethodBuilder::Signal:
            mb = d->addSignal(m->signature().toLocal8Bit());
            break;

        case MethodBuilder::InvokableMethod:
            mb = d->addMethod(m->signature().toLocal8Bit());
            break;

        default:
            break;
        }
        mb.setAccess((QMetaMethod::Access)(int)m->access());
        QList<QByteArray> pnames;
        for (QString str : m->parameterNames()) {
            pnames.append(str.toLocal8Bit());
        }
        mb.setParameterNames(pnames);
        mb.setReturnType(m->returnType().toLocal8Bit());
        mb.setTag(m->tag().toLocal8Bit());
        mb.setRevision(m->revision());
    }

    for (Enum * i : m_enums) {
        QMetaEnumBuilder p = d->addEnumerator(i->name().toLocal8Bit());
        for (int c = 0; c < i->keys().size(); ++c) {
            p.addKey(i->keys().at(c).toLocal8Bit(), i->values().at(c));
        }
    }

    auto it = m_classInfos.cbegin();
    while (it != m_classInfos.cend()) {
        d->addClassInfo(it.key().toLocal8Bit(), it.value().toString().toLocal8Bit());
        ++it;
    }

    if (m_baseClassName.isEmpty())
//        d->setSuperClass(&QObject::staticMetaObject);
        d->setSuperClass( &ElementAgentBase::staticMetaObject);
    else {
        QByteArray ptrName = QString("%1*").arg(m_baseClassName).toLocal8Bit();
        int tid = QMetaType::type(ptrName.constData());
        Q_ASSERT(tid != QMetaType::UnknownType);
        d->setSuperClass(QMetaType::metaObjectForType(tid));
    }
}

void TypeFactory::initModel()
{
    m_className = d->className();
    emit classNameChanged();

    if(d->superClass()) {
        m_baseClassName = d->superClass()->className();
    }
    for (int i = 0; i < d->propertyCount(); ++i) {
        qDebug() << "add property" << d->property(i).name();
        m_properties.append(new PropertyBuilder(d->property(i), this));
    }
    emit propertiesChanged();

    for (int i = 0; i < d->methodCount(); ++i) {
        m_methods.append(new MethodBuilder(d->method(i), this));
    }
    emit methodsChanged();

    for (int i = 0; i < d->constructorCount(); ++i) {
        m_constructors.append(new MethodBuilder(d->constructor(i), this));
    }
    emit constructorsChanged();

    for (int i = 0; i < d->enumeratorCount(); ++i) {
        m_enums.append(new Enum(d->enumerator(i), this));
    }

    int hid = d->indexOfClassInfo("MetaCallHandlerId");
    if (hid != -1) {
        m_handlerId = d->classInfoValue(hid).toInt();
        emit handlerIdChanged();
    }
    emit enumsChanged();
}

void TypeFactory::removeSlot(Method *m)
{
    if (m->methodType() != MethodBuilder::Slot)
        return;
    removeMethod(m);
}

void TypeFactory::removeSignal(Method *m)
{
    if (m->methodType() != MethodBuilder::Signal)
        return;
    removeMethod(m);
}

void TypeFactory::removeConstructor(Method *m)
{
    if (m->methodType() != MethodBuilder::Constructor)
        return;
    if (m_constructors.removeAll(m)) {
        m->deleteLater();
        emit constructorsChanged();
    }
}

void TypeFactory::removeMethod(Method *m)
{
    if (m_methods.removeAll(m)) {
        m->deleteLater();
        emit methodsChanged();
    }
}

void TypeFactory::removeProperty(Property *m)
{
    if (m->notifySignal()) {
        removeSignal(m->notifySignal());
    }
    if (m_properties.removeAll(m)) {
        m->deleteLater();
        emit propertiesChanged();
    }
}

void TypeFactory::addClassInfo(QString name, QString value)
{
    m_classInfos.insert(name, value);
    emit classInfoChanged();
}

void TypeFactory::unregisterType(TypeHandle handle)
{
    if( !handle.isRegistered() ) return;
    qDebug() << "we unregistering type";
    if( QMetaType::unregisterType(handle.metaTypeId()) ) {
        references.remove( handle.className().toLocal8Bit() );
        free( const_cast<QMetaObject*>(dynamicTypes.take( handle.className() ) ) );
        handle.node()->setMetaTypeId(0);
        emit handle.model()->dataChanged( handle.modelIndex(), handle.modelIndex(), { GraphModel::HandleRole } );
        model()->notify(QPersistentModelIndex( handle.modelIndex() ), { GraphModel::HandleRole, getRole(qMetaTypeId<Type>(), TypeHandle::id_metaTypeId) });
        model()->notify(QPersistentModelIndex( handle.modelIndex() ), { GraphModel::HandleRole, getRole(qMetaTypeId<Type>(), TypeHandle::id_isRegistered) });
        // TODO: handle.notifyConnectedCollections
        handle.notifyCollections(false);
    }
}

void TypeFactory::registerType(TypeHandle handle)
{
    qDebug() << "we registering type...";
    if( handle.isRegistered() ) return;
//    loadTypeDef(handle);
//    compileType();
    registerCurrentType();
    Type* handleNode{ handle.node() };
    handleNode->setMetaTypeId( m_currentTypeId );
    emit handle.model()->dataChanged( handle.modelIndex(), handle.modelIndex(), { GraphModel::HandleRole } );
    model()->notify(QPersistentModelIndex( handle.modelIndex() ), { GraphModel::HandleRole, getRole(qMetaTypeId<Type>(), TypeHandle::id_metaTypeId) });
    model()->notify(QPersistentModelIndex( handle.modelIndex() ), { GraphModel::HandleRole, getRole(qMetaTypeId<Type>(), TypeHandle::id_isRegistered) });
    // TODO: handle.notifyConnectedCollections
    handle.notifyCollections(true);
}

void TypeFactory::registerCurrentType()
{
    qDebug() << "we registering current type";
    // TODO: check if type is registered already
    QMetaObject *mo = d->toMetaObject();
//    importMetaObject(d->className(), mo);
    //    dynamicTypes.insert(d->className(), mo);
//    references.insert( m_className.toLocal8Bit(), dynamicTypes.take( m_className ) );
    references.insert( m_className.toLocal8Bit(), mo );
    dynamicTypes.insert( m_className, mo );

    const HandlerData &hdata = handlers_imp[m_handlerId];
    QMetaType::TypeFlags flags(QtPrivate::QMetaTypeTypeFlags<QObject>::Flags);
    QByteArray ptrName = QByteArray(mo->className()).append("*");

    m_currentTypeId = QMetaType::registerNormalizedType(ptrName, hdata.del, hdata.create, hdata.dtor, hdata.ctor, hdata.size, flags, mo);

    debugPrint << "class" << QString(mo->className()) << "registered, qMetaTypeId =" << m_currentTypeId;
    emit currentTypeIdChanged();
    emit savedTypesChanged();
    emit typesInfoChanged();


}


TypeHandle TypeFactory::create(const QString &nodeId)
{
    if (!model())
        return TypeHandle();
    auto tn = new Type;
    if (m_currentTypeId != QMetaType::UnknownType) {
        tn->setClassName(m_className);
        tn->setMetaTypeId(m_currentTypeId);
        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        d->serialize(out);
        tn->rawdata = ba;
    }
    auto idx = model()->appendNode(tn, rootIndex());
    TypeHandle n(idx);
    if (!nodeId.isEmpty())
        n.setNodeId(nodeId);
    else
        n.setNodeId(m_className);
    return n;

}

void TypeFactory::loadType(QString className)
{
    const QMetaObject* mo{ nullptr };
    mo = references.value( className.toLocal8Bit() );
    if( mo == nullptr ) mo = dynamicTypes.value( className );
    if( mo == nullptr ) return;
    loadMetaObject( mo );
}

void TypeFactory::clear()
{
    qDeleteAll(m_methods);
    qDeleteAll(m_properties);
    qDeleteAll(m_constructors);
    qDeleteAll(m_enums);
    m_methods.clear();
    m_constructors.clear();
    m_enums.clear();
    m_properties.clear();
    m_className = "MyClass";
    m_classInfos.clear();
    m_currentTypeId = QMetaType::UnknownType;
    emit propertiesChanged();
    emit constructorsChanged();
    emit methodsChanged();
    emit enumsChanged();
    emit classNameChanged();
    emit classInfoChanged();
    emit currentTypeIdChanged();
    d.reset();
}

void TypeFactory::saveToModel(const QModelIndex &parent)
{
    if (!model())
        return;
    Node *p = model()->getNode(parent);
    if (!p) {
        debugPrint << "wrong node";
        return;
    }
    if (m_currentTypeId == QMetaType::UnknownType) {
        debugPrint << "type is not compiled. Compile and register type prior to save.";
        return;
    }
    auto tn = new Type;
    tn->setClassName(m_className);
    tn->setMetaTypeId(m_currentTypeId);
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    d->serialize(out);
    tn->rawdata = ba;
    auto idx = model()->appendNode(tn, parent);
    model()->setNodeId(idx, m_className);
}

void TypeFactory::save(TypeHandle handle)
{
    compileType();

//    if ( dynamicTypes.contains( m_className ) )
//        free( const_cast<QMetaObject*>( dynamicTypes[ m_className ] ) );
//    dynamicTypes[ m_className ] = d->toMetaObject();

    Type* n = handle.node();
    if ( n == nullptr ) return;
    n->setClassName( m_className );
//    n->setMetaTypeId( m_currentTypeId );
    QDataStream out(&n->rawdata, QIODevice::WriteOnly);
    d->serialize(out);
    qDebug() << "saved" << d->className() << d->propertyCount() << n->rawdata.size();
    emit handle.model()->dataChanged( handle.modelIndex(), handle.modelIndex(), { GraphModel::HandleRole } );
}

void TypeFactory::loadTypesFromModel(const QModelIndex &parent)
{
    if (!model())
        return;
    //    debugPrint << "Registering types" << m_model->address(parent);
    Node *p = model()->getNode(parent);
    if (!p) {
        debugPrint << "wrong node";
        return;
    }
    for (Node *n : p->childNodes()) {
        Type *tn = dynamic_cast<Type*>(n);
        if (!tn) {
            debugPrint << QMetaType::typeName(n->typeId()) << "is not a type node";
            continue;
        }
        QDataStream in(tn->rawData());
        clear();
        d_chk();
        d->deserialize(in, references);
        registerCurrentType();
        tn->setMetaTypeId(m_currentTypeId);
    }
}


void TypeFactory::importMetaObject(const QByteArray className, const QMetaObject *metaObject)
{
    if (!references.contains(className))
        references.insert(className, metaObject);
}

QMetaObjectBuilder *TypeFactory::d_chk() const
{
    d.reset(new QMetaObjectBuilder);
    return d.data();
}

QString TypeFactory::className() const
{
    return m_className;
}

QVariantMap TypeFactory::classInfo() const
{
    return m_classInfos;
}

QString TypeFactory::baseClassName() const
{
    return m_baseClassName;
}

int TypeFactory::currentTypeId() const
{
    return m_currentTypeId;
}

void TypeFactory::setCurrentTypeId(int arg)
{
    if (m_currentTypeId == arg)
        return;
    if (QMetaType::isRegistered(arg)) {
        const QMetaObject *mo = QMetaType::metaObjectForType(arg);
        if (mo) {
            m_currentTypeId = arg;
            emit currentTypeIdChanged();
        }
    }
}

void TypeFactory::setPrototype(QObject *obj, TypeFactory::AddMembers addMembers)
{
    d.reset(new QMetaObjectBuilder(obj->metaObject(), (QMetaObjectBuilder::AddMembers)(int)addMembers));
    initModel();
    //    m_className = d->className();
    //    emit classNameChanged();

    //    for (int i = 0; i < d->propertyCount(); ++i) {
    //        m_properties.append(new PropertyBuilder(d->property(i), this));
    //    }
    //    emit propertiesChanged();

    //    for (int i = 0; i < d->methodCount(); ++i) {
    //        m_methods.append(new MethodBuilder(d->method(i), this));
    //    }
    //    emit methodsChanged();

    //    for (int i = 0; i < d->constructorCount(); ++i) {
    //        m_constructors.append(new MethodBuilder(d->constructor(i), this));
    //    }
    //    emit constructorsChanged();

    //    for (int i = 0; i < d->enumeratorCount(); ++i) {
    //        m_enums.append(new Enum(d->enumerator(i), this));
    //    }
    //    emit enumsChanged();
}

void TypeFactory::loadMetaObject(const QMetaObject *mo)
{
    d.reset(new QMetaObjectBuilder(mo));
    initModel();
}

QStringList TypeFactory::qObjectTypes() const
{
    QStringList ret;
    for (QByteArray ba : references.keys()) {
        ret.append(ba);
    }
    qDebug() << "references" << references.keys();
    ret.append(dynamicTypes.keys());
    return ret;
}

void TypeFactory::loadTypeDef(TypeHandle handle)
{
    Type *n = handle.node();
    if (!n)
        return;
    qDebug() << "load typedef" << handle.nodeId() << handle.className() << n->rawdata.size();
    if (!n)
        return;
    QDataStream in(n->rawdata);
    clear();
    d_chk();
    d->deserialize(in, references);
    qDebug() << in.status();
    initModel();
}


QQmlListProperty<Enum> TypeFactory::qmlEnums() {
    return QQmlListProperty<Enum>(this, 0,
                                  &TypeFactory::qmlEnumsCount,
                                  &TypeFactory::qmlEnumsAt);
}

QQmlListProperty<Method> TypeFactory::qmlMethods() {
    return QQmlListProperty<Method>(this, 0,
                                    &TypeFactory::qmlMethodsCount,
                                    &TypeFactory::qmlMethodsAt);
}

QQmlListProperty<Property> TypeFactory::qmlProperties() {
    return QQmlListProperty<Property>(this, 0,
                                      &TypeFactory::qmlPropertiesCount,
                                      &TypeFactory::qmlPropertiesAt);
}

QQmlListProperty<Method> TypeFactory::qmlConstructors() {
    return QQmlListProperty<Method>(this, 0,
                                    &TypeFactory::qmlConstructorsCount,
                                    &TypeFactory::qmlConstructorsAt);
}

Enum *TypeFactory::qmlEnumsAt(QQmlListProperty<Enum> *list, int index)
{
    TypeFactory *o = qobject_cast<TypeFactory*>(list->object);
    return o->m_enums.value(index);
}

Method *TypeFactory::qmlMethodsAt(QQmlListProperty<Method> *list, int index)
{
    TypeFactory *o = qobject_cast<TypeFactory*>(list->object);
    return o->m_methods.value(index);
}

Property *TypeFactory::qmlPropertiesAt(QQmlListProperty<Property> *list, int index)
{
    TypeFactory *o = qobject_cast<TypeFactory*>(list->object);
    return o->m_properties.value(index);
}

Method *TypeFactory::qmlConstructorsAt(QQmlListProperty<Method> *list, int index)
{
    TypeFactory *o = qobject_cast<TypeFactory*>(list->object);
    return o->m_constructors.value(index);
}

int TypeFactory::qmlEnumsCount(QQmlListProperty<Enum> *list)
{
    TypeFactory *o = qobject_cast<TypeFactory*>(list->object);
    return o->m_enums.size();
}

int TypeFactory::qmlMethodsCount(QQmlListProperty<Method> *list)
{
    TypeFactory *o = qobject_cast<TypeFactory*>(list->object);
    return o->m_methods.size();
}

int TypeFactory::qmlPropertiesCount(QQmlListProperty<Property> *list)
{
    TypeFactory *o = qobject_cast<TypeFactory*>(list->object);
    return o->m_properties.size();
}

int TypeFactory::qmlConstructorsCount(QQmlListProperty<Method> *list)
{
    TypeFactory *o = qobject_cast<TypeFactory*>(list->object);
    return o->m_constructors.size();
}


int TypeFactory::nodeMetatypeId() const
{
    return qMetaTypeId<Type>();
}

void TypeFactory::importNode(QDataStream &in, Node *node)
{
    Type *n = static_cast<Type*>(node);
//    in >> n->m_rawData >> n->m_metaTypeId;
    in >> n->rawdata >> n->m_metaTypeId;
    if (n->m_metaTypeId > 0) {
        // register type right now
//        QDataStream ds(n->m_rawData);
        QDataStream ds(n->rawdata);
        clear();
        d_chk();
        d->deserialize(ds, references);
        registerCurrentType();
        n->setMetaTypeId(m_currentTypeId);
    }
    NodeFactory::importNode(in, node);
}

void TypeFactory::exportNode(QDataStream &out, Node *node) const
{
    Type *n = static_cast<Type*>(node);
//    out << n->m_rawData << n->m_metaTypeId;
    out << n->rawdata << n->m_metaTypeId;
    NodeFactory::exportNode(out, node);
}

