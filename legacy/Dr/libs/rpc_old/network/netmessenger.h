#ifndef NETMESSENGER_H
#define NETMESSENGER_H


#include <QObject>
#include <QTcpServer>
#include <QQmlListProperty>
#include <QQmlParserStatus>
#include <QList>
#include "peer.h"
#include "message.h"
#include <QAtomicInt>
#include <QMutex>

class NetServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit NetServer(QObject *parent = 0);

protected:
    void incomingConnection(qintptr handle);

signals:
    void errorOccured(QAbstractSocket::SocketError error, QString errorText);

private:
    inline NetMessenger *net() const { return reinterpret_cast<NetMessenger*>(parent()); }

};

class MessageWorker;

class NetMessenger : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(int serverPort READ serverPort WRITE setServerPort NOTIFY serverPortChanged)
    Q_PROPERTY(bool listening READ listening WRITE setListening NOTIFY listeningChanged)
    Q_PROPERTY(NetServer* server READ server)
    Q_PROPERTY(QQmlListProperty<Peer> peers READ qmlPeerList NOTIFY qmlPeersChanged)
    Q_PROPERTY(int messagesReceived READ messagesReceived NOTIFY messagesReceivedChanged)
    Q_PROPERTY(qreal rxSpeed READ rxSpeed NOTIFY rxSpeedChanged)


public:
    friend class NetServer;
    friend class Peer;
    friend class PeerWorker;
    friend class MessageWorker;
    explicit NetMessenger(QObject *parent = 0, bool cpp = false);
    virtual ~NetMessenger();
    int serverPort() const { return m_serverPort; }
    bool listening() const { return m_server->isListening(); }
    QQmlListProperty<Peer> qmlPeerList();
    NetServer *server() const { return m_server; }

    void classBegin() {}
    void componentComplete();
    void registerMessageHandler(MessageType type, HandlerFn func);
    void registerMessageHandler(int msgType, HandlerFn func);
    void removePeer(Peer *peer);
    int messagesReceived() const;
    void timerEvent(QTimerEvent *e);
    qreal rxSpeed() const;
    void enqueueMessage(Message *msg, Peer *sender);

    void startServer();

signals:
    void serverPortChanged(int arg);
    void listeningChanged(bool arg);

    void peerAdded(Peer *peer);
    void peerRemoved(Peer *peer);

    void peerActivated(Peer *peer);
    void peerDeactivated(Peer *peer);

    void qmlPeersChanged();

    void messagesReceivedChanged(int arg);

    void rxSpeedChanged(qreal arg);
    void messageReceived();

public slots:
    void setServerPort(int arg);
    void setListening(bool arg);
    void addPeer(Peer *peer);
    Peer *connectToHost(QUrl &url);

private slots:
    void updatePeerState(Peer *peer);
    void sendHandshake(Peer *peer);


private:
    NetServer *m_server;
    static void qmlAppendPeer(QQmlListProperty<Peer> *list, Peer *peer);
    static int qmlPeersCount(QQmlListProperty<Peer> *list);
    static Peer *qmlPeerAt(QQmlListProperty<Peer> *list, int index);
    void removePeerHelper(QObject *peer);
    MessageWorker *m_msgWorker;
    QList<Peer *> m_peers;
    QSet<Peer *> m_activePeers;
    QVector<HandlerFn> m_messageHandlers;
    int m_serverPort;
    bool m_listening;
    bool m_ready;
    int perfTimer;

    qreal m_rxSpeed;
};

#endif // NETMESSENGER_H
