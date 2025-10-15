#include "connection.h"
#include <QTcpSocket>
#include "server.h"
#include <QTimer>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QCoreApplication>
#include <QQmlEngine>
#include <QDataStream>
Connection::Connection(quintptr handle, QSqlDatabase *database, Settings *settings, Server* server) :
    QObject( nullptr ),
    m_handle( handle ),
    m_database( database ),
    m_settings( settings ),
    m_server( server ),
    m_socket( nullptr ),
    m_blockSize( 0 ),
    m_rowCount( 0 ),
    m_authorized( false ),
    m_userId( -1 ),
    m_canRead( false ),
    m_canWrite( false ),
    m_canManageUsers( false )
{
    m_this = m_engine.newQObject( this );
    QQmlEngine::setObjectOwnership( this, QQmlEngine::CppOwnership );
    m_engine.globalObject().setProperty( "db", m_this );
    connect( this, &Connection::loginRetry, this, &Connection::processLoginMessage, Qt::QueuedConnection );
}

Connection::~Connection()
{
    qDebug() << "[ Connection" << m_handle << "] destructor called";
    emit finished();
}

QVariant Connection::exec(QString query, QVariantMap bindings)
{
//    qDebug() << query << bindings;
//    int querytype;
//    if( query.startsWith( "select", Qt::CaseInsensitive ) ) querytype = 0;
//    else if( query.startsWith( "delete", Qt::CaseInsensitive ) ) querytype = 1;
//    else if( query.startsWith( "update", Qt::CaseInsensitive ) ) querytype = 2;
//    else if( query.startsWith( "insert", Qt::CaseInsensitive ) ) querytype = 3;
//    else querytype = 4;

//    static QSqlQuery q( *m_database );
//    q.prepare( query );
//    if( q.lastError().type() != QSqlError::NoError ) {
//        QSqlError e = q.lastError();
//        qDebug() << " ===>> SQL ERROR" << e.text();
//        QVariantMap result;
//        result.insert( "error", e.text() );
//        result.insert( "when", "prepairing query " + query );
//        return result;
//    }
//    auto it = bindings.constBegin();
//    QString k;
//    QVariant v;
//    while( it != bindings.constEnd() ) {
//        k = it.key();
//        v = it.value();
//        q.bindValue( k.prepend( ':' ), v );
//        if( q.lastError().type() != QSqlError::NoError ) {
//            QSqlError e = q.lastError();
//            qDebug() << " ===>> SQL ERROR" << e.text();
//            QVariantMap result;
//            result.insert( "error", e.text() );
//            result.insert( "when", "binding " + it.key() + " = " + it.value().toString() );
//            return result;
//        }
//        it++;
//    }
//    q.exec();
//    if( q.lastError().type() != QSqlError::NoError ) {
//        QSqlError e = q.lastError();
//        qDebug() << " ===>> SQL ERROR" << e.text();
//        QVariantMap result;
//        result.insert( "error", e.text() );
//        result.insert( "when", "executed query " + query + " and bindings " + bindings );
//        return result;
//    }

//    if( querytype == 0 ) {

//    }
//    else if( querytype == 1 ) {

//    }
//    else if( querytype == 2 ) {

//    }
//    else if( querytype == 3 ) {
//        int lii = q.lastInsertId().toInt();
//        qDebug() << "last insert id is:" << lii;
//        return lii;
//    }
//    else if( querytype == 4 ) {

//    }
//    q.next();
//    QVariant x = q.value( 0 );
//    qDebug() << "GOT VALUE " << x;
//    return x;
}

QVariant Connection::dbOperation(QString query, QVariantMap bindings)
{
     qDebug() << query << bindings;
//    qDebug() << "doing query and bindings" << query << bindings;
    static QSqlQuery q( *m_database );
    q.prepare( query );
    if( q.lastError().type() != QSqlError::NoError ) qDebug() << " ===>> SQL ERROR" << q.lastError().text();
    auto it = bindings.constBegin();
    QString k;
    QVariant v;
    while( it != bindings.constEnd() ) {
        k = it.key();
        v = it.value();
        q.bindValue( k.prepend( ':' ), v );
        if( q.lastError().type() != QSqlError::NoError ) qDebug() << " ===>> SQL ERROR" << q.lastError().text();
        it++;
    }
//    qDebug() << "boundValues" << q.boundValues();
    q.exec();
    if( q.lastError().type() != QSqlError::NoError ) qDebug() << " ===>> SQL ERROR" << q.lastError().text();
    int lii = q.lastInsertId().toInt();
    qDebug() << "last insert id is:" << lii;
    return lii;
}

QVariant Connection::dbValue(QString query, QVariantMap bindings)
{
    qDebug() << query << bindings;
    static QSqlQuery q( *m_database );
    q.prepare( query );
    if( q.lastError().type() != QSqlError::NoError ) qDebug() << " ===>> SQL ERROR" << q.lastError().text();
    auto it = bindings.constBegin();
    QString k;
    QVariant v;
    while( it != bindings.constEnd() ) {
        k = it.key();
        v = it.value();
        q.bindValue( k.prepend( ':' ), v );
        if( q.lastError().type() != QSqlError::NoError ) qDebug() << " ===>> SQL ERROR" << q.lastError().text();
        it++;
    }
//    qDebug() << "boundValues" << q.boundValues();
    q.exec();
    if( q.lastError().type() != QSqlError::NoError ) qDebug() << " ===>> SQL ERROR" << q.lastError().text();
    q.next();
    QVariant x = q.value( 0 );
    qDebug() << "GOT VALUE " << x;
    return x;
}

void Connection::processMessage(const QVariantMap &msg)
{
    if( !msg.contains( "messageType" ) ) {
        qDebug() << "[ Connection" << m_handle << "] processing message skipped, no messageType defined";
        QVariantMap m;
        m.insert( "messageType", "error" );
        m.insert( "replyId", msg.value( "messgeId" ).toInt() );
        m.insert( "errorString", "processing message skipped, no messageType defined" );
        sendMessage( m );
        return;
    }
    qDebug() << "[ Connection" << m_handle << "] processing message type" << msg.value( "messageType" ).toString();


    const QString& messageType = msg.value( "messageType" ).toString();

    if( messageType == QString( "hello" ) ) {
        processHelloMessage( msg );
        return;
    }
    else if( messageType == QString( "serverStatus" ) ) {
        processServerStatusMessage( msg );
        return;
    }

    if( !m_authorized ) {
        processLoginMessage( msg );
        return;
    }

    if( messageType == QString( "dataRequest" ) ) processDataRequestMessage( msg );
    else if( messageType == QString( "login" ) ) processLoginMessage( msg );
    else if( messageType == QString( "logout" ) ) processLogoutMessage( msg );
    else if( messageType == QString( "stopServer" ) ) processStopServerMessage( msg );
    else if( messageType == QString( "dataAdd" ) ) processDataAddMessage( msg );
    else if( messageType == QString( "dataDel" ) ) processDataDelMessage( msg );
    else if( messageType == QString( "dataEdit" ) ) processDataEditMessage( msg );
    else if( messageType == QString( "testf" ) ) processTestfMessage( msg );
    else if( messageType == QString( "serverReloadSettings" ) ) processServerReloadSettingsMessage( msg );


    // TODO: other msg processing
    else qDebug() << "[ Connection" << m_handle << "] unknown message type";
}

void Connection::processLoginMessage( const QVariantMap &msg )
{
//    if( !checkDatabase() ) return;
    checkDatabase();
    const QString& messageType = msg.value( "messageType" ).toString();
    if( messageType != QString( "login" ) ){
        qDebug() << "[ Connection" << m_handle << "] unauthorized client request rejected";
        QVariantMap m;
        m.insert( "messageType", "error" );
        m.insert( "replyId", msg.value( "messgeId" ).toInt() );
        m.insert( "errorString", "unauthorized client request rejected" );
        sendMessage( m );
        return;
    }
    if( m_authorized ){
        qDebug() << "[ Connection" << m_handle << "] already authorized client asked for authorizetion again!";
        QVariantMap m;
        m.insert( "messageType", "error" );
        m.insert( "replyId", msg.value( "messgeId" ).toInt() );
        m.insert( "errorString", "already authorized client asked for authorizetion again!" );
        sendMessage( m );
        return;
    }


    QSqlQuery query( *m_database );
    bool qprep = query.prepare( "SELECT idusers AS id, name AS name, roleName AS role, viewPermission, editPermission, userManagePermission FROM users u, roles r where login = ? AND pass = ? AND u.role = r.idroles;" );
    query.addBindValue( msg.value( "login" ) );
    query.addBindValue( msg.value( "pass" ) );


    bool qexec = query.exec();


    if( !m_database->isOpen() || query.lastError().type() != QSqlError::NoError ){
        qDebug() << "[ Connection" << m_handle << "] database error: " << query.lastError().text();
        m_database->close();

        /* retry */
        emit loginRetry( msg );
        qDebug() << "[ Connection" << m_handle << "] retrying";
        /**/

        /* no retry *
        checkDatabase();
        QVariantMap m;
        m.insert( "messageType", "loginReply" );
        m.insert( "replyId", msg.value( "messgeId" ).toInt() );
        m.insert( "error", query.lastError().text() );
        sendMessage( m );
        /**/
        return;
    }



    while( query.next() ) {


        m_authorized = true;
        m_userId = query.value( 0 ).toInt();
        m_userName = query.value( 1 ).toString();
        m_roleName = query.value( 2 ).toString();
    //    m_login = msg.value( "login" ).toString();
        m_canRead = query.value( 3 ).toBool();
        m_canWrite = query.value( 4 ).toBool();
        m_canManageUsers = query.value( 5 ).toBool();

        qDebug() << "[ Connection" << m_handle << "] user login id: " << m_userId;
        qDebug() << "[ Connection" << m_handle << "] user login name: " << m_userName;
        qDebug() << "[ Connection" << m_handle << "] user login role: " << m_roleName;
        qDebug() << "[ Connection" << m_handle << "] user login can read: " << m_canRead;
        qDebug() << "[ Connection" << m_handle << "] user login can write: " << m_canWrite;
        qDebug() << "[ Connection" << m_handle << "] user login can manage users: " << m_canManageUsers;



        QVariantMap m;
        m.insert( "messageType", "loginReply" );
        m.insert( "replyId", msg.value( "messgeId" ).toInt() );
        m.insert( "userId", m_userId );
        m.insert( "userName", m_userName );
        m.insert( "roleName", m_roleName );
        m.insert( "canRead", m_canRead );
        m.insert( "canWrite", m_canWrite );
        m.insert( "canManageUsers", m_canManageUsers );
        sendMessage( m );
    }
}

void Connection::processHelloMessage(const QVariantMap &msg)
{
    qDebug() << "[ Connection" << m_handle << "] Hello.";
    QVariantMap m;
    m.insert( "messageType", "helloReply" );
    m.insert( "replyId", msg.value( "messgeId" ).toInt() );
    sendMessage( m );
}

void Connection::processLogoutMessage(const QVariantMap &msg)
{
    qDebug() << "[ Connection" << m_handle << "] User logout";

    m_authorized = false;
    m_userId = -1;
    m_userName.clear();
    m_roleName.clear();
    m_canRead = false;
    m_canWrite = false;
    m_canManageUsers = false;

    QVariantMap m;
    m.insert( "messageType", "logoutReply" );
    m.insert( "replyId", msg.value( "messgeId" ).toInt() );
    sendMessage( m );
}

void Connection::processServerStatusMessage(const QVariantMap &msg)
{
    qDebug() << "[ Connection" << m_handle << "] Client asked server status";
    QVariantMap m;
    m.insert( "messageType", "serverStatusReply" );
    m.insert( "replyId", msg.value( "messgeId" ).toInt() );
    m.insert( "serverAlive", true );
    m.insert( "serverDatabaseReady", m_database->isOpen() );
    if( m_authorized ) {
        // other info
    }
    sendMessage( m );
}

void Connection::processStopServerMessage(const QVariantMap &msg)
{
    if( m_canManageUsers ){
        qApp->quit();
        return;
    }
    qDebug() << "[ Connection" << m_handle << "] unknown message type";
}

void Connection::processDataAddMessage(const QVariantMap &msg)
{
    setMsg( msg.value( "data" ).toMap() );
    QString dataSource = msg.value( "dataSource" ).toString();
    QString source = m_settings->data().value( "dataSources" ).toMap().value( dataSource ).toMap().value( "methods" ).toMap().value( "ADD" ).toString();

    QVariant response = m_engine.evaluate( source ).toVariant();
    return;



//    QSqlQuery query( *m_database );

//    QString dataSource = msg.value( "dataSource" ).toString();
//    QString method = msg.value( "method" ).toString();

//qDebug() << dataSource << method;
//    query.prepare( m_settings->data().value( "dataSources" ).toMap().value( dataSource ).toMap().value( "methods" ).toMap().value( method ).toString() );
//    for( const QVariant& k: msg.value( "values" ).toList() )
//    query.addBindValue( k );

//    query.exec();


}

void Connection::processDataDelMessage(const QVariantMap &msg)
{

    setMsg( msg.value( "data" ).toMap() );
    QString dataSource = msg.value( "dataSource" ).toString();
    QString source = m_settings->data().value( "dataSources" ).toMap().value( dataSource ).toMap().value( "methods" ).toMap().value( "DEL" ).toString();

    QVariant response = m_engine.evaluate( source ).toVariant();
    return;


//    QSqlQuery query( *m_database );

//    QString dataSource = msg.value( "dataSource" ).toString();
//    QString method = msg.value( "method" ).toString();

//qDebug() << dataSource << method;
//    query.prepare( m_settings->data().value( "dataSources" ).toMap().value( dataSource ).toMap().value( "methods" ).toMap().value( method ).toString() );
//    query.addBindValue( msg.value( "id" ).toInt() );

//    query.exec();
}

void Connection::processDataEditMessage(const QVariantMap &msg)
{

    setMsg( msg.value( "data" ).toMap() );
    QString dataSource = msg.value( "dataSource" ).toString();
    QString source = m_settings->data().value( "dataSources" ).toMap().value( dataSource ).toMap().value( "methods" ).toMap().value( "UPD" ).toString();

//    qDebug() << "source" << source;
    QVariant response =
            m_engine.evaluate( source ).toVariant();

//    qDebug() << "response" << response;
    return;


//    QSqlQuery query( *m_database );

//    QString dataSource = msg.value( "dataSource" ).toString();
//    QString method = msg.value( "method" ).toString();

//qDebug() << dataSource << method << m_settings->data().value( "dataSources" ).toMap().value( dataSource ).toMap().value( "methods" ).toMap().value( method ).toString();
//    query.prepare( m_settings->data().value( "dataSources" ).toMap().value( dataSource ).toMap().value( "methods" ).toMap().value( method ).toString() );

//    for( const QVariant& k: msg.value( "values" ).toList() )
//    query.addBindValue( k );

//    qDebug() << query.lastError();
//    query.exec();

//    qDebug() << query.lastError();
}

void Connection::processTestfMessage(const QVariantMap &msg)
{
//    setMsg( msg.value( "data" ).toMap() );
//    QString dataSource = msg.value( "dataSource" ).toString();
//    QString source = m_settings->data().value( "dataSources" ).toMap().value( dataSource ).toMap().value( "methods" ).toMap().value( "TEST" ).toString();

//    QVariant response = m_engine.evaluate( source ).toVariant();

}

void Connection::processServerReloadSettingsMessage(const QVariantMap &msg)
{
    if( m_canManageUsers ){
        m_settings->reload();
        return;
    }
    qDebug() << "[ Connection" << m_handle << "] unknown message type";
}

void Connection::processDataRequestMessage(const QVariantMap &msg)
{
    qDebug() << "currentSelectedSiteId" <<msg.value( "currentSelectedSiteId" ).toInt();

    checkDatabase();
    // TODO: msg processing

//qDebug() << msg;
    static bool refreshCount = true;
    static quint32 rowCount = 0;
    QSqlQuery query( *m_database );

//    if( msg.value( "messageType" ).toString() == QString( "dataRequest" ) )
//        refreshCount = false;
//refreshCount = true;

//    if( refreshCount ){
    // if we need to get count - get count
//        qDebug() << sqlSelect( msg, true );

//    }



//    qDebug() << sqlSelect( msg );
    query.prepare( sqlSelect( msg ) );
    query.exec();
    QList<QString> field;
    QVariantList recordList;
    QVariantMap fieldMap;
    while( query.next() ) {
    // get data

        if (field.isEmpty()) {
            const QSqlRecord& record = query.record();
            for (int i = 0; i < record.count(); ++i){
                field.append(record.fieldName(i));
                fieldMap.insert( record.fieldName(i), m_settings->data()[ "dataSources" ].toMap()[ msg.value( "dataSource" ).toString() ].toMap()[ "fields" ].toList().at( i ).toMap()[ "caption" ].toString() );
//                qDebug() << m_settings->data()[ "dataSources" ].toMap()[ source ].toMap()[ "fields" ].toList().at( i ).toMap()[ "caption" ].toString();
            }
        }
        QVariantMap record;
        for (int i = 0; i < field.size(); ++i) {
            QVariant value = query.value(i);
            // JavaScript/QML doesn't support 64bit integers (everything is converted to double)
            // so convert to string here to make sure we don't run into trouble.
            // In the long term it will be cleaner to move this code into something like a
            // query decorator instead of the core query code.
            if (value.type() == QVariant::LongLong || value.type() == QVariant::ULongLong)
                value = value.toString();
            record.insert(field.at(i), value);
        }
        recordList.append( record );
    }
    QVariantMap daat;
    daat.insert( "messageType", "dataReply" );



    if( msg.contains( "getFieldData" ) )
        if( msg.value( "getFieldData" ).toBool() ) {
            daat.insert( "fieldMap", fieldMap );
            daat.insert( "fieldData", m_settings->data()[ "dataSources" ].toMap()[ msg.value( "dataSource" ).toString() ].toMap()[ "fields" ] );
            daat.insert( "editable", m_settings->data()[ "dataSources" ].toMap()[ msg.value( "dataSource" ).toString() ].toMap()[ "editable" ].toBool() );
            daat.insert( "indexField", m_settings->data()[ "dataSources" ].toMap()[ msg.value( "dataSource" ).toString() ].toMap()[ "indexField" ].toString() );
            daat.insert( "readableField", m_settings->data()[ "dataSources" ].toMap()[ msg.value( "dataSource" ).toString() ].toMap()[ "readableField" ].toString() );
        }

    daat.insert( "recordList", recordList );
    daat.insert( "pos", msg.value( "windowPosition" ).toInt() );

    if( msg.contains( "getRowCount" ) )
        if( msg.value( "getRowCount" ).toBool() ) {
            qDebug() << sqlSelect( msg, true );
            query.prepare( sqlSelect( msg, true ) );
            query.exec();
            if( query.size() != 1 ) return;
            query.next();
            daat.insert( "rowCount", query.value( 0 ) );
        }
    sendMessage( daat );
}

void Connection::sendReadyMessage()
{
    QVariantMap readyMessage;
    readyMessage.insert( "messageType", "info" );
    readyMessage.insert( "serverAlive", true );
    readyMessage.insert( "serverDatabaseReady", m_database->isOpen() );
    sendMessage( readyMessage );
}

bool Connection::checkDatabase()
{
    if( !m_server->databaseOpened() ) {
        qDebug() << "[ Connection" << m_handle << "] we lost database connection";
        QTimer::singleShot( 0, m_server, &Server::openDatabase );
        QVariantMap m;
        m.insert( "messageType", "info" );
        m.insert( "serverAlive", true );
        m.insert( "serverDatabaseReady", m_database->isOpen() );
        sendMessage( m );
        return false;
    }
    return true;
}

void Connection::onConnected()
{
    qDebug() << "[ Connection" << m_handle << "] connected.";
}

void Connection::onDisconnected()
{
    qDebug() << "[ Connection" << m_handle << "] disconnected";
}

void Connection::onError( QAbstractSocket::SocketError error )
{
    qDebug() << "[ Connection" << m_handle << "] error" << error;
}

void Connection::onStateChanged( QAbstractSocket::SocketState state )
{
    qDebug() << "[ Connection" << m_handle << "] state changed to" << state;
}

void Connection::onReadyRead()
{
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
            qDebug() << "[ Connection" << m_handle << "] recieved garbage. skipping";
            // TODO: send message about it to client
            return;
        }
    }

    if( m_socket->bytesAvailable() < (qint64)m_blockSize - (qint64)sizeof(quint32) ) return;

    qDebug() << "[ Connection" << m_handle << "] recieved a message" << m_socket->bytesAvailable() << "bytes";

    QVariantMap msg;
    in >> msg;
    processMessage( msg );


    m_blockSize = 0;

    if( m_socket->bytesAvailable() > 0 ) QTimer::singleShot( 0, this, &Connection::onReadyRead );
}

void Connection::start()
{
    m_socket = new QTcpSocket( this );
    if (!m_socket->setSocketDescriptor(m_handle) ) {
        qDebug() << "[ Connection" << m_handle << "] couldn't establish connection to peer.";
        emit finished();
        return;
    }
    connect( m_socket, &QTcpSocket::disconnected, this, &Connection::finished );
    connect( m_socket, &QTcpSocket::connected, this, &Connection::onConnected );
    connect( m_socket, &QTcpSocket::disconnected, this, &Connection::onDisconnected );
    connect( m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)) );
    connect( m_socket, &QTcpSocket::stateChanged, this, &Connection::onStateChanged );
    connect( m_socket, &QTcpSocket::readyRead, this, &Connection::onReadyRead );
    qDebug() << "[ Connection" << m_handle << "] connection created and established";

    checkDatabase();
    sendReadyMessage();
}

void Connection::sendMessage(QVariantMap msg)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << (quint32)0;
    out << magic;
    out << msg;
    out.device()->seek(0);
    qDebug() << "[ Connection" << m_handle << "] sending a message" << block.size() << "bytes";
    out << (quint32)(block.size() - sizeof(quint32));
    m_socket->write(block);
}

void Connection::onDatabaseOpenedChanged(bool arg)
{
    Q_UNUSED(arg)
    sendReadyMessage();
}


QString Connection::sqlSelect(const QVariantMap &msg, bool count)
{
    if( msg.isEmpty() ) return QString();

    QString dataSource;
    if( msg.contains( "dataSource" ) )
        dataSource = msg[ "dataSource" ].toString();
    else
        return QString();
    QString method;
    if( msg.contains( "method" ) )
        method = msg[ "method" ].toString();
    else
        return QString();

    QString queryString = m_settings->data()[ "dataSources" ].toMap()[ dataSource ].toMap()[ "methods" ].toMap()[ method ].toString();
    if( queryString.isEmpty() )
        return QString();
qDebug() << "wanna parent " << msg.value( "currentSelectedSiteId" ).toString();
    // if( queryString.contains( "CALL" ) ) return queryString;
    if( queryString.contains( ":currentSelectedSiteId" ) )
        if( msg.contains( "currentSelectedSiteId" ) )
            queryString.replace( ":currentSelectedSiteId", msg.value( "currentSelectedSiteId" ).toString());
//        else
//            queryString.replace( "currentSelectedSiteId", "-1" );
    if( queryString.contains( ":parentId" ) )
        if( msg.contains( "parentId" ) )
            queryString.replace( ":parentId", msg.value( "parentId" ).toString());


    QStringList where; // TODO: compose where statement
    if( msg.contains( "filters" ) ){
        const QVariantMap& filters = msg.value( "filters" ).toMap();
        for( const QString& k: filters.keys() ){
            const QVariantMap& filter = filters.value( k ).toMap();
            if( filter.value( "type" ).toString() == "bool" ) {
                where.append( QString( "%1 = %2" ).arg( filter.value( "field" ).toString() ).arg( filter.value( "value" ).toBool() ) );
            }
            else if( filter.value( "type" ).toString() == "number" ) {

            }
            else if( filter.value( "type" ).toString() == "dateRange" ) {
                where.append( QString( "%1 = %2" ).arg( filter.value( "field" ).toString() ).arg( filter.value( "value" ).toBool() ) );
            }
            else if( filter.value( "type" ).toString() == "string" ) {
                QString filterstringhere = QString( "%1 LIKE '%" ).arg( filter.value( "field" ).toString() );
                filterstringhere.append( filter.value( "value" ).toString() );
                filterstringhere.append( "%'" );
                where.append( filterstringhere );
            }
        }
//        if( command.value( "where" ).type() == QVariant::Map ) {
//            const QVariantMap& w = command.value( "where" ).toMap();
//            QStringList wc;
//            for( const QString& k: w.keys() ){
//                wc.append( QString( "%1 LIKE '%%2%'" ).arg( k ).arg( w.value( k ) ) );
//            }
//        }
    }
    QString w = where.join( " AND " );
    if( !where.isEmpty() ) w.prepend( " AND " );


    if( count ) {
        QString co( m_settings->data()[ "dataSources" ].toMap()[ dataSource ].toMap()[ "methods" ].toMap()[ "COUNT" ].toString() );
        if( co.contains( ":currentSelectedSiteId" ) )
            if( msg.contains( "currentSelectedSiteId" ) )
                co.replace( ":currentSelectedSiteId", msg.value( "currentSelectedSiteId" ).toString());
        if( co.contains( ":parentId" ) )
            if( msg.contains( "parentId" ) )
                co.replace( ":parentId", msg.value( "parentId" ).toString());
        return co.append( w );
    }
//    if( count ) return QString( "SELECT count(*) FROM (%1%2) a;" ).arg( queryString ).arg( w );

    QString orderBy;
    if( msg.contains( "orderBy" ) ){
        orderBy = msg.value( "orderBy" ).toMap().value( "field" ).toString();
        if( !orderBy.isEmpty() ) {
            if( msg.value( "orderBy" ).toMap().value( "ascending" ).toBool() ) orderBy.append( " ASC" );
            else orderBy.append( " DESC" );
        }
    }
    if( !orderBy.isEmpty() )
        orderBy.prepend( " ORDER BY " );




    int wpos = 0;
    if( msg.contains( "windowPosition" ) )
        wpos = msg.value( "windowPosition" ).toInt();
    int wsize = 0;
    if( msg.contains( "windowSize" ) )
        wsize = msg.value( "windowSize" ).toInt();
    QString limit;
    if( wsize != 0 )
        limit = QString( " LIMIT %1,%2" ).arg( wpos ).arg( wsize );

    return queryString.append( w ).append( orderBy ).append( limit );
}


