#include "server.h"
#include <QTimer>
#include <QThread>
#include "connection.h"
#include <QDebug>
#include <QDateTime>
#include <QSqlQuery>
void Server::keepAliveQuery() const
{
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    int delay;
    if( QDateTime::currentMSecsSinceEpoch() - m_lastActivity >= m_keepAliveInterval ) {
        QSqlQuery q( m_database );
        q.exec("DO 1;");
        qDebug() << QDateTime::currentDateTime().toString( "[ dd/MM/yyyy hh:mm:ss.zzz ]" ).toStdString().c_str() << "[ Server ] sending keepalive to database";
        m_lastActivity = now;
        delay = (int)(m_keepAliveInterval - (now - m_lastActivity));
    }else{
        delay = (int)(m_keepAliveInterval + (now - m_lastActivity));
    }
    QTimer::singleShot( delay, this, &Server::keepAliveQuery );
}

void Server::renewActivity(qint64 timestamp) const
{
    if( m_lastActivity < timestamp ) m_lastActivity = timestamp;
}

Server::Server() :
    QTcpServer(nullptr),
    m_reopenDatabaseDelay( 1000 )
{
}

Server::~Server()
{
    closeDatabase();
    qDebug() << QDateTime::currentDateTime().toString( "[ dd/MM/yyyy hh:mm:ss.zzz ]" ).toStdString().c_str() << "[ Server ] destructor called.";
}

bool Server::initialize()
{
    if( !m_settings.initialize() ) {
        qDebug() << QDateTime::currentDateTime().toString( "[ dd/MM/yyyy hh:mm:ss.zzz ]" ).toStdString().c_str() << "[ Server ] Settings corrupted. Exiting.";
        return false;
    }
    m_reopenDatabaseDelay = m_settings.data()[ "db" ].toMap()[ "reopenDatabaseDelay" ].toInt();

    m_database = QSqlDatabase::addDatabase( m_settings.data()[ "db" ].toMap().value("driver").toString(), "connection1" );
    m_database.setConnectOptions( m_settings.data()[ "db" ].toMap()[ "connectOptionsString" ].toString() );
    m_database.setPort( m_settings.data()[ "db" ].toMap()[ "port" ].toInt() );
    m_database.setDatabaseName( m_settings.data()[ "db" ].toMap()[ "database" ].toString() );
    m_database.setUserName( m_settings.data()[ "db" ].toMap()[ "user" ].toString() );
    m_database.setPassword( m_settings.data()[ "db" ].toMap()[ "password" ].toString() );

    m_keepAliveInterval = m_settings.data()[ "db" ].toMap()[ "keepAliveInterval" ].toInt() * 1000;

//    qDebug() << "setting keepalive interval" << m_keepAliveInterval;

    QTimer::singleShot( 0, this, &Server::openDatabase );

    if( !listen( QHostAddress::Any, m_settings.data()[ "server" ].toMap()[ "port" ].toInt() ) ) {
        qDebug() << QDateTime::currentDateTime().toString( "[ dd/MM/yyyy hh:mm:ss.zzz ]" ).toStdString().c_str() << "[ Server ] server cannot listen to port" << m_settings.data()[ "server" ].toMap()[ "port" ].toInt();
        return false;
    }

    qDebug() << QDateTime::currentDateTime().toString( "[ dd/MM/yyyy hh:mm:ss.zzz ]" ).toStdString().c_str() << "[ Server ] server is listening on" << m_settings.data()[ "server" ].toMap()[ "port" ].toInt();
    return true;
}

void Server::incomingConnection(qintptr handle)
{
    qDebug() << QDateTime::currentDateTime().toString( "[ dd/MM/yyyy hh:mm:ss.zzz ]" ).toStdString().c_str() << "[ Server ] incoming connection" << handle;
    QThread* thread = new QThread;

    Connection* connection = new Connection( handle, &m_database, &m_settings, this );
    connection->moveToThread( thread );

    connect( connection, &Connection::finished, thread, &QThread::quit );
    connect( connection, &Connection::dataManipulation, this, &Server::onDataManipulation );
    connect( connection, &Connection::activity, this, &Server::renewActivity );
    connect( this, &Server::onDataManipulation, connection, &Connection::onDataManipulation );
    connect( this, &Server::databaseOpenedChanged, connection, &Connection::onDatabaseOpenedChanged );
    connect( thread, &QThread::finished, connection, &QThread::deleteLater );
    connect( thread, &QThread::finished, connection, &Connection::deleteLater );
    connect( thread, &QThread::started, connection, &Connection::start );

    thread->start();
}


void Server::openDatabase()
{
    qDebug() << QDateTime::currentDateTime().toString( "[ dd/MM/yyyy hh:mm:ss.zzz ]" ).toStdString().c_str() << "[ Server ] opening database";
    if( !m_database.isOpen() ){
        m_database.setHostName( m_settings.data()[ "db" ].toMap()[ "host1" ].toString() );
        m_database.open();
    }
    if( !m_database.isOpen() ){
        m_database.setHostName( m_settings.data()[ "db" ].toMap()[ "host2" ].toString() );
        m_database.open();
    }
    if( !m_database.isOpen() ) {
        QTimer::singleShot( m_reopenDatabaseDelay, this, &Server::openDatabase );
        qDebug() << QDateTime::currentDateTime().toString( "[ dd/MM/yyyy hh:mm:ss.zzz ]" ).toStdString().c_str() << "[ Server ] database did not open. trying later";
    }else{
        m_lastActivity = QDateTime::currentMSecsSinceEpoch();
        QTimer::singleShot((int)m_keepAliveInterval, this, &Server::keepAliveQuery );
//        qDebug() << "started keepalive timer" << m_keepAliveInterval;
        qDebug() << QDateTime::currentDateTime().toString( "[ dd/MM/yyyy hh:mm:ss.zzz ]" ).toStdString().c_str() << "[ Server ] database did open";
    }
}

void Server::closeDatabase()
{
    if( m_database.isOpen() )
        m_database.close();
}

bool Server::databaseOpened() const
{
    return m_database.isOpen();
}
