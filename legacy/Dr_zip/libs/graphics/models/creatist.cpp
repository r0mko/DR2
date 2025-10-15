#include "creatist.h"
#include <QMetaType>
#include <QDebug>
#include <QDataStream>


Creatist::Creatist(QObject *parent) :
    QObject(parent)
{

}

QVariant Creatist::create(const QString &type, const QVariantMap &attr)
{
    int tid = QMetaType::type(type.toLocal8Bit());
    if (tid == QMetaType::UnknownType) {
        qWarning() << "Unknown item type" << type;
        return QVariant::Invalid;
    }
    QVariant ret(tid, nullptr);
    void *ptr = QMetaType::construct(tid, const_cast<void*>(ret.constData()), nullptr);
    if (!ptr) {
        qWarning() << "Failed to create item" << tid;
        return QVariant::Invalid;
    }
    const QMetaObject *metaObject = QMetaType::metaObjectForType(tid);
    if (!metaObject)
        qWarning() << QMetaType::typeName(tid) << "has no attributes accessible in QML!";
    else
        update(ptr, metaObject, attr);
//    QMetaType::destroy(tid, ptr);
    return ret;
}

QVariant Creatist::handle(const QString &type)
{
    return create(type);
}


bool Creatist::update(void *ptr, const QMetaObject *metaObject,const QVariantMap &attr) const
{
    if (attr.isEmpty())
        return true;
    bool result = true;
    auto i_map = attr.cbegin();
    while (i_map != attr.cend()) {
        int i = metaObject->indexOfProperty(i_map.key().toLocal8Bit().constData());
        if (i < 0) {
            qDebug() << "No property" << i_map.key() << "in" << metaObject->className();
            continue;
        }
        QMetaProperty property = metaObject->property(i);
        QVariant v = i_map.value();
        result &= property.writeOnGadget(ptr, v);
        if (!result)
            qDebug() << "writing property" << property.typeName() << property.name() << "failed";
        ++i_map;
    }
    return result;
}

QByteArray Creatist::serialize(QVariant value) const
{
    QByteArray ba;
    QDataStream d(&ba, QIODevice::WriteOnly);
    if (!QMetaType::save(d, value.userType(), value.constData())) {
        qWarning() << "Unable to serialize type" << value.typeName() << "(" << value.userType() << ")";
    }
    return ba;
}

QVariant Creatist::deserialize(const QString &type, const QByteArray &blob) const
{
//    qDebug() << "deserialize type" << type << "from blob size" << blob.size();
    int tid = QMetaType::type(type.toLocal8Bit());
    if (tid == QMetaType::UnknownType) {
        qWarning() << "Unknown type" << type;
        return QVariant::Invalid;
    }
    QVariant ret(tid, nullptr);
    QDataStream in(blob);
    if (!QMetaType::load(in, tid, const_cast<void*>(ret.constData()))) {
        qWarning() << "Failed to deserialize object type" << type << "from blob size" << blob.size();
        return QVariant();
    }
    return ret;



}
