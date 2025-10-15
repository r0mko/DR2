#ifndef T2CWORKER2_H
#define T2CWORKER2_H

#include <QObject>
#include <QHostAddress>

class QTcpSocket;
class T2CWorker2: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString host READ host WRITE setHost)
    Q_PROPERTY(int port READ port WRITE setPort)


    QSet<int>       m_queue;
    QSet<int>       m_subscribed;
    bool            m_started       {false};
    bool            m_t2cok         {false};
    QTcpSocket     *m_socket        {nullptr};

    void connectToT2C();
    void disconnectFromT2C();
    inline QByteArray cmdTest() const { return "T2C VER\n"; }
    inline QByteArray cmdListen(int idf) const { return QString( "ANM 0 ORG %1\n" ).arg(idf).toLatin1(); }
    void parse(const QByteArray& line);

private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError(QAbstractSocket::SocketError socketError);
    void onSocketStateChanged(QAbstractSocket::SocketState socketState);
    void onSocketReadyRead();

    void sendCommand(const QByteArray &cmd) const;

signals:
    void gotMessage(QString);


public:
    T2CWorker2(QObject *parent = nullptr);

public slots:
    void start(); // bound to qthread start
    void stop(); // bound to qthread stop
    void subscribe(int idf);
    void sendMessage(QString msg);
    void setValue(int idf, QString type, QVariant value );

signals:
    void started();
    void stopped();



private:
    QString m_host;
    quint16 m_port;

public:
    QString host() const;
    int port() const;

public slots:
    void setHost(QString host);
    void setPort(int port);

};



#endif // T2CWORKER2_H
