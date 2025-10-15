#ifndef MESSAGEWORKER_H
#define MESSAGEWORKER_H

#include <QObject>
#include <QMutex>
#include "message.h"
#include "peer.h"
class NetMessenger;
class MessageWorker : public QObject
{
    Q_OBJECT
public:
    explicit MessageWorker(QObject *parent = 0);
    void enqueueMessage(Message *msg, Peer *sender);
    NetMessenger *messenger() const;
    void setMessenger(NetMessenger *messenger);
    QList<QPair<Message*, Peer*>> m_msgqueue;
    QMutex m_msgMutex;
    QAtomicInt m_counter;
    NetMessenger *m_messenger;
public slots:
    void processMessage(Message *message, Peer *peer);

signals:
    void messageReceived();

public slots:
    bool dequeueMessage(Message *&msg, Peer *&peer);
    void processMessageQueue();

};

#endif // MESSAGEWORKER_H
