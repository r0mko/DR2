#ifndef T2CWORKER_H
#define T2CWORKER_H

#include <QObject>
#include <QHostAddress>
#include "processvariable.h"

class QTcpSocket;
class QWaitCondition;
class T2CWorker: public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString host READ host WRITE setHost)
    Q_PROPERTY(int port READ port WRITE setPort)
    Q_PROPERTY(int reconnectDelay READ reconnectDelay WRITE setReconnectDelay)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(QString version READ version NOTIFY versionChanged)
    Q_PROPERTY(int maxIdf READ maxIdf NOTIFY maxIdfChanged)
    Q_PROPERTY(int maxInp READ maxInp NOTIFY maxInpChanged)
    Q_PROPERTY(int maxOut READ maxOut NOTIFY maxOutChanged)
    Q_PROPERTY(int symLevel READ symLevel WRITE setSymLevel NOTIFY symLevelChanged)
    Q_PROPERTY(QString encoding READ encoding WRITE setEncoding)




    bool            m_started       {false};
    bool            m_t2cok         {false};
    QTcpSocket     *m_socket        {nullptr};
    bool            m_autoReconnect {true};
    QTextCodec     *m_codec         {nullptr};


    void connectToT2C();
    void disconnectFromT2C();
    QByteArray blockingInit( const QByteArray &command );
    void init();
    void validateReplies(const QSet<QByteArray> &replies);
    void validateReply(const QByteArray &reply);



    // property members
    QString     m_host;
    quint16     m_port{0};
    int         m_reconnectDelay{500};
    bool        m_connected{false};
    QString     m_version;
    int         m_maxIdf{0};
    int         m_maxInp{0};
    int         m_maxOut{0};
    int         m_symLevel{0};

    QString m_encoding;

public:
    T2CWorker(QObject *parent = nullptr);

    bool sendCommand( const QByteArray &command );
    QString getName(int idf);


//    void sendCommand(QByteArray command, QWaitCondition *waiter = 0, bool waitForReply = false);
//    QByteArray& sendCommandBlocking(const QByteArray &command);

    // property getters
    QString host() const;
    int port() const;
    int reconnectDelay() const;
    bool connected() const;

    QString version() const
    {
        return m_version;
    }

    int maxIdf() const
    {
        return m_maxIdf;
    }

    int maxInp() const
    {
        return m_maxInp;
    }

    int maxOut() const
    {
        return m_maxOut;
    }

    int symLevel() const
    {
        return m_symLevel;
    }

    QString encoding() const
    {
        return m_encoding;
    }

private slots:
//    void sendCommand(const QByteArray &cmd) const;



    // socket slots
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError(QAbstractSocket::SocketError socketError);
    void onSocketStateChanged(QAbstractSocket::SocketState socketState);
    void onSocketReadyRead();



public slots:
    void start(); // bound to qthread start
    void stop(); // bound to qthread stop

    void subscribe(int idf);
    void unsubscribe(int idf);

//    void subscribe(int idf);
//    void sendMessage(QString msg);
//    void setValue(int idf, QString type, QVariant value );

    //property setters
    void setHost(QString host);
    void setPort(int port);
    void setReconnectDelay(int reconnectDelay);



    void setSymLevel(int symLevel)
    {
        if (m_symLevel == symLevel)
            return;

        m_symLevel = symLevel;
        emit symLevelChanged(symLevel);
    }

    void setEncoding(QString encoding)
    {
        m_encoding = encoding;
    }

signals:
//    void gotMessage(QString);

    void started();
    void stopped();

    void gotProcessVariableMessage(ProcessVariable);

//    void validateReplies(QSet<QByteArray> &relpies);

    // property signals
    void connectedChanged(bool connected);
    void versionChanged(QString version);
    void maxIdfChanged(int maxIdf);
    void maxInpChanged(int maxInp);
    void maxOutChanged(int maxOut);
    void symLevelChanged(int symLevel);
};



#endif // T2CWORKER_H
