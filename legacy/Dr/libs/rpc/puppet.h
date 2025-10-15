#ifndef PUPPET_H
#define PUPPET_H

#include <QObject>
#include <QtGlobal>
#include <QVector>
#include <QVariant>

class RPCPeer;
class Puppet : public QObject
{
    QMetaObject mo;

public:
    friend class RPCEngine;
    Puppet();
    Puppet(QMetaObject *metaObject, RPCPeer *rpcPeer, quint64 objPtr);

    static void puppetStaticMetacall(QObject *object, QMetaObject::Call call, int id, void **arguments);

    virtual int qt_metacall(QMetaObject::Call call, int id, void **arguments);

    const QMetaObject *metaObject() const;
    void cancelSignalForwarding(int signalIndex);
    void forwardSignal(int signalIndex);
    void forwardPropertyNotifier(int signalIndex, int propertyIndex);

    virtual ~Puppet();
    void emitSignal(int index, void **arguments);
    RPCPeer *rpcPeer() const;
    void setRpcPeer(RPCPeer *rpcPeer);

protected:
    void connectNotify(const QMetaMethod &signal);
    void disconnectNotify(const QMetaMethod &signal);

private:
    RPCPeer *m_rpcPeer;
    quint64 m_ptr;
    struct CachedProperty
    {
        int type;
        QVariant value;
        qint64 lastQuery;
    };

    QVector<CachedProperty> m_propertyCache;
};



#endif // PUPPET_H
