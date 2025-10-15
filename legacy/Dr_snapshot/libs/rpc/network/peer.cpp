#include "peer.h"
#include "defines.h"
#include <QHostAddress>
#include <QDebug>
#include "message.h"
#include <QCoreApplication>
#include <QThread>
#include "netmessenger.h"
#include "qwaitcondition.h"
#include "messageworker.h"

Peer::Peer(QObject *parent)
    : QObject(parent)
{
    setPort(DEFAULT_PORT);
    m_worker = new PeerWorker(this);
    QThread *workerThread = new QThread;
    m_worker->moveToThread(workerThread);
    connect(workerThread, &QThread::started, m_worker, &PeerWorker::init);
    connect(this, &Peer::sendMessage, m_worker, &PeerWorker::writeMessage, Qt::QueuedConnection);
    connect(m_worker, &PeerWorker::updateState, this, &Peer::setStatus);

    connect(this, &Peer::initAsClient, m_worker, &PeerWorker::initActive);
    connect(this, &Peer::initAsServer, m_worker, &PeerWorker::initPassive);
    connect(workerThread, &QThread::finished, workerThread, &QThread::deleteLater);
    workerThread->start();
}


Peer::~Peer()
{
    //    qDebug() << this << "shutting down...";
    m_worker->thread()->quit();
    if (!m_worker->thread()->wait(3000)) {
        qDebug() << "Worker thread not responding!";
        m_worker->thread()->terminate();
    }
    delete m_worker;
    qDebug() << this << "deleted.";
}


void Peer::componentComplete()
{
    m_componentComplete = true;
    setActive(m_active);
}

bool Peer::active() const { return m_active; }

void Peer::setUrl(QUrl arg)
{
    if (m_url != arg) {
        int oldPort = m_url.port();
        QString oldHost = m_url.host();
        m_url = arg;
        emit urlChanged(arg);
        if (m_url.port() != oldPort)
            emit portChanged(m_url.port());
        if (m_url.host() != oldHost)
            emit hostChanged(m_url.host());
    }
}

void Peer::setHost(QString arg)
{
    if (m_url.host() != arg) {
        m_url.setHost(arg);
        emit hostChanged(arg);
        emit urlChanged(m_url);
    }
}

void Peer::setPort(int arg)
{
    if (m_url.port() != arg) {
        m_url.setPort(arg);
        emit portChanged(arg);
        emit urlChanged(m_url);
    }
}

void Peer::setActive(bool arg)
{
    if (m_componentComplete) {
        //        qDebug() << "Activating peer" << this << arg << this->worker() << m_status;

        if (m_status != Initializing) {
            if (arg)
                initAsClient(m_url);
            else
                m_worker->socket()->close(); // FIXME: make thread-safe
        }
    } else {
        m_active = arg;
    }
}

void Peer::setRemoteUserData(QVariant data)
{
    SetUserDataMessage *msg = new SetUserDataMessage({data});
    sendMessage(msg);

}

void Peer::updateSocketState(QAbstractSocket::SocketState socketState)
{
    //    qDebug() << "Socket state changed to" << socketState << sender() << m_active;
//    qDebug() << "Socket state changed to" << socketState << "previous state was" << m_status;
    switch (socketState) {
    case QAbstractSocket::UnconnectedState:
        if (m_status == Peer::Connecting) {
            setStatus(ConnectFailed);
        } else {
            setStatus(Disconnected);
        }
        break;
    case QAbstractSocket::HostLookupState:
        setStatus(HostLookup);
        break;
    case QAbstractSocket::ConnectingState:
        setStatus(Connecting);
        break;
    case QAbstractSocket::ConnectedState:
        if (m_status == Peer::Connected) {
            return;
        }
        if (m_active)
            setStatus(Negotiation);
        else
            setStatus(WaitingForHello);
        break;
    case QAbstractSocket::ClosingState:
        setStatus(Closing);
        break;
    default:
        qDebug() << "Unknown socket status:" << socketState;
        break;
    }
}

void Peer::setStatus(Peer::Status s)
{
//    qDebug() << "Set status" << s << "for" << this << "by" << sender() << senderSignalIndex();
    if (m_status != s) {
        m_status = s;
        emit statusChanged(this);
        switch (s) {
        case Peer::SocketReady:
            if (m_active)
                initAsClient(m_url);
            break;
        case Peer::Reconnecting:
            m_reconnectTimer = startTimer(m_reconnectTimeout);
            break;
        case Peer::Closing:
            emit deactivated();
            break;
        case Peer::Negotiation:
        case Peer::WaitingForHello:
            emit connected();
            break;
        case Peer::Connected:
            emit activated();
        default:
            break;
        }
    }
}

void Peer::handleHandshakeMsg(Peer *peer, Message *message)
{
    //    qDebug() << "Running Handshake message handler for peer" << peer;
    GET_MESSAGE(Handshake, message, h);
    if (peer->status() == WaitingForHello) {
        peer->setStatus(Peer::Connected);
        if (!peer->active()) // in case of server send handshake back to sender
            peer->messenger()->sendHandshake(peer);
        //        qDebug() << "Accepted peer" << h->kernelName << "protocol version" << h->protocolVersion;
    }
}

void Peer::handleSetUserDataMessage(Peer *peer, Message *message)
{
    GET_MESSAGE(SetUserData, message, h);
    qDebug() << "Dummy message for" << peer << "content:" << h->messageData;
    peer->setLocalUserData(h->messageData);

}

void Peer::handleNegativeAckMessage(Peer *peer, Message *message)
{
    GET_MESSAGE(NegativeAck, message, h);
    qDebug() << peer << "doesn't support message type" << h->unrecognizedMessageType << "received at" << h->timestamp.toString("h:mm:ss.zzz");
    emit peer->negativeAckReceived(h->unrecognizedMessageType, h->timestamp);
}

void Peer::updateLastSeen()
{
    m_lastSeen = QDateTime::currentDateTime();
    emit lastSeenChanged(m_lastSeen);
}


NetMessenger *Peer::messenger() const
{
    return m_messenger;
}

QString Peer::peerAddress() const
{
    if (m_worker->socket()->isOpen())
        return m_worker->socket()->peerAddress().toString();
    else
        return m_url.host();
}

void Peer::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == m_reconnectTimer) {
        killTimer(m_reconnectTimer);
        m_reconnectTimer = 0;
        if (!m_autoReconnect)
            return;
        //        qDebug() << "reconnecting...";
        initAsClient(m_url);
    }
}

bool Peer::autoReconnect() const
{
    return m_autoReconnect;
}

QVariant Peer::userData() const
{
    return m_userData;
}

int Peer::reconnectTimeout() const
{
    return m_reconnectTimeout;
}

void Peer::setMessenger(NetMessenger *messenger)
{
    if (m_messenger != messenger) {
        if (m_messenger)
            m_messenger->disconnect(this);
        m_messenger = messenger;
        connect(m_worker, &PeerWorker::messageReceived, m_messenger->m_msgWorker, &MessageWorker::processMessage, Qt::QueuedConnection);
    }
}
PeerWorker *Peer::worker() const
{
    return m_worker;
}



QTcpSocket *PeerWorker::socket() const
{
    return m_socket;
}

void PeerWorker::waitForInitialized()
{
    QThread::msleep(100);
}

void PeerWorker::shutdown()
{
    if (m_socket->isOpen()) {
        m_socket->close();
        delete m_socket;
    }
}

void PeerWorker::initPassive(qintptr handle)
{
    if (!m_socket->setSocketDescriptor(handle)) {
        updateState(Peer::Error);
    } else {
        qDebug() << "Incoming connection from" << m_socket->peerAddress().toString().toLocal8Bit().data();
        m_peer->updateLastSeen();
        m_peer->setHost(m_socket->peerAddress().toString());
        m_peer->setPort(m_socket->peerPort());
        m_peer->setStatus(Peer::WaitingForHello);
    }
}

void PeerWorker::initActive(const QUrl &url)
{
    qDebug() << m_peer << "connecting to remote host" << url.toString().toLocal8Bit().data();
    m_socket->connectToHost(url.host(), url.port(DEFAULT_PORT));
}

void PeerWorker::writeMessage(Message *msg)
{
    //    qDebug() << "Sending message size" << msg->size() << "to host" << m_socket->peerAddress().toString();
    //    qDebug() << "send message type" << msg->type() << "to" << m_socket->peerAddress().toString();
    QDataStream out(m_socket);
    msg->save(out);
    qint64 bc = m_socket->bytesToWrite();
    if (bc)
        if (!m_socket->waitForBytesWritten(READ_TIMEOUT))
            qDebug() << "Write error" << bc << "bytes";
    delete msg; // FIXME: change to signal "messageSent(Message *msg)
}

void PeerWorker::init()
{
    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::stateChanged, m_peer, &Peer::updateSocketState, Qt::QueuedConnection);
    connect(m_socket, &QTcpSocket::readyRead, this, &PeerWorker::readMessage);
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), m_peer, SIGNAL(socketErrorChanged(QAbstractSocket::SocketError)));
    emit updateState(Peer::SocketReady);
}

void PeerWorker::readMessage()
{
    static bool ok;
    QDataStream in(m_socket);
    while(m_socket->bytesAvailable()) {
        QScopedPointer<Message> msg(Message::loadFromStream(in));
        if (in.status() != QDataStream::Ok) {
            qWarning() << "Broken stream!";
            updateState(Peer::Error);
            ok = false;
        } else {
            if (m_socket->bytesAvailable() > 16384)
                qDebug() << m_socket->bytesAvailable() << "bytes remaining in" << m_socket;
            emit messageReceived(msg.take(), m_peer);
//            m_peer->messenger()->enqueueMessage( m_peer);
            if (!ok) {
                ok = true;
                updateState(Peer::Connected);
            }
        }
    }
}


void Peer::setAutoReconnect(bool arg)
{
    if (m_autoReconnect != arg) {
        m_autoReconnect = arg;
        emit autoReconnectChanged(arg);
    }
}

void Peer::setLocalUserData(QVariant arg)
{
    if (m_userData != arg) {
        m_userData = arg;
        emit userDataChanged(arg);
    }
}

void Peer::setReconnectTimeout(int arg)
{
    if (m_reconnectTimeout != arg) {
        m_reconnectTimeout = arg;
        emit reconnectTimeoutChanged(arg);
    }
}

QDebug operator<<(QDebug dbg, Peer::Status s)
{
    switch (s) {
    case Peer::Initializing:
        dbg << "Peer::Initializing";
        break;
    case Peer::SocketReady:
        dbg << "Peer::SocketReady";
        break;
    case Peer::HostLookup:
        dbg << "Peer::HostLookup";
        break;
    case Peer::Connecting:
        dbg << "Peer::Connecting";
        break;
    case Peer::Negotiation:
        dbg << "Peer::Negotiation";
        break;
    case Peer::WaitingForHello:
        dbg << "Peer::WaitingForHello";
        break;
    case Peer::Connected:
        dbg << "Peer::Connected";
        break;
    case Peer::Closing:
        dbg << "Peer::Closing";
        break;
    case Peer::Disconnected:
        dbg << "Peer::Disconnected";
        break;
    case Peer::Destruction:
        dbg << "Peer::Destruction";
        break;
    case Peer::Reconnecting:
        dbg << "Peer::Reconnecting";
        break;
    case Peer::Error:
        dbg << "Peer::Error";
        break;
    case Peer::ConnectFailed:
        dbg << "Peer::ConnectFailed";
        break;
    default:
        break;
    }
    return dbg.space();
}
