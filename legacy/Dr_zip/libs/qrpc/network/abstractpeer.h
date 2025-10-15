#ifndef ABSTRACTPEER_H
#define ABSTRACTPEER_H

#include <QObject>
#include <QTcpSocket>
#include <QtQml>

//class AbstractPeer : public QObject
//{
//    Q_ENUMS(Status QAbstractSocket::SocketError)
//    Q_INTERFACES(QQmlParserStatus)

//    Q_OBJECT

//    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
//    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged STORED false)
//    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged STORED false)
//    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
//    Q_PROPERTY(QAbstractSocket::SocketError socketError READ socketError NOTIFY socketErrorChanged STORED false)
//    Q_PROPERTY(QString socketErrorString READ socketErrorString STORED false)
//    Q_PROPERTY(QDateTime creationTimestamp READ creationTimestamp CONSTANT)
//    Q_PROPERTY(QDateTime lastSeen READ lastSeen NOTIFY lastSeenChanged)

//public:
//    enum Status
//    {
//        Initializing = 0,
//        SocketReady = 1,
//        HostLookup = 2,
//        Connecting = 3,
//        Negotiation = 4,
//        WaitingForHello = 5,
//        Connected = 6,
//        Closing = 7,
//        Disconnected = 8,
//        Destruction = 9,
//        Reconnecting = 10,
//        ConnectFailed = 11,
//        Error = 64
//    };
//    explicit AbstractPeer(QObject *parent = 0);

//signals:

//public slots:

//};

#endif // ABSTRACTPEER_H
