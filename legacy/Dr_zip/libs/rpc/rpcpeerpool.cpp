#include "rpcpeerpool.h"

QQmlListProperty<RPCPeer> RpcPeerPool::qmlPeers() {
    return QQmlListProperty<RPCPeer>(this, 0,
                                     &RpcPeerPool::qmlPeersAppend,
                                     &RpcPeerPool::qmlPeersCount,
                                     &RpcPeerPool::qmlPeersAt,
                                     QQmlListProperty<RPCPeer>::ClearFunction());
}

void RpcPeerPool::qmlPeersAppend(QQmlListProperty<RPCPeer> *list, RPCPeer *object)
{
    RpcPeerPool *o = qobject_cast<RpcPeerPool*>(list->object);
    o->m_peers.append(object);
    emit o->peersChanged();
}

int RpcPeerPool::qmlPeersCount(QQmlListProperty<RPCPeer> *list)
{
    RpcPeerPool *o = qobject_cast<RpcPeerPool*>(list->object);
    return o->m_peers.size();
}

RPCPeer *RpcPeerPool::qmlPeersAt(QQmlListProperty<RPCPeer> *list, int index)
{
    RpcPeerPool *o = qobject_cast<RpcPeerPool*>(list->object);
    return o->m_peers.at(index);
}

RpcPeerPool::RpcPeerPool(QObject *parent) :
    QObject(parent)
{
}
