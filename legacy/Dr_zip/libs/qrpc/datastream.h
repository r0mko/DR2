#ifndef DATASTREAM_H
#define DATASTREAM_H
#include <QVariant>
#include <QDataStream>
#include <rpcpeer.h>

class QVariantStreamer
{
    int         m_type = 0;
    QByteArray  rawData;
    RPCPeer *   m_rpcPeer;

    friend QDataStream &operator <<(QDataStream &, const QVariantStreamer &);
    friend QDataStream &operator >>(QDataStream &, QVariantStreamer &);
public:
    QVariantStreamer(RPCPeer *peer = nullptr): m_rpcPeer(peer) {}
    QVariantStreamer(const QVariant &value, RPCPeer *peer = nullptr);
    QVariant reconstruct() const;
    RPCPeer *getRpcPeer() const { return m_rpcPeer; }
//    inline operator QVariant() const { return QVariant::fromValue(*this); }

    inline int type() const { return m_type; }

private:
    static QByteArray serialize(QJsonValue value);
    static QByteArray serialize(QJsonObject value);
    static QByteArray serialize(QJsonArray value);
    static QByteArray serialize(QJsonDocument value);
    static QByteArray serialize(QVariantList value, RPCPeer *peer = nullptr);

    template <class T>
    static QByteArray serialize(const T &t);
};

QDataStream &operator <<(QDataStream &out, const QVariantStreamer &v);
QDataStream &operator >>(QDataStream &in, QVariantStreamer &v);

template <class T>
QByteArray serialize(const T &t)
{
    int tid = qMetaTypeId<T>();
    QByteArray ba;
    QDataStream s(&ba, QIODevice::WriteOnly);
    QMetaType::save(s, tid, &t);
    return ba;
}
Q_DECLARE_METATYPE(QVariantStreamer)

#endif // DATASTREAM_H
