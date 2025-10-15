#include "serviceworker.h"
#include <QDataStream>
#include <QTcpSocket>
#include <QTimer>

ServiceWorker::ServiceWorker() :
    QObject( nullptr ),
    m_socket( nullptr ),
    m_connectionRetryCount( 0 ),
    m_blockSize( 0 ),
    m_host( "localhost" ),
    m_port( 33667 ),
    m_autoReconnect( true )
{
}

ServiceWorker::~ServiceWorker()
{
}

void ServiceWorker::start()
{
    qDebug() << "[ Socket ] created";
    m_socket = new QTcpSocket( this );
    connect( m_socket, &QTcpSocket::connected, this, &ServiceWorker::onConnected ); // emit connected state to model
    connect( m_socket, &QTcpSocket::disconnected, this, &ServiceWorker::onDisconnected ); // emit disconnected state to model
    connect( m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)) );
    connect( m_socket, &QTcpSocket::stateChanged, this, &ServiceWorker::onStateChanged );
    connect( m_socket, &QTcpSocket::readyRead, this, &ServiceWorker::onReadyRead );
    connectToHost();
}

void ServiceWorker::connectToHost()
{
    m_connectionRetryCount++;
    qDebug() << "[ Socket ] trying to connect" << m_connectionRetryCount;
    m_socket->connectToHost( m_host, m_port );
}

void ServiceWorker::disconnectFromHost()
{
    m_socket->disconnectFromHost();
}

void ServiceWorker::onConnected()
{
    qDebug() << "[ Socket ] connected after" << m_connectionRetryCount << "reties to" << QString( "%1:%2" ).arg( m_host ).arg( m_port ).toLocal8Bit();
    m_connectionRetryCount = 0;
    emit connected();
}

void ServiceWorker::onDisconnected()
{
    qDebug() << "[ Socket ] disconnected";
    emit disconnected();
}

void ServiceWorker::onError(QAbstractSocket::SocketError error)
{
    qDebug() << "[ Socket ] error:" << error;
    emit errorHappened( error );
}

void ServiceWorker::onStateChanged(QAbstractSocket::SocketState state)
{
//    qDebug() << "[ Socket ] state changed:" << state;
    if( state == QAbstractSocket::UnconnectedState && m_autoReconnect ) QTimer::singleShot( 0, this, &ServiceWorker::connectToHost );
    emit stateChanged( state );
}

void ServiceWorker::onReadyRead()
{
//    qDebug() << "[ Socket ] ready read signal recieved";
    QDataStream in( m_socket );
    in.setVersion( QDataStream::Qt_4_8 );

    if( m_blockSize == 0 ) {
        if( m_socket->bytesAvailable() < (qint64)sizeof(quint32) * 2 ) return;
        in >> m_blockSize;
        quint32 m;
        in >> m;
        if( m != magic ) {
            m_blockSize = 0;
            m_socket->readAll();
            // TODO: send message about it to client
            return;
        }
    }

    if (m_socket->bytesAvailable() < (qint64)m_blockSize - (qint64)sizeof(quint32) ) return;

//    qDebug() << "[ Socket ] recieved a message" << m_socket->bytesAvailable() << "bytes";

    QVariantMap message;
    in >> message;
    emit recievedMessage( message );

    m_blockSize = 0;
    if( m_socket->bytesAvailable() > 0 ) onReadyRead();
}

void ServiceWorker::sendMessage(QVariantMap message)
{
    static int messageId = 0;
    message.insert( "messageId", messageId );
    QByteArray block;
    QDataStream out( &block, QIODevice::WriteOnly );
    out.setVersion( QDataStream::Qt_4_8 );
    out << (quint32)0;
    out << magic;
    out << message;
    out.device()->seek(0);
//    qDebug() << "[ Socket ] sending a message" << block.size() << "bytes";
    out << (quint32)(block.size() - sizeof(quint32));
    m_socket->write(block);
    messageId < 65535 ? messageId++ : messageId = 0;
}

