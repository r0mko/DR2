#include "libqrpc_plugin.h"
#include "netmessenger.h"
#include "engine.h"
#include "qqml.h"
#include "rpcpeer.h"
#include "rpcattached.h"
#include "simulant.h"
#include <QDebug>
#include <QtQml>
#include "qmlpeer.h"
#include "qdrobject.h"

QObject *rpc_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(scriptEngine)
    Q_UNUSED(engine)
    RPCEngine *e = new RPCEngine;
    //    engine->rootContext()->setContextProperty("RPCEngine`", e);
    return e;
}

QDataStream &operator<<(QDataStream &out, const QQmlListProperty<RPCPeer> &list)
{
    RPCEngine *e = qobject_cast<RPCEngine *>(list.object);
    const QHash<Peer *, RPCPeer *> peers = e->peers();
    QHash<Peer *, RPCPeer *>::const_iterator it = peers.cbegin();
    QList<QVariantMap> plist;
    while (it != peers.cend()) {
        QVariantMap peer;
        peer.insert("url", it.key()->url());
        peer.insert("active", it.key()->active());
        peer.insert("nodename", it.value()->nodeName());
        plist.append(peer);
        ++it;
    }
    out << plist;
    return out;
}

QDataStream &operator>>(QDataStream &in, QQmlListProperty<RPCPeer> &list)
{
    qDebug() << "list object" << list.object;
    QList<QVariantMap> plist;
    in >> plist;
    for (QVariantMap i:plist) {
        QUrl url = i.value("url").toUrl();
        qDebug() << "Adding stored peer" << url.toString();
        RPCEngine::engine()->addConnection(url, i.value("active").toBool());
    }
    return in;
}

void LibQRPCPlugin::registerTypes(const char *uri)
{
    // @uri DataReactor.RPC
    qRegisterMetaType<QQmlListProperty<RPCPeer>>();
    qRegisterMetaTypeStreamOperators<QQmlListProperty<RPCPeer>>();
    qRegisterMetaType<Peer::Status>();
    qRegisterMetaType<QAbstractSocket::SocketState>();
    qRegisterMetaType<QAbstractSocket::SocketError>();
    qRegisterMetaType<BlobMap>();
    qRegisterMetaType<TypeIdHash>();
    qRegisterMetaType<RPCIntroMessage::Body*>();

    qmlRegisterUncreatableType<RPCAttached>(uri, 1, 0, "RPC", tr("RPC is only available via attached properties"));
    qmlRegisterType<Peer>(uri, 1, 0, "Peer");
    qmlRegisterType<RPCPeer>(uri, 1, 0, "RPCPeer");
    qmlRegisterType<NetServer>(uri, 1, 0, "NetServer");
    qmlRegisterType<NetMessenger>(uri, 1, 0, "NetMessenger");
    qmlRegisterType<QmlPeer>(uri, 1, 0, "RemotePeer");
    qmlRegisterType<Simulant>(uri, 1, 0, "Simulant");
    qmlRegisterType<DRObject>(uri, 1, 0, "Object");
//    qmlRegisterSingletonType<RPCEngine>(uri, 1, 0, "RPC", rpc_singletontype_provider);
    qmlRegisterType<RPCEngine>(uri, 1, 0, "RPCEngine");
}


