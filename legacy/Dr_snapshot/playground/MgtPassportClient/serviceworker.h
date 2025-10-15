#ifndef SERVICEWORKER_H
#define SERVICEWORKER_H

#include <QObject>
#include <QAbstractSocket>
class QTcpSocket;

class ServiceWorker : public QObject
{
    Q_OBJECT

    static const quint32 magic = 0xfcb0ad53; // protocol magic number

    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged)
    Q_PROPERTY(quint16 port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(bool autoReconnect READ autoReconnect WRITE setAutoReconnect NOTIFY autoReconnectChanged)


    QTcpSocket* m_socket;
    quint32 m_connectionRetryCount;
    quint32 m_blockSize;

public:
    explicit ServiceWorker();
    ~ServiceWorker();

signals: // tcp socket forwarding signals
//    void finished(); // worker is done can be deleted // ?? do we need that?
    void connected(); // worker socket is connected
    void disconnected(); // worker socket is disconnected
    void errorHappened(QAbstractSocket::SocketError error);
    void stateChanged(QAbstractSocket::SocketState state);
    void recievedMessage( QVariantMap message );

public slots:
    void start();
    void connectToHost();
    void disconnectFromHost();
    void sendMessage( QVariantMap message );


private slots: // tcp socket slots
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError error);
    void onStateChanged(QAbstractSocket::SocketState state);
    void onReadyRead();


// ------------- property stuff here --------------- //


private: // property members
    QString m_host;
    quint16 m_port;
    bool m_autoReconnect;

public: // property getters
    QString host() const
    {
        return m_host;
    }
    quint16 port() const
    {
        return m_port;
    }
    bool autoReconnect() const
    {
        return m_autoReconnect;
    }

signals: // property signals
    void hostChanged(QString arg);
    void portChanged(quint16 arg);
    void autoReconnectChanged(bool arg);

public slots: // property setters
    void setHost(QString arg)
    {
        if (m_host == arg)
            return;

        m_host = arg;
        emit hostChanged(arg);
    }
    void setPort(quint16 arg)
    {
        if (m_port == arg)
            return;

        m_port = arg;
        emit portChanged(arg);
    }
    void setAutoReconnect(bool arg)
    {
        if (m_autoReconnect == arg)
            return;

        m_autoReconnect = arg;
        emit autoReconnectChanged(arg);
    }
};

#endif // SERVICEWORKER_H
