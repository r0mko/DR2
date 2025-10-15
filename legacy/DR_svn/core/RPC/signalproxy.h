#ifndef SIGNALPROXY_H
#define SIGNALPROXY_H

#include <QObject>
#include <QVector>

class RPCPeer;
class SignalProxy : public QObject
{
public:
    explicit SignalProxy(QObject *parent = 0);
    int qt_metacall(QMetaObject::Call call, int id, void **arguments);
    static void qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a);

    int addMapping(QObject *sender, int senderSigId, quint64 puppetPtr, int propertyIndex = -1);
    void removeMapping(int id);
    RPCPeer *rpcPeer() const;

private:
    struct VirtualSlot
    {
        bool isEmpty; // true after removeSlot()
        QObject *object;
        int signalIdx;
        QVarLengthArray<int, 10> parameterTypes;
        quint64 puppetPtr;
        int propertyIndex;
    };

    QVector<VirtualSlot> m_virtualSlots;
};

#endif // SIGNALPROXY_H
