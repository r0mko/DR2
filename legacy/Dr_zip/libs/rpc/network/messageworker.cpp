#include "messageworker.h"
#include "netmessenger.h"
#include <QMutexLocker>

MessageWorker::MessageWorker(QObject *parent) :
    QObject(parent)
{
    connect(this, &MessageWorker::messageReceived, this, &MessageWorker::processMessageQueue, Qt::QueuedConnection);
}

void MessageWorker::enqueueMessage(Message *msg, Peer *sender)
{
    QMutexLocker locker(&m_msgMutex);
    m_msgqueue.append(qMakePair(msg, sender));
    emit messageReceived();
}


void MessageWorker::processMessageQueue()
{
    int size;
    Message *message;
    Peer *sender;
    while (dequeueMessage(message, sender)) {
        processMessage(message, sender);
    };
}

bool MessageWorker::dequeueMessage(Message *&msg, Peer *&peer)
{
    QMutexLocker locker(&m_msgMutex);
    if (m_msgqueue.isEmpty())
        return false;
    //    qDebug() << "Processing queue" << m_msgqueue.size();
    QPair<Message*, Peer*> ret = m_msgqueue.takeLast();
    msg = ret.first;
    peer = ret.second;
    return true;

}

void MessageWorker::processMessage(Message *message, Peer *peer)
{
    QScopedPointer<Message> msg(message);
    ++m_counter;
    HandlerFn fun = nullptr;
    if(m_messenger->m_messageHandlers.size() > msg->type())
        fun = m_messenger->m_messageHandlers.at(msg->type());
    if (fun)
        fun(peer, msg.data());
    else {
        emit peer->unknownMessageReceived(msg->type());
        qWarning() << this << "Unregistered message type" << msg->type();
    }
}

NetMessenger *MessageWorker::messenger() const
{
    return m_messenger;
}

void MessageWorker::setMessenger(NetMessenger *messenger)
{
    m_messenger = messenger;
}

