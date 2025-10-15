#include "blob.h"
#include "graphmodel.h"
#include "qqml.h"
#include <QQmlEngine>
QHash<uint, QString> BlobFactory::typeHash2String = QHash<uint, QString>();
int BlobHandle::id_metadataHandle = BlobHandle::staticMetaObject.indexOfProperty("metadataHandle") - BlobHandle::staticMetaObject.propertyOffset();

/*!
    \property BlobHandle::data

    \brief Двоичные данные узла
    \detail При наследовании от Blob эти данные можно использовать как постоянное хранилище, заменив ими реализацию методов сохранения и загрузки.

    \sa Blob::data
*/


QVariant Blob::data() const
{
    QVariantMap ret;
    ret["data"] = rawdata;
    ret["metadata"] = metadata;
    return ret;
}

void Blob::setData(const QVariant &value)
{
    QVariantMap map = value.toMap();
    if (map.contains("data"))
        rawdata = map["data"].toByteArray();
    if (map.contains("metadata"))
        metadata = QJsonObject::fromVariantMap(map["metadata"].toMap());
}

void BlobFactory::setData(QByteArray data)
{
    if (m_data == data)
        return;

    m_data = data;
    emit dataChanged();
}

void BlobFactory::setupNode(Node *node)
{
    Blob *b = static_cast<Blob*>(node);
    uint h = typeHash(m_type);
    b->type = h;
    if (!typeHash2String.contains(h))
        typeHash2String[h] = m_type;
    b->metadata = m_metadata;
    b->rawdata = m_data;
}

uint BlobFactory::typeHash(const QString &blobType)
{
    return qHash(blobType, MAGIC_V1);
}

void BlobFactory::setType(QString type)
{
    if (m_type == type)
        return;

    m_type = type;
    emit typeChanged();
}

void BlobFactory::setMetadata(QJsonObject metadata)
{
    if (m_metadata == metadata)
        return;
    m_metadata = metadata;
    emit metadataChanged();
}

int BlobFactory::nodeMetatypeId() const
{
    return qMetaTypeId<Blob>();
}

void BlobFactory::importNode(QDataStream &in, Node *node)
{
    Blob *n = static_cast<Blob*>(node);
    QString type;
    QByteArray meta;
    in >> type >> n->rawdata >> meta;
    n->metadata = QJsonDocument::fromBinaryData(meta).object();
    n->type = qHash(type, MAGIC_V1);
    typeHash2String.insert(n->type, type);
    NodeFactory::importNode(in, node);
}

void BlobFactory::exportNode(QDataStream &out, Node *node) const
{
    Blob *n = static_cast<Blob*>(node);
    out << typeHash2String.value(n->type) << n->rawdata << QJsonDocument(n->metadata).toBinaryData();
    NodeFactory::exportNode(out, node);
}

/**
 * \fn BlobHandle::data
 * @return
 */
QByteArray BlobHandle::data() const
{
    idxCheck(QByteArray());
    return node()->rawdata;
}

void BlobHandle::setData(QByteArray data)
{
    idxCheck();
    node()->rawdata     = data;
}

QJsonObject BlobHandle::metadata() const
{
    idxCheck(QJsonObject());
    return node()->metadata;
}

QString BlobHandle::type() const
{
    idxCheck(QString());
    return model()->factory<Blob>()->typeHash2String.value(node()->type);
}

void BlobHandle::setMetadata(QJsonObject metadata)
{
    idxCheck();
    node()->metadata = metadata;
    model()->notify(d, { GraphModel::HandleRole });
}

void BlobHandle::setType(QString type)
{
    qDebug() << "Setting type via handle" << type;
    idxCheck();
    uint hash = BlobFactory::typeHash(type);
    node()->type = hash;
    model()->factory<Blob>()->typeHash2String[hash] = type;
}

int BlobHandle::size() const
{
    idxCheck(0);
    return node()->rawdata.size();
}

QQmlPropertyMap *BlobHandle::metadataHandle() const
{
    idxCheck(nullptr);

    QJsonObject dmap = node()->metadata;

    QQmlPropertyMap* map = new QQmlPropertyMap();
    qmlEngine(model())->setObjectOwnership(map, QQmlEngine::JavaScriptOwnership);

    QJsonObject::const_iterator it = dmap.constBegin();
    while (it != dmap.constEnd()) {
        map->insert(it.key(), it.value().toVariant());
        it++;
    };

    QObject::connect(map, &QQmlPropertyMap::destroyed, [this]() { qDebug() << "map destroyed"; });
    QObject::connect(map, &QQmlPropertyMap::valueChanged, [this](const QString &key, const QVariant &value) {
        idxCheck();
//        qDebug() << "before" << QJsonDocument(node()->metadata).toJson();
        node()->metadata[key] = QJsonValue::fromVariant(value);
//        qDebug() << "after" << QJsonDocument(node()->metadata).toJson();
//        dmap[key] = value;`
        model()->notify(d, { GraphModel::HandleRole, getRole(qMetaTypeId<Blob>(), id_metadataHandle) });
    });
    return map;
}

BlobHandle::operator QByteArray() const
{
    idxCheck(QByteArray());
    return node()->rawdata;
}


