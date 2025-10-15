#include "server.h"
#include <QTimer>
#include <QThread>
#include "connection.h"
#include <QDebug>

Server::Server() :
    QTcpServer(nullptr),
    m_reopenDatabaseDelay( 1000 )
{
}

Server::~Server()
{
    closeDatabase();
    qDebug() << "[ Server ] destructor called.";
}

bool Server::initialize()
{
    if( !m_settings.initialize() ) {
        qDebug() << "[ Server ] Settings corrupted. Exiting.";
        return false;
    }
    m_reopenDatabaseDelay = m_settings.data()[ "db" ].toMap()[ "reopenDatabaseDelay" ].toInt();

    m_database = QSqlDatabase::addDatabase( "QMYSQL", "connection1" );
    m_database.setConnectOptions( m_settings.data()[ "db" ].toMap()[ "connectOptionsString" ].toString() );
    m_database.setPort( m_settings.data()[ "db" ].toMap()[ "port" ].toInt() );
    m_database.setDatabaseName( m_settings.data()[ "db" ].toMap()[ "database" ].toString() );
    m_database.setUserName( m_settings.data()[ "db" ].toMap()[ "user" ].toString() );
    m_database.setPassword( m_settings.data()[ "db" ].toMap()[ "password" ].toString() );

    QTimer::singleShot( 0, this, &Server::openDatabase );

    if( !listen( QHostAddress::Any, m_settings.data()[ "server" ].toMap()[ "port" ].toInt() ) ) {
        qDebug() << "[ Server ] server cannot listen to port" << m_settings.data()[ "server" ].toMap()[ "port" ].toInt();
        return false;
    }

    qDebug() << "[ Server ] server is listening on" << m_settings.data()[ "server" ].toMap()[ "port" ].toInt();
    return true;
}

void Server::incomingConnection(qintptr handle)
{
    qDebug() << "[ Server ] incoming connection" << handle;
    QThread* thread = new QThread;

    Connection* connection = new Connection( handle, &m_database, &m_settings, this );
    connection->moveToThread( thread );

    connect( connection, &Connection::finished, thread, &QThread::quit );
    connect( this, &Server::databaseOpenedChanged, connection, &Connection::onDatabaseOpenedChanged );
    connect( thread, &QThread::finished, connection, &QThread::deleteLater );
    connect( thread, &QThread::finished, connection, &Connection::deleteLater );
    connect( thread, &QThread::started, connection, &Connection::start );

    thread->start();
}

void Server::openDatabase()
{
    qDebug() << "[ Server ] opening database";
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
        qDebug() << "[ Server ] database did not open. trying later";
    }else{
        qDebug() << "[ Server ] database did open";
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
