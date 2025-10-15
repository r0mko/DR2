#include "graphmodelhelper.h"
#include "graphmodel.h"
GraphModelHelper::GraphModelHelper(QObject *parent) : QObject(parent)
{

}

QStringList GraphModelHelper::graphModels() const
{
    return GraphModel::graphModels.keys();
}

GraphIndex GraphModelHelper::resolve(const QUrl &mrl) const
{
    if (mrl.scheme() != "node")
        return GraphIndex();
    GraphModel *model = GraphModel::graphModels.value(mrl.host());
    if (!model)
        return GraphIndex();
    return model->getNodeIndex(model->getNode(mrl.path()));
}

QVariantList GraphModelHelper::extractProperties(QVariant value, bool excludeSuper) const
{
    qDebug() << value;
    if( value.isNull() ) return QVariantList();
    if( !value.isValid() ) return QVariantList();
    QObject *obj = value.value<QObject*>();
    const QMetaObject *mo = nullptr;
    if (obj) {
        mo = obj->metaObject();
    } else {
        int tid = value.userType();
        if (tid == QMetaType::UnknownType) {
            qWarning() << "Invalid instance";
            return QVariantList();
        }
        mo = QMetaType::metaObjectForType(tid);
    }
    if (!mo) {
        qWarning() << "Passed value" << value << "is not Q_GADGET neither Q_OBJECT";
        return QVariantList();
    }
    return dumpProperties(mo, excludeSuper);

}

QVariantList GraphModelHelper::extractPropertiesFromClassName(QString className, bool excludeSuper) const
{
    int tid = QMetaType::type(className.toLocal8Bit());
    if (tid == QMetaType::UnknownType) {
        qWarning() << "Unknown type" << className;
        return QVariantList();
    }
    const QMetaObject *mo = QMetaType::metaObjectForType(tid);
    if (!mo) {
        qWarning() << "Passed type" << className << "is not Q_GADGET neither Q_OBJECT";
        return QVariantList();
    }
    return dumpProperties(mo, excludeSuper);

}

QVariantMap GraphModelHelper::extractMetaObject(QObject *obj) const
{
    const QMetaObject *mo = obj->metaObject();
    return buildTree(mo);
}

QVariantMap GraphModelHelper::extractMetaObjectFromClassName(QString className) const
{
    int tid = QMetaType::type(className.toLocal8Bit());
    if (tid == QMetaType::UnknownType) {
        qWarning() << "Unknown type" << className;
        return QVariantMap();
    }
    const QMetaObject *mo = QMetaType::metaObjectForType(tid);
    if (!mo) {
        qWarning() << "Passed type" << className << "is not Q_GADGET neither Q_OBJECT";
        return QVariantMap();
    }
    return buildTree(mo);
}

QVariantList GraphModelHelper::dumpProperties(const QMetaObject *mo, bool excludeSuper) const
{
    QVariantList ret;
    int st = excludeSuper ? mo->propertyOffset() : 0;
    qDebug() << "dump props" << mo->className() << mo->propertyCount() << "offset" << mo->propertyOffset();
    for (int i = st; i < mo->propertyCount(); ++i) {
        QMetaProperty mp = mo->property(i);
        QVariantMap prop = buildProperty(mp);
        if (!excludeSuper)
            prop["class"] = mp.enclosingMetaObject()->className();
        ret.append(prop);
    }
    return ret;
}





QVariantMap buildProperty(const QMetaProperty &prop)
{
    QVariantMap p;
    p["type"] = "Property";
    p["name"] = QString(prop.name());
    p["valueType"] = prop.userType();
    p["typeName"] = QString(prop.typeName());
    p["userType"] = prop.userType();
    p["propertyIndex"] = prop.propertyIndex();
    p["isReadable"] = prop.isReadable();
    p["isWritable"] = prop.isWritable();
    p["isResettable"] = prop.isResettable();
    p["isConstant"] = prop.isConstant();
    p["isFinal"] = prop.isFinal();
    p["isFlagType"] = prop.isFlagType();
    p["isEnumType"] = prop.isEnumType();
    p["enumerator"] = prop.enumerator().name();
    p["hasNotifySignal"] = prop.hasNotifySignal();
    p["notifySignalIndex"] = prop.notifySignalIndex();
    p["revision"] = prop.revision();
    p["hasStdCppSet"] = prop.hasStdCppSet();
    return p;
}


QVariantMap buildEnum(const QMetaEnum &e)
{
    QVariantMap p;
    p["name"] = QString(e.name());
    p["scope"] = e.scope();
    p["isFlag"] = e.isFlag();
    QVariantList l;
    for (int i = 0; i < e.keyCount(); ++i) {
        QVariantMap v;
        v["key"] = e.key(i);
        v["value"] = e.value(i);
        l.append(v);
    }
    p["map"] = l;
    p["type"] = "Enumerator";
    return p;
}


QVariantMap buildMethod(const QMetaMethod &mm)
{
    QVariantMap m;
    m["name"] = QString(mm.name());
    m["type"] = "Method";
    switch (mm.methodType()) {
    case QMetaMethod::Method:
        m["methodType"] = "Method";
        break;
    case QMetaMethod::Signal:
        m["methodType"] = "Signal";
        break;
    case QMetaMethod::Slot:
        m["methodType"] = "Slot";
        break;
    case QMetaMethod::Constructor:
        m["methodType"] = "Constructor";
        break;
    }
    QVariantList args;
    for (int i = 0; i < mm.parameterCount(); ++i) {
        QVariantMap a;
        a["type"] = "Argument";
        a["name"] = QString(mm.parameterNames().at(i));
        a["argType"] = mm.parameterType(i);
        a["typeName"] = QString(mm.parameterTypes().at(i));
        args.append(a);
    }
    m["arguments"] = args;
    m["returnType"] = mm.returnType();
    return m;

}


QVariantMap buildClassInfo(const QMetaClassInfo &ci)
{
    QVariantMap m;
    m["type"] = "ClassInfo";
    m["name"] = QString(ci.name());
    m["value"] = QString(ci.value());
    return m;
}


QVariantMap buildTree(const QMetaObject *metaObject)
{
    QVariantMap ret;
    QVariantList props;
    for (int i = metaObject->propertyOffset(); i < metaObject->propertyCount(); ++i) {
        props.append(buildProperty(metaObject->property(i)));
    }
    ret["properties"] = props;

    QVariantMap enums;
    for (int i = metaObject->enumeratorOffset(); i < metaObject->enumeratorCount(); ++i) {
        QVariantMap e = buildEnum(metaObject->enumerator(i));
        enums.insert(e["name"].toString(), e);
    }
    ret["enums"] = enums;

    QVariantList methods;
    for (int i = metaObject->methodOffset(); i < metaObject->methodCount(); ++i) {
        methods.append(buildMethod(metaObject->method(i)));
    }

    ret["methods"] = methods;

    QVariantList cinfo;
    for (int i = metaObject->classInfoOffset(); i < metaObject->classInfoCount(); ++i) {
        cinfo.append(buildClassInfo(metaObject->classInfo(i)));
    }
    ret["classInfo"] = cinfo;
    int sup = 0;
    if (metaObject->superClass()) {
        int sup = QMetaType::type(QByteArray(metaObject->superClass()->className()).append('*').constData());
        if (sup == QMetaType::UnknownType) {
            qDebug() << "Superclass" << metaObject->superClass() << "not found!";
        }
    }
    ret["superClass"] = sup;
    ret["className"] = QString(metaObject->className());
    ret["type"] = "ObjectDef";
    return ret;
}
