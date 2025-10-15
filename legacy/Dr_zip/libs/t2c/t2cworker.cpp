#include "t2cworker.h"
#include <QBitArray>
#include <QDateTime>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QTcpSocket>
#include <QTimer>
#include <QTextCodec>

void T2CWorker::connectToT2C()
{
    if (m_socket == nullptr) {
        qWarning() << "somehow socket is nullptr";
        return;
    }
    if (m_host.isEmpty() || m_port == 0) {
        qWarning() << "set host and port please";
        return;
    }
    qDebug() << "connecting from T2C";
    m_socket->connectToHost(m_host, m_port);
}
void T2CWorker::disconnectFromT2C()
{
    if (m_socket == nullptr) {
        qWarning() << "somehow socket is nullptr";
        return;
    }
    qDebug() << "disconnecting from T2C";
    m_socket->disconnectFromHost();
}
QByteArray T2CWorker::blockingInit(const QByteArray &command)
{

    qint64 blockingWaitTimeout = 2000;
    qint64 startTime = QDateTime::currentMSecsSinceEpoch();
    if( !sendCommand(command) )
        return QByteArray();
    QByteArray result;
    while(!result.contains('\n')) {
        m_socket->waitForReadyRead();
        result += m_socket->readAll();
        if( QDateTime::currentMSecsSinceEpoch() - startTime > blockingWaitTimeout ) {
            qWarning() << "blocking request timedout";
            return QByteArray();
        }
    }
    int bytes = result.indexOf('\n') + 1;
    return result.left(bytes).simplified();
}
void T2CWorker::init()
{
    m_version = blockingInit( "T2C VER" );
    if( m_version.isEmpty() ) {
        qWarning() << "version shouldn't be empty";
        return;
    }
    m_maxIdf = blockingInit( "T2C PRP MAX_IDF" ).toInt();
    m_maxInp = blockingInit( "T2C PRP MAX_INP" ).toInt();
    m_maxOut = blockingInit( "T2C PRP MAX_OUT" ).toInt();
//    connect(m_socket, &QTcpSocket::readyRead, this, &T2CWorker::onSocketReadyRead);
    m_connected = true;
    emit connectedChanged(true);
}

void T2CWorker::validateReplies(const QSet<QByteArray> &replies)
{
    for( auto it = replies.constBegin(); it != replies.constEnd(); ++it )
        validateReply( *it );
}

void T2CWorker::validateReply(const QByteArray &reply)
{
    if( reply.isEmpty() ) return;
        QString sreply = m_codec->toUnicode(reply);
    QRegularExpression re( R"regexp(^DAT\s+(?<symLevel>\d)\s+(?<idf>\d+)\s+(?<type>(?<typename>\S)(?<typesize>\d+))\s+(?<value>\S+|".+")\s+(?<status>\S+)\s+(?<time>\d+)\.(?<msec>\d+))regexp" );
    QRegularExpressionMatch match = re.match( sreply );
    if( match.hasMatch() ) {
        qDebug() << reply.simplified();

        ProcessVariable pv;
        pv.setIdf( match.captured("idf").toInt() );
        pv.setValueType( match.captured("type").toLatin1());
        pv.setTime( QDateTime::fromMSecsSinceEpoch( match.captured( "time" ).toLongLong() * 1000 + match.captured( "msec" ).toLongLong() ));
        if( match.captured("typename") == "I" )
            pv.setValue( match.captured("value").toInt() );
        else if( match.captured("typename") == "F" ) {
            if( match.captured("typesize").toInt() == 32 )
                pv.setValue( match.captured("value").toFloat() );
            else if( match.captured("typesize").toInt() == 64 )
                pv.setValue( match.captured("value").toDouble() );
        }else if( match.captured("typename") == "B" ) {
            QBitArray bits;
            bits.resize(match.captured("typesize").toInt());
            int v = match.captured("value").toInt();
            for( int i = 0; i < bits.size(); ++i )
                bits[i] = v & ( 1 << i );
            pv.setValue( bits );
        }else if( match.captured("typename") == "T" ) {
            QString s = match.captured("value").simplified();
            if( s.startsWith('"') ) s = s.mid(1);
            if( s.endsWith('"')) s.chop(1);
            pv.setValue( s );
        }
        int status{0};
        auto st = match.captured("status");
        for(int i=0; i<st.size(); ++i)
            if(st.at(i) != '_')
                status += 1 << i;
        pv.setStatus(status);
        emit gotProcessVariableMessage(pv);
    }
}






T2CWorker::T2CWorker(QObject *parent) :
    QObject(parent),
    m_codec(QTextCodec::codecForName("ISO 8859-5"))
{

}

bool T2CWorker::sendCommand(const QByteArray &command)
{
    if( m_socket == nullptr){
        qWarning() << "socket is nullptr";
        return false;
    }
    if( !m_socket->isReadable() || !m_socket->isWritable() ) {
        qWarning() << "socket is not readable and/or writable";
        return false;
    }
    m_socket->write(command);
    m_socket->write("\n");
    return true;
}

QString T2CWorker::getName(int idf)
{
    qint64 blockingWaitTimeout = 2000;
    qint64 startTime = QDateTime::currentMSecsSinceEpoch();
    if( !sendCommand( QString("DAS %1").arg(idf).toLocal8Bit() ) )
        return QString();
    QByteArray result;
    while(!result.contains('\n')) {
        m_socket->waitForReadyRead();
        result += m_socket->readAll();
        if( QDateTime::currentMSecsSinceEpoch() - startTime > blockingWaitTimeout ) {
            qWarning() << "blocking request timedout";
            return QString();
        }
    }
    int bytes = result.indexOf('\n') + 1;

    return m_codec->toUnicode(result.left(bytes).simplified().split('"')[1]);
}


QString T2CWorker::host() const
{
    return m_host;
}

int T2CWorker::port() const
{
    return m_port;
}

int T2CWorker::reconnectDelay() const
{
    return m_reconnectDelay;
}

bool T2CWorker::connected() const
{
    return m_connected;
}

void T2CWorker::onSocketConnected()
{
    qDebug() << "connected to host" << m_host << ":" << m_port;
    init();
}

void T2CWorker::onSocketDisconnected()
{
    qDebug() << "disconnected from host";
    m_connected = false;
    emit connectedChanged(false);
}

void T2CWorker::onSocketError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "socket error" << socketError << m_socket->errorString();
}

void T2CWorker::onSocketStateChanged(QAbstractSocket::SocketState socketState)
{
    qDebug() << "socket state changed" << socketState << m_socket->state();
    m_connected = false;
    emit connectedChanged(false);
    if (socketState == QAbstractSocket::UnconnectedState) {
        if (m_autoReconnect) {
            qDebug() << "socket moved to UnconnectedState trying to reconnect in" << m_reconnectDelay << "ms";
            QTimer::singleShot(m_reconnectDelay, this, &T2CWorker::connectToT2C);
        }
    }else{
//        qDebug() << "state was not unconnected";
    }
}

void T2CWorker::onSocketReadyRead()
{
    static QByteArray data;
    while (m_socket->bytesAvailable())
        data.append(m_socket->readAll());
    auto lines = data.split('\n');
    if (data.endsWith('\n'))
        data.clear();
    else data = lines.takeLast();
    validateReplies(lines.toSet());
    if( m_socket->bytesAvailable() )
        QTimer::singleShot( 0, this, &T2CWorker::onSocketReadyRead);
}

void T2CWorker::start()
{
    m_socket = new QTcpSocket(this);
    m_autoReconnect = true;
    connect(m_socket, &QTcpSocket::connected, this, &T2CWorker::onSocketConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &T2CWorker::onSocketDisconnected);
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onSocketError(QAbstractSocket::SocketError)));
    connect(m_socket, &QTcpSocket::stateChanged, this, &T2CWorker::onSocketStateChanged);

    connectToT2C();
    emit started();
}

void T2CWorker::stop()
{
    m_autoReconnect = false;
    disconnectFromT2C();
    emit stopped();
}

void T2CWorker::subscribe(int idf)
{
    sendCommand( QString( "ANM %1 ORG %2" ).arg( m_symLevel ).arg( idf ).toLocal8Bit() );
//    }
}

void T2CWorker::unsubscribe(int idf)
{
    sendCommand( QString( "ANM %1 END %2" ).arg( m_symLevel ).arg( idf ).toLocal8Bit() );
}

void T2CWorker::setHost(QString host)
{
    if (m_host == host)
        return;

    m_host = host;
}

void T2CWorker::setPort(int port)
{
    if (m_port == port)
        return;

    m_port = port;
}

void T2CWorker::setReconnectDelay(int reconnectDelay)
{
    m_reconnectDelay = reconnectDelay;
}
