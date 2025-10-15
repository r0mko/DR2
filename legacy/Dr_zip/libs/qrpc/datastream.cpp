#include "datastream.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>



QVariantStreamer::QVariantStreamer(const QVariant &value, RPCPeer *peer)
{
    m_rpcPeer = peer;
    if (!value.isValid())
        return;
    m_type = value.userType();

//    qDebug() << "constructed QVariantStreamWrapper on" << value;
    switch(m_type) {
    case QMetaType::UnknownType:
    case QMetaType::Void:
    case QMetaType::VoidStar:
        m_type = QMetaType::UnknownType;
        break;
    case QMetaType::QObjectStar:
        // FIXME: implement puppent automatic gen
        m_type = QMetaType::QObjectStar;
        break;
//        return QVariantStreamWrapper(QMetaType::UnknownType);
    case QMetaType::QModelIndex:
    case QMetaType::QPersistentModelIndex:
        // FIXME: implement RemoteModelIndex
        m_type = QMetaType::UnknownType;
        break;
    case QMetaType::QJsonValue:
        rawData = serialize(value.toJsonValue());
        break;
    case QMetaType::QJsonObject:
        rawData = serialize(value.toJsonObject());
        break;
    case QMetaType::QJsonArray:
        rawData = serialize(value.toJsonArray());
        break;
    case QMetaType::QJsonDocument:
        rawData = serialize(value.toJsonDocument());
        break;
    case QMetaType::QVariantList:
        rawData = serialize(value.toList(), peer);
        break;
    default: {
        if (QMetaType::typeFlags(m_type) & QMetaType::IsGadget) {
            const QMetaObject *mo = QMetaType::metaObjectForType(m_type);
            qWarning() << "Unable to send object of type" << mo->className() << ": sending Q_GADGETS is not yet implemented. Wanna fix it? ;)";
            m_type = QMetaType::UnknownType;
            rawData.clear();
            return;
        }
        if (QMetaType::typeFlags(m_type) & QMetaType::PointerToQObject) {
            const QMetaObject *mo = QMetaType::metaObjectForType(m_type);
            qWarning() << "Unable to send object of type" << mo->className() << "sending QObject* is not yet implemented. Wanna fix it? ;)";
            m_type = QMetaType::UnknownType;
            rawData.clear();
            return;
        }
        QDataStream s(&rawData, QIODevice::WriteOnly);
        bool res = QMetaType::save(s, m_type, value.constData());
        if (!res) {
            qWarning() << Q_FUNC_INFO << "failed to save type" << value.typeName();
            m_type = QMetaType::UnknownType;
            rawData.clear();
        }
    }
    }
}

QByteArray QVariantStreamer::serialize(QJsonValue value)
{
    if (value.isArray())
        return serialize(value.toArray());
    if (value.isObject())
        return serialize(value.toObject());
    if (value.isObject())
        return serialize(value.toObject());
    QJsonDocument doc;
    QJsonObject o;
    o["__type"] = (int) value.type();
    o["__value"] = value;
    doc.setObject(o);
    return serialize(doc);
//    qWarning() << Q_FUNC_INFO << "saving of plain QJsonValues (bool, double, string, null, undefined) is not currently supported";

}

QByteArray QVariantStreamer::serialize(QJsonObject value)
{
    QJsonDocument doc;
    doc.setObject(value);
    return serialize(doc);
}

QByteArray QVariantStreamer::serialize(QJsonArray value)
{
    QJsonDocument doc;
    doc.setArray(value);
    return serialize(doc);
}

QByteArray QVariantStreamer::serialize(QJsonDocument value)
{
    return value.toBinaryData();
}

QByteArray QVariantStreamer::serialize(QVariantList value, RPCPeer *peer)
{
    QByteArray ba;
    QDataStream s(&ba, QIODevice::WriteOnly);
    s << value.size();
    for (QVariant v : value) {
        QVariantStreamer str(v, peer);
        s << str;
    }
    return ba;
}

QVariant QVariantStreamer::reconstruct() const
{
//    qDebug() << "reconstructing value type" << m_type << "size" << rawData.size();
    QDataStream in(rawData);
    switch(m_type) {
    case QMetaType::UnknownType:
        return QVariant();
//    case QMetaType::QObjectStar:
        // FIXME: implement puppent automatic gen
//    case QMetaType::QModelIndex:
//    case QMetaType::QPersistentModelIndex:
        // FIXME: implement RemoteModelIndex
//        return QVariantStreamWrapper(QMetaType::UnknownType);
    case QMetaType::QJsonValue: {
        QJsonDocument doc = QJsonDocument::fromBinaryData(rawData);
        if (doc.isArray())
            return doc.array();
        if (doc.isObject()) {
            if (doc.object().contains("__type")) {
//                int t = doc.object()["__type"].toInt();
                qDebug() << "Got single QJsonValue" << doc.object()[("__value")];
                return doc.object().value("__value");
            } else
                return QJsonValue(doc.object());
        }
        qWarning() << Q_FUNC_INFO << "unhandled JSON value" << doc.toJson();
        return QJsonValue(QJsonValue::Undefined);
    }
    case QMetaType::QJsonObject: {
        QJsonDocument doc = QJsonDocument::fromBinaryData(rawData);
        return doc.object();
    }
    case QMetaType::QJsonArray: {
        QJsonDocument doc = QJsonDocument::fromBinaryData(rawData);
        return doc.array();
    }
    case QMetaType::QJsonDocument:
        return QJsonDocument::fromBinaryData(rawData);
    case QMetaType::QVariantList: {
        QVariantList ret;
        int size;
        in >> size;
        while (size--) {
            QVariantStreamer vs(m_rpcPeer);
            in >> vs;
            ret.append(vs.reconstruct());
        }
        return ret;
    }
    default: {
        QVariant ret(m_type, nullptr);
        QMetaType::load(in, m_type, const_cast<void*>(ret.constData()));
        return ret;
    }
    }
}


QDataStream &operator <<(QDataStream &out, const QVariantStreamer &v)
{
    int type;

    (v.getRpcPeer()) ? type = v.getRpcPeer()->getRemoteType(v.m_type) : type = v.m_type;
//    if (v.getRpcPeer()) {
//        type = v.getRpcPeer()->getRemoteType(v.m_type);
//        qDebug() << "Remap local and remote type" << v.m_type << "->" << type;
//    } else {
//        type = v.m_type;
//        qDebug() << "Do not remap local and remote type" << type;
//    }
    out << type << v.rawData;
//    qDebug() << "saved value" << v.m_type << "size" << v.rawData.size();
    return out;
}


QDataStream &operator >>(QDataStream &in, QVariantStreamer &v)
{
    in >> v.m_type >> v.rawData;
//    qDebug() << "got value" << v.m_type << "size" << v.rawData.size();
    return in;
}
