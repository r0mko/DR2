#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>
#include <QByteArray>
#include <QHash>
#include <QMap>
#include <QDateTime>
#include <QQmlListProperty>
#include "msgdefs.h"
#include <QWaitCondition>
#include <QSet>
#include <QUrl>
#include "rpcattached.h"

//#include "netmessenger.h"
class Peer;
class RPCPeer;
class Message;
class NetMessenger;
class RPCAttached;
class QmlPeer;
class QJSEngine;

typedef QMap<QByteArray, QByteArray> BlobMap;
typedef QHash<int, QByteArray> TypeIdHash;

typedef void (*StaticMetacallFunction)(QObject *, QMetaObject::Call, int, void **);

struct Transaction
{
    QMutex mutex;
    quint64 timestamp;
    QWaitCondition dataReady;
    QVariant data;

    Transaction() : timestamp(QDateTime::currentMSecsSinceEpoch()) { mutex.lock(); }
    ~Transaction() { mutex.unlock(); }

    bool waitForDataReady(quint64 timeout = 10000) { return dataReady.wait(&mutex, timeout); }
};

class RPCEngine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString nodeName READ nodeName WRITE setNodeName NOTIFY nodeNameChanged)
    Q_PROPERTY(NetMessenger* networkMessenger READ networkMessenger WRITE setNetworkMessenger NOTIFY networkMessengerChanged)
    Q_PROPERTY(QQmlListProperty<RPCPeer> peers READ qmlRpcPeerList NOTIFY peersChanged)

public:
    RPCEngine(const RPCEngine &other);
    RPCEngine(QObject *parent = 0);
    static RPCEngine *engine();
    QString nodeName() const;
    NetMessenger *networkMessenger() const;
    QQmlListProperty<RPCPeer> qmlRpcPeerList();

    QHash<Peer *, RPCPeer *> peers() const { return m_peers; }
    Q_INVOKABLE QObject *queryRemoteObject(RPCPeer *p, const QString &name);
    static RPCAttached *qmlAttachedProperties(QObject *object)
    {
        qDebug() << "Constructed RPC attached for" << object;
        return new RPCAttached(object);
    }
signals:
    void nodeNameChanged(QString arg);
    void networkMessengerChanged(NetMessenger* arg);
    void puppetAdded(QObject *puppet);
    void peersChanged();
    void peerAdded(RPCPeer *peer);
    void peerRemoved(RPCPeer *peer);
    void objectRegistered(RPCPeer *peer, QString name, QString className);

public slots:
    void setNodeName(QString arg);
    void setNetworkMessenger(NetMessenger* arg);
    bool registerObject(QObject *object, bool notify = false);
    bool unRegisterObject(QObject *object, bool notify = false);
    void addConnection(QUrl url, bool active = true, QmlPeer *p = 0);
    QVariantMap metadata(QObject *object);

private slots:
    void queryPeer(Peer *peer);
    void discardPeer(int type, QDateTime time);
    void clearPeer();
    void waitForInitialize(Peer *peer, BlobMap metaObjects, TypeIdHash types);
    void setupTypeSystem(Peer *peer, BlobMap metaObjects, TypeIdHash types);
    void constructPeer(Peer *peer, RPCIntroMessage::Body *msg);
    void emitPuppetSignalHelper(quint64 puppetPtr, int signalIndex, QVariantList arguments);
    void emitPuppetNotifierHelper(quint64 puppetPtr, int signalIndex, int propertyIndex, QVariant value);

private:
    friend QObject *rpc_singletontype_provider(QQmlEngine *, QJSEngine *);


    static void h_MetatypeList(Peer *peer, Message *message);
    static void h_RPCIntro(Peer *peer, Message *message);
    static void h_PeerRejected(Peer *peer, Message *message);
    static void h_ObjectRegistered(Peer *peer, Message *message);
    static void h_ObjectUnregistered(Peer *peer, Message *message);
    static void h_ReadProperty(Peer *peer, Message *message);
    static void h_ReadPropertyReply(Peer *peer, Message *message);
    static void h_WriteProperty(Peer *peer, Message *message);
    static void h_SignalConnected(Peer *peer, Message *message);
    static void h_SignalDisconnected(Peer *peer, Message *message);
    static void h_NotifierConnected(Peer *peer, Message *message);
    static void h_EmitSignal(Peer *peer, Message *message);
    static void h_EmitNotifier(Peer *peer, Message *message);
    static void h_InvokeMetaMethod(Peer *peer, Message *message);
    static void h_InvokeMetaMethodReply(Peer *peer, Message *message);
    static void h_QueryObject(Peer *peer, Message *message);
    static void h_QueryObjectReply(Peer *peer, Message *message);

    static void f_staticMetacall(QObject *object, QMetaObject::Call call, int id, void **arguments);

    static int qmlRpcPeersCount(QQmlListProperty<RPCPeer> *list);
    static RPCPeer *qmlRpcPeerAt(QQmlListProperty<RPCPeer> *list, int index);

    static MSG_BODY(QueryObjectReply) prepareObjectMessage(QObject *object);

    void checkType(int typeId);
    void discardPeer_helper(QObject *peer);
    bool importType(RPCPeer *peer, int id, const QByteArray &name);
    void importMetaObject(const QByteArray className, const QMetaObject *metaObject);

    static RPCEngine *instance;
    QMap<QByteArray, QByteArray> metaObjectInfo() const;

    NetMessenger *m_net;
    QMap<QByteArray, const QMetaObject *> m_metaobjects;
    QHash<int, QByteArray> m_types;
    QHash<Peer *, RPCPeer *> m_peers;
    QString m_nodeName;
    QHash<Peer*, QmlPeer*> m_pendingPeers;
    NetMessenger* m_networkMessenger;
    QHash<QString, QObject *> m_registeredObjects;
    QHash<int, Transaction> m_transactions;
};

QDebug operator<<(QDebug dbg, QMetaObject::Call call);
//QDebug operator<<(QDebug dbg, QMetaObject::Call call);
QML_DECLARE_TYPE(RPCEngine)
QML_DECLARE_TYPEINFO(RPCEngine, QML_HAS_ATTACHED_PROPERTIES)
Q_DECLARE_METATYPE(BlobMap)
Q_DECLARE_METATYPE(TypeIdHash)
Q_DECLARE_METATYPE(RPCIntroMessage::Body*)
Q_DECLARE_METATYPE(QQmlListProperty<RPCPeer>)
#endif // RPCENGINE_H
