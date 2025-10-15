#ifndef RPCPEERPOOL_H
#define RPCPEERPOOL_H

#include <QObject>
#include "rpcpeer.h"

class RpcPeerPool : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<RPCPeer> peers READ qmlPeers NOTIFY peersChanged)

public:
    explicit RpcPeerPool(QObject *parent = 0);

signals:
    void peersChanged();

public slots:
private:
    QList<RPCPeer*> m_peers;
    QQmlListProperty<RPCPeer> qmlPeers();
    static void qmlPeersAppend(QQmlListProperty<RPCPeer> *list, RPCPeer *object);
    static int qmlPeersCount(QQmlListProperty<RPCPeer> *list);
    static RPCPeer *qmlPeersAt(QQmlListProperty<RPCPeer> *list, int index);
};

#endif // RPCPEERPOOL_H
