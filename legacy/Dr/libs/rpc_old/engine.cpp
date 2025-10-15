#include "engine.h"
#include "netmessenger.h"
#include "private/qmetaobjectbuilder_p.h"
#include <QCoreApplication>
#include <QTimer>
#include "msgdefs.h"
#include "rpcpeer.h"
#include <QMetaObject>
#include "puppet.h"
#include "QMetaObject"
#include <QThread>
#include "rpcattached.h"
#include "qmlpeer.h"
#define E_PTR(Ptr) reinterpret_cast<RPCEngine *>(Ptr)


RPCEngine *RPCEngine::instance = nullptr;
RPCEngine::RPCEngine(QObject *parent) :
    QObject(parent),
    m_net(0)
{
    Q_ASSERT(instance == nullptr);
    instance = this;
    int i = QMetaType::FirstCoreType;
    while (i < QMetaType::HighestInternalId) {
        checkType(i++);
    }
    i = QMetaType::User;
    while (QMetaType::isRegistered(i)) {
        checkType(i++);
    }
    qDebug() << m_types;
    setNetworkMessenger(new NetMessenger(this, true));
}

RPCEngine *RPCEngine::engine()
{
    return instance;
}

QString RPCEngine::nodeName() const
{
    return m_nodeName;
}

NetMessenger *RPCEngine::networkMessenger() const
{
    return m_net;
}

QQmlListProperty<RPCPeer> RPCEngine::qmlRpcPeerList()
{
    return QQmlListProperty<RPCPeer>(this, 0, &RPCEngine::qmlRpcPeersCount, &RPCEngine::qmlRpcPeerAt);
}

QObject *RPCEngine::queryRemoteObject(RPCPeer *p, const QString &name)
{
    MSG_BODY(QueryObject) q;
    q.objectName = name;
    q.flags = 0;
    QScopedPointer<Transaction> trn(new Transaction);
    q.transactionId = (quint64)trn.data();
    qDebug() << "Query" << name << "from" << p->nodeName();
    if (p->m_puppets.contains(name))
        return p->m_puppets.value(name);
    p->netPeer()->sendMessage(new QueryObjectMessage(q));
    if(!trn->waitForDataReady()) {
        qDebug() << "Failed to query object" << name << "from" << p->nodeName();
        return 0;
    }
    quint64 objPtr = trn->data.toULongLong();
    if (objPtr == 0) {
        qDebug() << "no object" << q.objectName << "at" << p->nodeName();
        return 0;
    } else {
        QObject *puppet = reinterpret_cast<QObject*>(objPtr);
        return puppet;
    }
}

void RPCEngine::addConnection(QUrl url, bool active, QmlPeer *p)
{
    if (!url.isValid())
        return;
    auto it = m_peers.cbegin();
    while (it != m_peers.cend()) {
        if (url == it.key()->url()) {
            qDebug() << "Peer" << url.toString() << "already exists";
        }
        ++it;
    }
    if (active) {
        m_pendingPeers.insert(m_net->connectToHost(url), p);
        qDebug() << "Connecting to" << url.toString();
    }
    // TODO: whitelist
}


void RPCEngine::setNodeName(QString arg)
{
    if (m_nodeName != arg) {
        qDebug() << "Node name:" << arg;
        m_nodeName = arg;
        emit nodeNameChanged(arg);
    }
}

void RPCEngine::setNetworkMessenger(NetMessenger *arg)
{
    if (m_net != arg) {
        if (m_net) {
            m_net->disconnect();
            if (m_net->parent() == this) {
                m_net->deleteLater();
                qDebug() << "Replacing netmessenger from" << m_net << "to" << arg ;
            }
        }
        m_net = arg;
        emit networkMessengerChanged(arg);
        m_net->registerMessageHandler(RPCIntro, RPCEngine::h_RPCIntro);
        m_net->registerMessageHandler(MetaTypeList, RPCEngine::h_MetatypeList);
        m_net->registerMessageHandler(ObjectRegistered, RPCEngine::h_ObjectRegistered);
        m_net->registerMessageHandler(ReadProperty, RPCEngine::h_ReadProperty);
        m_net->registerMessageHandler(WriteProperty, RPCEngine::h_WriteProperty);
        m_net->registerMessageHandler(ReadPropertyReply, RPCEngine::h_ReadPropertyReply);
        m_net->registerMessageHandler(SignalConnected, RPCEngine::h_SignalConnected);
        m_net->registerMessageHandler(SignalDisconnected, RPCEngine::h_SignalDisconnected);
        m_net->registerMessageHandler(NotifierConnected, RPCEngine::h_NotifierConnected);
        m_net->registerMessageHandler(EmitSignal, RPCEngine::h_EmitSignal);
        m_net->registerMessageHandler(EmitNotifier, RPCEngine::h_EmitNotifier);
        m_net->registerMessageHandler(InvokeMetaMethod, RPCEngine::h_InvokeMetaMethod);
        m_net->registerMessageHandler(InvokeMetaMethodReply, RPCEngine::h_InvokeMetaMethodReply);
        m_net->registerMessageHandler(QueryObject, RPCEngine::h_QueryObject);
        m_net->registerMessageHandler(QueryObjectReply, RPCEngine::h_QueryObjectReply);
        //        connect(m_net, &NetMessenger::peerAdded, [this](Peer *peer) { peer->setPtr(this); } );
        connect(m_net, &NetMessenger::peerActivated, this, &RPCEngine::queryPeer);
    }
}

bool RPCEngine::registerObject(QObject *object, bool notify)
{
    QString id;
    if (!object->objectName().isEmpty()) {
        id = object->objectName();
    } else {
        id = QString("%1_%2(%3)").arg(object->metaObject()->className()).arg(m_nodeName).arg(QString::number((qintptr)object, 16));
        object->setObjectName(id);
    }
    if (m_registeredObjects.contains(id))
        return false;
    m_registeredObjects.insert(id, object);
    if (!notify)
        return true;
    MSG_BODY(ObjectRegistered) b;
    b.objectName = id;
    b.className = object->metaObject()->className();
    //    MSG_BODY(ObjectRegistered) b = prepareObjectMessage(object);
    auto it = m_peers.cbegin();
    while (it != m_peers.cend()) {
        Peer *netpeer = it.key();
        netpeer->sendMessage(new ObjectRegisteredMessage(b));
        ++it;
    }
    return true;
}

bool RPCEngine::unRegisterObject(QObject *object, bool notify)
{
    if (!m_registeredObjects.contains(object->objectName()))
        return false;
    else
        m_registeredObjects.remove(object->objectName());
    if (!notify)
        return true;

    //TODO: unregeister object notification
    MSG_BODY(ObjectUnregistered) b;
    b.objectName = object->objectName();
    auto it = m_peers.cbegin();
    while (it != m_peers.cend()) {
        Peer *netpeer = it.key();
        netpeer->sendMessage(new ObjectUnregisteredMessage(b));
        ++it;
    }
    return true;
}

QVariantMap RPCEngine::metadata(QObject *object)
{
    QVariantMap ret;
    if (!object)
        return ret;
    QMetaObjectBuilder mo(object->metaObject());
    QVariantList props;
    for (int i = 0; i < mo.propertyCount(); ++i) {
        QMetaPropertyBuilder pb = mo.property(i);
        QVariantMap prop;
        prop.insert("name", QString(pb.name()));
        prop.insert("type", QString(pb.type()));
        props.append(prop);
    }
    QVariantList methods;
    for (int i = 0; i < mo.methodCount(); ++i) {
        QMetaMethodBuilder mm = mo.method(i);
        QVariantMap meth;
        meth.insert("signature", QString(mm.signature()));
        meth.insert("returnType", QString(mm.returnType()));

        //        meth.insert("parameterNames", QVariantList() << mm.parameterNames().to);
        //        meth.insert("parameterTypes", mm.parameterTypes());/

        meth.insert("methodType", (int)mm.methodType());

        methods.append(meth);
    }
    ret.insert("className", QString(mo.className()));
    ret.insert("properties", props);
    ret.insert("methods", methods);
    return ret;
}




void RPCEngine::clearPeer()
{
    Peer *p = reinterpret_cast<Peer *>(sender());
    if (p) {
        RPCPeer *rp = m_peers.take(p);
        if (rp) {
            emit peersChanged();
            emit peerRemoved(rp);
        }
    }
}

void RPCEngine::waitForInitialize(Peer *peer, BlobMap metaObjects, TypeIdHash types)
{
    RPCEngine *e = RPCEngine::engine();
    RPCPeer *p = m_peers.value(peer, nullptr);
    if (p)
        QMetaObject::invokeMethod(e, "setupTypeSystem", Qt::QueuedConnection, Q_ARG(Peer*, peer), Q_ARG(BlobMap, metaObjects), Q_ARG(TypeIdHash, types));
    else {
        qDebug() << "RPCPeer is not ready yet. Waiting for 200 ms";
        QThread::msleep(200);
        QMetaObject::invokeMethod(e, "waitForInitialize", Qt::QueuedConnection, Q_ARG(Peer*, peer), Q_ARG(BlobMap, metaObjects), Q_ARG(TypeIdHash, types));
    }
}

void RPCEngine::setupTypeSystem(Peer *peer, BlobMap metaObjects, TypeIdHash types)
{
    RPCPeer *p = m_peers.value(peer, nullptr);
    // FIXME: crashes here sometimes
    Q_ASSERT(p != nullptr);
    auto it = types.cbegin();
    int countImp = 0;
    int countErr = 0;
    while (it != types.cend()) {
        int remoteId = it.key();
        QByteArray remoteName = it.value();
        if (importType(p, remoteId, remoteName))
            ++countImp;
        else ++countErr;
        ++it;
    }
    qDebug() << "Imported" << countImp << "types," << countErr << "types not found";
    //    auto i = metaObjects.cbegin();
    //    while (i != metaObjects.cend()) {
    //        importMetaObject(peer, i.key(), i.value());
    //        ++i;
    //    }
    for (QObject *o:m_registeredObjects) {
        MSG_BODY(ObjectRegistered) q;
        q.className = o->metaObject()->className();
        q.objectName = o->objectName();
        peer->sendMessage(new ObjectRegisteredMessage(q));
        //        qDebug() << "Sent registered object" << o;
    }
}

void RPCEngine::constructPeer(Peer *peer, RPCIntroMessage::Body *msg)
{
    QScopedPointer<RPCIntroMessage::Body> d(msg);
    if (m_peers.contains(peer)) {
        qDebug() << "Duplicate intro message from" << peer->host() << d->nodeName;
        return;
    }
    RPCPeer *p = new RPCPeer(peer, this);
    p->setAppName(d->appName);
    p->setAppVersion(d->appVersion);
    p->setNodeName(d->nodeName);
    m_peers.insert(peer, p);
    connect(peer, &Peer::destroyed, this, &RPCEngine::clearPeer);
    connect(peer, &Peer::disconnected, this, &RPCEngine::clearPeer);
    connect(p, &RPCPeer::puppetAdded, this, &RPCEngine::puppetAdded, Qt::QueuedConnection);
    emit peersChanged();
    emit peerAdded(p);
    qDebug() << "Got RPC peer" << peer->host() << "nodename" << d->nodeName;
    MSG_BODY(MetaTypeList) b;
    //    MetaTypeListMessage::Body b;
    b.metaObjects = metaObjectInfo();
    b.types = m_types;
    peer->sendMessage(new MetaTypeListMessage(b));
    if (m_pendingPeers.contains(peer)) {
        QmlPeer *qp = m_pendingPeers.take(peer);
        if (qp)
            qp->setPeer(p);
    }
}

void RPCEngine::queryPeer(Peer *peer)
{
    //    qDebug() << "Querying" << peer << "if it is RPC-compliant";
    RPCIntroMessage::Body b;
    b.appName = qApp->applicationName();
    b.appVersion = qApp->applicationVersion();
    b.nodeName = m_nodeName;
    peer->sendMessage(new RPCIntroMessage(b));
    //    m_peers.insert(peer);
    connect(peer, &Peer::negativeAckReceived, this, &RPCEngine::discardPeer);
}

void RPCEngine::discardPeer(int type, QDateTime time)
{
    Q_UNUSED(time);
    if (type == RPCIntro) {
        discardPeer_helper(sender());
    }
}

void RPCEngine::discardPeer_helper(QObject *peer)
{
    Peer *p = qobject_cast<Peer*>(peer);
    Q_ASSERT(p != 0);
    if (m_peers.contains(p)) {
        qDebug() << p << "is not RPC compliant.";
        m_net->removePeer(p);
    }
}

bool RPCEngine::importType(RPCPeer *peer, int id, const QByteArray &name)
{
    int localId = QMetaType::type(name.constData());
    if (localId != QMetaType::UnknownType) {
        peer->addTypeMapping(localId, id);
        //        qDebug() << "Type" << name << id << localId;
        return true;
    } else {
        //        qDebug() << "Unknown remote type" << name << "typeid" << id;
        return false;
    }
}

void RPCEngine::importMetaObject(const QByteArray className, const QMetaObject *metaObject)
{
    if (!m_metaobjects.contains(className)) {
        m_metaobjects.insert(className, metaObject);
        qDebug() << "importing metaobject for class" << className;
    }
    // TODO: fake qt_static_metacall to imported qmetaobjects. Store to RPCPeer
}



void RPCEngine::emitPuppetSignalHelper(quint64 puppetPtr, int signalIndex, QVariantList arguments)
{
    Puppet *p = reinterpret_cast<Puppet*>(puppetPtr);
    QVector<void*> args;
    args.append(0);
    for(int i = 0; i < arguments.size(); ++i) {
        args.append(arguments[i].data());
    }
    p->emitSignal(signalIndex, args.data());
}

void RPCEngine::emitPuppetNotifierHelper(quint64 puppetPtr, int signalIndex, int propertyIndex, QVariant value)
{
    Puppet *p = reinterpret_cast<Puppet*>(puppetPtr);
    //    qDebug() << p << "property" << propertyIndex << "changed" << value;
    void *args[11] = { value.data() };
    p->m_propertyCache[propertyIndex].lastQuery = QDateTime::currentMSecsSinceEpoch();
    p->m_propertyCache[propertyIndex].value = value;
    p->emitSignal(signalIndex, args);
}

int RPCEngine::qmlRpcPeersCount(QQmlListProperty<RPCPeer> *list)
{
    RPCEngine *e = qobject_cast<RPCEngine *>(list->object);
    return e->m_peers.size();
}

RPCPeer *RPCEngine::qmlRpcPeerAt(QQmlListProperty<RPCPeer> *list, int index)
{
    RPCEngine *e = qobject_cast<RPCEngine *>(list->object);
    return e->m_peers.values().at(index);
}


MSG_BODY(QueryObjectReply) RPCEngine::prepareObjectMessage(QObject *object)
{
    qDebug() << "packing" << object;
    MSG_BODY(QueryObjectReply) q;
    q.objPtr = (quint64)object;
    q.objectName = object->objectName();
    const QMetaObject *mo = object->metaObject();
    while (mo) {
        //        qDebug() << "packing metaobject for class" << mo->className();
        QMetaObjectBuilder b(mo);
        for (int i = 0; i < b.classInfoCount(); ++i) {
            if (b.classInfoName(i) == "qt_HasQmlAccessors") // Workaround for FastProperty-caused crash
                b.removeClassInfo(i);
            //            qDebug() << mo->className() << "has classinfo" << mo->classInfo(i).name() << "value" << mo->classInfo(i).value();
        }
        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);
        b.serialize(out);
        q.metaObjects.append(data);
        mo = mo->superClass();
    }
    return q;
}

QMap<QByteArray, QByteArray> RPCEngine::metaObjectInfo() const
{
    QMap<QByteArray, QByteArray> ret;
    QMap<QByteArray, const QMetaObject *>::const_iterator it = m_metaobjects.cbegin();
    while (it != m_metaobjects.cend()) {
        QMetaObjectBuilder bld(it.value());
        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);
        bld.serialize(out);
        ret.insert(it.key(), data);
        ++it;
    }
    return ret;
}


void RPCEngine::checkType(int typeId)
{
    const QMetaObject *mo = QMetaType::metaObjectForType(typeId);
    if (mo) {
        QByteArray tname(QMetaType::typeName(typeId));
        if (tname.endsWith('*'))
            tname.chop(1);
        //        m_metaobjects.insert(tname, mo);
    } else {
        if (QMetaType::isRegistered(typeId))
            m_types.insert(typeId, QByteArray(QMetaType::typeName(typeId)));
    }
}


QDebug operator<<(QDebug dbg, QMetaObject::Call call)
{

    switch (call) {
    case QMetaObject::InvokeMetaMethod:
        dbg << "InvokeMetaMethod";
        break;
    case QMetaObject::ReadProperty:
        dbg << "ReadProperty";
        break;
    case QMetaObject::WriteProperty:
        dbg << "WriteProperty";
        break;
    case QMetaObject::ResetProperty:
        dbg << "ResetProperty";
        break;
    case QMetaObject::QueryPropertyDesignable:
        dbg << "QueryPropertyDesignable";
        break;
    case QMetaObject::QueryPropertyScriptable:
        dbg << "QueryPropertyScriptable";
        break;
    case QMetaObject::QueryPropertyStored:
        dbg << "QueryPropertyStored";
        break;
    case QMetaObject::QueryPropertyEditable:
        dbg << "QueryPropertyEditable";
        break;
    case QMetaObject::QueryPropertyUser:
        dbg << "QueryPropertyUser";
        break;
    case QMetaObject::CreateInstance:
        dbg << "CreateInstance";
        break;
    case QMetaObject::IndexOfMethod:
        dbg << "IndexOfMethod";
        break;
    case QMetaObject::RegisterPropertyMetaType:
        dbg << "RegisterPropertyMetaType";
        break;
    case QMetaObject::RegisterMethodArgumentMetaType:
        dbg << "RegisterMethodArgumentMetaType";
        break;
    default:
        break;
    }
    return dbg;
}
