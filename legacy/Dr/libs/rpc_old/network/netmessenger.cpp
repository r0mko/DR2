#include "netmessenger.h"
#include "message.h"
#include "peer.h"
#include "defines.h"
#include <QThread>
#include <QTimerEvent>
#include "messageworker.h"
NetMessenger::NetMessenger(QObject *parent, bool cpp) :
    QObject(parent),
    m_serverPort(DEFAULT_PORT),
    m_listening(false),
    m_ready(cpp),
    m_rxSpeed(0)
{
    m_server = new NetServer(this);
    registerMessageHandler(Handshake, Peer::handleHandshakeMsg);
    registerMessageHandler(SetUserData, Peer::handleSetUserDataMessage);
    perfTimer = startTimer(33);
    QThread *workerThread = new QThread(this);
    m_msgWorker = new MessageWorker();
    m_msgWorker->moveToThread(workerThread);
    m_msgWorker->setMessenger(this);
    connect(workerThread, &QThread::finished, m_msgWorker, &MessageWorker::deleteLater);
    workerThread->start();
//
}

NetMessenger::~NetMessenger()
{
    m_msgWorker->thread()->quit();
    m_msgWorker->thread()->wait(READ_TIMEOUT);
    //    qDebug() << "Server is shutting down";
    if (m_server->isListening())
        setListening(false);
}

QQmlListProperty<Peer> NetMessenger::qmlPeerList()
{
    return QQmlListProperty<Peer>(this, 0, &NetMessenger::qmlAppendPeer,
                                  &NetMessenger::qmlPeersCount,
                                  &NetMessenger::qmlPeerAt,
                                  QQmlListProperty<Peer>::ClearFunction());
}

void NetMessenger::componentComplete()
{
    m_ready = true;
    startServer();

}

void NetMessenger::registerMessageHandler(MessageType type, HandlerFn func)
{
    registerMessageHandler((int)type, func);
}

void NetMessenger::registerMessageHandler(int msgType, HandlerFn func)
{
    if (msgType >= m_messageHandlers.size())
        m_messageHandlers.resize(msgType+1);
    if (m_messageHandlers[msgType] != nullptr) {
        qWarning() << "Handler for message type" << msgType << "already registered!";
    } else {
        qWarning() << "Registered handler for message type" << msgType;
        m_messageHandlers[msgType] = func;
    }
}

void NetMessenger::setServerPort(int arg)
{
    if (m_serverPort != arg) {
        m_serverPort = arg;
        qDebug() << this->metaObject()->className() << "port set to" << arg;
        emit serverPortChanged(arg);
    }
}

void NetMessenger::setListening(bool arg)
{
    qDebug() << "Listening" << arg << m_serverPort << m_server->isListening() << m_ready;
    if (m_ready) {
        if (m_server->isListening() != arg) {
            if (arg) {
                bool ok = m_server->listen(QHostAddress::Any, m_serverPort);
                if (!ok) {
//                    qDebug() << "Server error:" << m_server->errorString() << "port" << m_serverPort;
                    emit m_server->errorOccured(m_server->serverError(), m_server->errorString());
                }
                else {
//                    qDebug() << "Server listening at port" << m_serverPort;
                    emit listeningChanged(ok);
                }
            } else {
                m_server->close();
            }
        }
    } else {
        m_listening = arg;
        emit listeningChanged(arg);
    }
}

void NetMessenger::updatePeerState(Peer *peer)
{
    switch (peer->status()) {
    case Peer::Connected:
        if (!m_activePeers.contains(peer)) {
            m_activePeers.insert(peer);
            emit peerActivated(peer);
        }
        break;
    case Peer::Negotiation:
        sendHandshake(peer);
        peer->setStatus(Peer::WaitingForHello);
        break;
    case Peer::Closing:
        qDebug() << "Peer" << peer << "is closing down";
        if (m_activePeers.remove(peer))
            emit peerDeactivated(peer);
        break;
    case Peer::Disconnected:
        if (!peer->active()) {
            qDebug() << peer << "disconnected remotely.";
            removePeer(peer);
        } else {
            qDebug() << "Local" << peer << "disconnected.";
            if (peer->autoReconnect())
                peer->setStatus(Peer::Reconnecting);
            else
                removePeer(peer);
        }
        break;
    case Peer::ConnectFailed:
        if (peer->autoReconnect())
            peer->setStatus(Peer::Reconnecting);
        else
            removePeer(peer);
        break;
    default:
        break;
    }
}

void NetMessenger::sendHandshake(Peer *peer)
{
    HandshakeMessage::Body h;
    h.protocolVersion = PROTOCOL_VER;
    HandshakeMessage *msg = new HandshakeMessage(h);
    peer->sendMessage(msg);
}

Peer *NetMessenger::qmlPeerAt(QQmlListProperty<Peer> *list, int index)
{
    NetMessenger *f = qobject_cast<NetMessenger *>(list->object);
    return f->m_peers.at(index);
}

int NetMessenger::qmlPeersCount(QQmlListProperty<Peer> *list)
{
    NetMessenger *f = qobject_cast<NetMessenger *>(list->object);
    return f->m_peers.size();
}

void NetMessenger::qmlAppendPeer(QQmlListProperty<Peer> *list, Peer *peer)
{
    NetMessenger *f = qobject_cast<NetMessenger *>(list->object);
    f->addPeer(peer);
}

void NetMessenger::addPeer(Peer *peer)
{
    peer->setMessenger(this);
    peer->setParent(this);
    m_peers.append(peer);
    connect(peer, &Peer::statusChanged, this, &NetMessenger::updatePeerState);
    connect(peer, &Peer::destroyed, this, &NetMessenger::removePeerHelper);
    emit peerAdded(peer);
    emit qmlPeersChanged();
}

Peer *NetMessenger::connectToHost(QUrl &url)
{
    Peer *peer = new Peer(this);
    peer->setMessenger(this);
    m_peers.append(peer);
    connect(peer, &Peer::statusChanged, this, &NetMessenger::updatePeerState);
    connect(peer, &Peer::destroyed, this, &NetMessenger::removePeerHelper);

    peer->setUrl(url);
    peer->setActive(true);
    peer->m_componentComplete = true;
    emit qmlPeersChanged();
    emit peerAdded(peer);

    return peer;
}




void NetMessenger::removePeer(Peer *peer)
{
    if (m_peers.removeAll(peer)) {
        if (m_activePeers.remove(peer))
            emit peerDeactivated(peer);
        emit peerRemoved(peer);
        emit qmlPeersChanged();
        peer->deleteLater();
    }
}

int NetMessenger::messagesReceived() const
{
    return m_msgWorker->m_counter;
}

void NetMessenger::timerEvent(QTimerEvent *e)
{
    static int prev;
    if (e->timerId() == perfTimer) {
        emit messagesReceivedChanged(m_msgWorker->m_counter);
        m_rxSpeed = (m_msgWorker->m_counter - prev) / 0.033;
        prev = m_msgWorker->m_counter;
        emit rxSpeedChanged(m_rxSpeed);
    }
}

qreal NetMessenger::rxSpeed() const
{
    return m_rxSpeed;
}

void NetMessenger::enqueueMessage(Message *msg, Peer *sender)
{
    m_msgWorker->enqueueMessage(msg, sender);
}

void NetMessenger::startServer()
{
    if (m_serverPort != 0 && m_listening) {
        qDebug() << "Starting server at port" << m_serverPort;
        setListening(true);
    }
}

void NetMessenger::removePeerHelper(QObject *peer)
{
    Peer *p = reinterpret_cast<Peer*>(peer);
    if (p) {
        qDebug() << "Deleting" << p;
        removePeer(p);
    }
}


NetServer::NetServer(QObject *parent) :
    QTcpServer(parent)
{
    //    if (!qobject_cast<NetMessenger*>(parent))
    //        qFatal("RPC::NetServer can only be constructed as direct child of NetMessenger. The application will now quit.");
}

void NetServer::incomingConnection(qintptr handle)
{
    Peer *p = new Peer(net());
    net()->addPeer(p);
    p->initAsServer(handle);
}
