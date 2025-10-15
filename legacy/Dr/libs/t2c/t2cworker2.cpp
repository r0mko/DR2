#include "t2cworker2.h"
#include <QDebug>
#include <QTcpSocket>
#include <QTimer>


void T2CWorker2::establishConnection()
{
    if (m_socket == nullptr) {
        qWarning() << "somehow socket is nullptr";
        return;
    }
    qDebug() << "trying to connect";
    m_socket->connectToHost(m_host, m_port);
}

void T2CWorker2::parse(const QByteArray &line)
{
    if( !m_t2cok && line == "04.01" ) {
        m_t2cok = true;
        qDebug() << "T2C alive";
        for (auto idf: m_queue) sendCommand(cmdListen(idf));
    }
    emit gotMessage(line.trimmed());
}

void T2CWorker2::onSocketConnected()
{
    qDebug() << "connected to host";
    sendCommand(cmdTest());
}

void T2CWorker2::onSocketDisconnected()
{
    qDebug() << "disconnected from host";
}

void T2CWorker2::onSocketError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "socket error" << socketError << m_socket->errorString();
}

void T2CWorker2::onSocketStateChanged(QAbstractSocket::SocketState socketState)
{
    qDebug() << "socket state changed" << socketState << m_socket->state();
    if( m_started ) {
        if (socketState == QAbstractSocket::UnconnectedState)
            QTimer::singleShot(500, this, &T2CWorker2::establishConnection);
        else
            qDebug() << "state was not unconnected";
    }
}

void T2CWorker2::onSocketReadyRead()
{
    static QByteArray data;
    while (m_socket->bytesAvailable())
        data.append(m_socket->readAll());

    auto lines = data.split('\n');

    if (data.endsWith('\n')) data.clear();
    else data = lines.takeLast();

    for( const QByteArray &line: lines ) parse(line);
}

void T2CWorker2::sendCommand(const QByteArray &cmd) const
{
    m_socket->write(cmd);
}

T2CWorker2::T2CWorker2(QObject *parent) :
    QObject(parent)
{
}

void T2CWorker2::start()
{
    m_socket = new QTcpSocket(this);
    m_started = true;
    connect(m_socket, &QTcpSocket::connected, this, &T2CWorker2::onSocketConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &T2CWorker2::onSocketDisconnected);
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onSocketError(QAbstractSocket::SocketError)));
    connect(m_socket, &QTcpSocket::stateChanged, this, &T2CWorker2::onSocketStateChanged);
    connect(m_socket, &QTcpSocket::readyRead, this, &T2CWorker2::onSocketReadyRead);
    establishConnection();
    emit started();
}

void T2CWorker2::stop()
{
    m_started = false;
    emit stopped();
}

void T2CWorker2::subscribe(int idf)
{
    if (m_t2cok)
        sendCommand(cmdListen(idf));
    else
        m_queue.insert(idf);
}

QString T2CWorker2::host() const
{
    return m_host;
}

int T2CWorker2::port() const
{
    return m_port;
}

void T2CWorker2::setHost(QString host)
{
    if (m_host == host)
        return;

    m_host = host;
}

void T2CWorker2::setPort(int port)
{
    if (m_port == port)
        return;

    m_port = port;
}
