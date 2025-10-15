#include "rpcpeer.h"
#include "puppet.h"
#include "engine.h"
#include <QMetaMethod>

RPCPeer::RPCPeer(QObject *parent) :
    QObject(parent),
    m_peer(nullptr),
    m_engine(nullptr)
{
}

RPCPeer::~RPCPeer()
{
    qDebug() << this << "deleted";
    //    for (QObject *p:m_puppets) {
    //        p->deleteLater();
    //    }
}

RPCPeer::RPCPeer(Peer *peer, QObject *parent) :
    QObject(parent),
    m_peer(peer)
{
    //    connect(m_peer, &Peer::deactivated, this, &RPCPeer::deleteLater);
    m_engine = qobject_cast<RPCEngine *>(parent);
    m_signalProxy = new SignalProxy(this);
    Q_ASSERT(m_engine != nullptr);
}

QString RPCPeer::nodeName() const
{
    return m_nodeName;
}

QString RPCPeer::appName() const
{
    return m_appName;
}

QString RPCPeer::appVersion() const
{
    return m_appVersion;
}

void RPCPeer::setNetPeer(Peer *peer)
{
    if (m_peer)
        m_peer->disconnect(this);
    m_peer = peer;
    connect(m_peer, &Peer::destroyed, this, &RPCPeer::deleteLater);
    emit netPeerChanged(peer);
}

void RPCPeer::setNodeName(QString arg)
{
    qDebug() << "Node name:" << arg;
    if (m_nodeName != arg) {
        m_nodeName = arg;
        emit nodeNameChanged(arg);
    }
}

void RPCPeer::setAppName(QString arg)
{
    if (m_appName != arg) {
        m_appName = arg;
        emit appNameChanged(arg);
    }
}

void RPCPeer::setAppVersion(QString arg)
{
    if (m_appVersion != arg) {
        m_appVersion = arg;
        emit appVersionChanged(arg);
    }
}

void RPCPeer::addTypeMapping(int local, int remote)
{
    m_localTypeMap.insert(local, remote);
    m_remoteTypeMap.insert(remote, local);
}


void RPCPeer::addMetaObject(const QByteArray &name, QMetaObject *mo)
{
    if (m_metaObjects.contains(name)) {
        //        qDebug() << "Metaobject for class " << name.constData() << "already exists";
        return;
    }
    m_metaObjects.insert(name, mo);
}

QObject* RPCPeer::addPuppet(const QString &name, QMetaObject *metaObject, quint64 remotePtr)
{
    if (m_puppets.contains(name)) {
        qDebug() << "Puppet" << name << "already exists";
        return m_puppets.value(name);
    }
    Puppet *p = new Puppet(metaObject, this, remotePtr);
    p->moveToThread(this->thread());
    //    qDebug() << "Created puppet" << p;
    //    const QMetaObject *mo = p->metaObject();
    //    for(int i = 0; i < mo->methodCount(); ++i) {
    //        QMetaMethod m = mo->method(i);
    //        if (m.methodType() == QMetaMethod::Signal)
    //            p->forwardSignal(i);
    //    }

    m_puppets.insert(name, p);

    emit puppetsChanged();
    emit puppetAdded(p);
    return p;
}

void RPCPeer::removePuppet(const QString &name)
{

    if (!m_puppets.contains(name))
        return;
    QObject *p = m_puppets.take(name);
    p->deleteLater();
}

Peer *RPCPeer::netPeer() const
{
    return m_peer;
}

QQmlListProperty<QObject> RPCPeer::qmlPuppets()
{
    return QQmlListProperty<QObject>(this, &m_puppets, &RPCPeer::qmlPuppetCount, &RPCPeer::qmlPuppetAt);
}


int RPCPeer::qmlPuppetCount(QQmlListProperty<QObject> *list)
{
    PuppetHash *p = reinterpret_cast<PuppetHash*>(list->data);
    return p->size();
}

QObject *RPCPeer::qmlPuppetAt(QQmlListProperty<QObject> *list, int index)
{
    PuppetHash *p = reinterpret_cast<PuppetHash*>(list->data);
    return p->values().at(index);
}
RPCEngine *RPCPeer::engine() const
{
    return m_engine;
}

void RPCPeer::setEngine(RPCEngine *engine)
{
    m_engine = engine;
}

void RPCPeer::forwardSignal(QObject *sender, int signalIdx, quint64 puppetPtr, int propertyIndex)
{
    SignalMapHandle h = qMakePair(sender, qMakePair(signalIdx, puppetPtr));
    if (m_forwardedSignals.contains(h)) {
        return;
    }
    m_forwardedSignals.insert(h, m_signalProxy->addMapping(sender, signalIdx, puppetPtr, propertyIndex));

}

void RPCPeer::removeSignalForwarding(QObject *sender, int signalIdx, quint64 puppetPtr)
{
    SignalMapHandle h = qMakePair(sender, qMakePair(signalIdx, puppetPtr));
    if (m_forwardedSignals.contains(h)) {
        int id = m_forwardedSignals.take(h);
        m_signalProxy->removeMapping(id);
    }
}

void RPCPeer::reject(QString reason, int code)
{
    MSG_BODY(PeerRejected) q;
    q.messageText = reason;
    q.reasonCode = code;
    m_peer->sendMessage(new PeerRejectedMessage(q));
}
