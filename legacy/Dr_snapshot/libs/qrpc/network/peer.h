#ifndef PEER_H
#define PEER_H
#include <QObject>
#include <QTcpSocket>
#include <QQmlParserStatus>
#include <QUrl>
#include <QDateTime>
#include <QScopedPointer>
#include <QHostAddress>
#include "message.h"

//class Message;
class NetMessenger;
class PeerWorker;
class Peer;
typedef void (*HandlerFn)(Peer*, Message*);

class Peer : public QObject, public QQmlParserStatus
{
    Q_ENUMS(Status QAbstractSocket::SocketError)
    Q_INTERFACES(QQmlParserStatus)

    Q_OBJECT

    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged STORED false)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged STORED false)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QDateTime creationTimestamp READ creationTimestamp CONSTANT)
    Q_PROPERTY(QDateTime lastSeen READ lastSeen NOTIFY lastSeenChanged)
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged STORED false)
    Q_PROPERTY(bool autoReconnect READ autoReconnect WRITE setAutoReconnect NOTIFY autoReconnectChanged)
    Q_PROPERTY(int reconnectTimeout READ reconnectTimeout WRITE setReconnectTimeout NOTIFY reconnectTimeoutChanged)
    Q_PROPERTY(QVariant userData READ userData WRITE setLocalUserData NOTIFY userDataChanged)
public:
    friend class NetMessenger;
    friend class PeerWorker;
    enum Status
    {
        Initializing = 0,
        SocketReady = 1,
        HostLookup = 2,
        Connecting = 3,
        Negotiation = 4,
        WaitingForHello = 5,
        Connected = 6,
        Closing = 7,
        Disconnected = 8,
        Destruction = 9,
        Reconnecting = 10,
        ConnectFailed = 11,
        Error = 64
    };

    explicit Peer(QObject *parent = 0);
    virtual ~Peer();

    Status status() const { return m_status; }
    QUrl url() const { return m_url; }
    QString host() const { return m_url.host(); }
    int port() const { return m_url.port(); }
    QDateTime lastSeen() const { return m_lastSeen; }

    void classBegin() {}
    void componentComplete();

    bool active() const;
    QDateTime creationTimestamp() const { return m_creationTimestamp; }
    NetMessenger *messenger() const;
    QString peerAddress() const;
    void timerEvent(QTimerEvent *e);
    bool autoReconnect() const;
    QVariant userData() const;
    int reconnectTimeout() const;

    void *ptr() const { return m_ptr; }
    void setPtr(void *ptr) { m_ptr = ptr; }

    PeerWorker *worker() const;

public slots:
    void setUrl(QUrl arg);
    void setHost(QString arg);
    void setPort(int arg);
    void setActive(bool arg);
    void setRemoteUserData(QVariant data);
    void setLocalUserData(QVariant arg);
    void setAutoReconnect(bool arg);
    void setReconnectTimeout(int arg);

signals:
    void sendMessage(Message *msg);
    void initAsServer(qint64 handle);
    void initAsClient(const QUrl &url);

    void urlChanged(QUrl arg);
    void hostChanged(QString arg);
    void portChanged(int arg);

    void connected();
    void disconnected();
    void activated();
    void deactivated();

    void statusChanged(Peer *peer);
    void socketErrorChanged(QAbstractSocket::SocketError error);
    void lastSeenChanged(QDateTime arg);
    void activeChanged(bool arg);

    void protocolVersionChanged(int arg);
    void autoReconnectChanged(bool arg);
    void userDataChanged(QVariant arg);
    void unknownMessageReceived(int type);
    void negativeAckReceived(int type, QDateTime time);
    void reconnectTimeoutChanged(int arg);

private slots:
    void updateSocketState(QAbstractSocket::SocketState socketState);
    void setStatus(Peer::Status s);

private:
    static void handleHandshakeMsg(Peer *peer, Message *message);
    static void handleSetUserDataMessage(Peer *peer, Message *message);
    static void handleNegativeAckMessage(Peer *peer, Message *message);

    static int statusEnumIdx;
    void updateLastSeen();
    void setMessenger(NetMessenger *messenger);
    int m_reconnectTimer;
    Status m_status;
    QUrl m_url;
    QDateTime m_lastSeen;
    QDateTime m_creationTimestamp;
    bool m_active;
    bool m_componentComplete;
    NetMessenger *m_messenger;
    PeerWorker *m_worker;
    bool m_autoReconnect;
    QVariant m_userData;
    void *m_ptr;
    int m_reconnectTimeout;
};


class PeerWorker : public QObject
{
    Q_OBJECT
    Peer *m_peer;
    QTcpSocket *m_socket;

public:
    PeerWorker(Peer *peer) : m_peer(peer), m_socket(nullptr)
    { }
    ~PeerWorker() {}

    bool initialized() const { return m_socket != nullptr; }
    bool isOpen() const { return m_socket->isOpen(); }
    QTcpSocket *socket() const;
    void waitForInitialized();
signals:
    void sendMessage(Message *msg);
    void messageReceived(Message *msg, Peer *peer);
    void updateState(Peer::Status state);
    void initializedChanged(bool);

public slots:
    void shutdown();
    void initPassive(qintptr handle);
    void initActive(const QUrl &url);
    void writeMessage(Message *msg);
    void init();
    void readMessage();

};
QDebug operator<<(QDebug dbg, Peer::Status s);
//QDebug operator<<(QDebug, QAbstractSocket::SocketState);

Q_DECLARE_METATYPE(Peer*)
Q_DECLARE_METATYPE(Peer::Status)
#endif // PEER_H
