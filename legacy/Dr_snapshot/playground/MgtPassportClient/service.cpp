#include "service.h"
#include <QThread>
#include "serviceworker.h"
#include <QTimer>
#include "partmodel.h"

Service::Service(QObject *parent) :
    QObject(parent),
    m_thread( new QThread ),
    m_serviceWorker( new ServiceWorker ),
    m_busy( false ),
    m_model( nullptr ),
    m_modelNext( nullptr ),
    m_host( "localhost" ),
    m_port( 33667 ),
    m_isConnected( false ),
    m_autoReconnect( true ),
    m_databaseOnline( false ),
    m_authenticated( false )
{
    /* socket creation initialization and start */
    connect( this, &Service::hostChanged, m_serviceWorker, &ServiceWorker::setHost );
    connect( this, &Service::portChanged, m_serviceWorker, &ServiceWorker::setPort );
    connect( this, &Service::autoReconnectChanged, m_serviceWorker, &ServiceWorker::setAutoReconnect );
    connect( m_serviceWorker, &ServiceWorker::connected, this, &Service::connected );
    connect( m_serviceWorker, &ServiceWorker::disconnected, this, &Service::disconnected );

    connect( m_thread, &QThread::finished, m_thread, &QThread::deleteLater );
    connect( m_thread, &QThread::started, m_serviceWorker, &ServiceWorker::start );
    //    connect( m_serviceWorker, &ServiceWorker::finished, m_serviceWorker, &ServiceWorker::deleteLater ); // ?? do we need that?

    connect( m_serviceWorker, &ServiceWorker::connected, this, &Service::serviceWorkerConnected );
    //    connect( m_serviceWorker, &ServiceWorker::disconnected, this, &Service::authenticated );
    //    connect( m_serviceWorker, &ServiceWorker::disconnected, this, &Service::databaseOnline );
    connect( m_serviceWorker, &ServiceWorker::disconnected, this, &Service::serviceWorkerDisconnected );

    connect( this, &Service::sendMessage, m_serviceWorker, &ServiceWorker::sendMessage );
    connect( m_serviceWorker, &ServiceWorker::recievedMessage, this, &Service::onMessageRecieved );
    //    connect( this, &OldModel::onlineModeChanged, this, &OldModel::onOnlineModeChanged );

    /**/    connect( this, &Service::dirtyQueue, this, &Service::onDirtyQueue );

    m_serviceWorker->moveToThread( m_thread );
    m_thread->start();
}

Service::~Service()
{

}

void Service::serviceWorkerConnected()
{
    m_isConnected = true;
    emit isConnectedChanged( true );
}

void Service::serviceWorkerDisconnected()
{
    m_isConnected = false;
    emit isConnectedChanged( false );
    m_userInfo.clear();
    m_authenticated = false;
    emit userInfoChanged( m_userInfo );
    emit authenticatedChanged( m_authenticated );
}

void Service::onMessageRecieved(const QVariantMap &message)
{
    if( message.isEmpty() ){
        qDebug() << "[ Client ] !! recieved empty message";
        return;
    }
    if( !message.contains( "messageType" ) ){
        qDebug() << "[ Client ] !! recieved message had no messageType";
        return;
    }

    const QString& messageType = message.value( "messageType" ).toString();
//    qDebug() << "[ Client ] processing message type" << messageType;


    if( messageType == QString( "info" ) ) processInfoMessage( message );
    else if( messageType == QString( "loginReply" ) ) processLoginReplyMessage( message );
    else if( messageType == QString( "logoutReply" ) ) processLogoutReplyMessage( message );
    else if( messageType == QString( "dataReply" ) ) processDataReplyMessage( message );
    else if( messageType == QString( "error" ) ) processErrorMessage( message );
    else if( messageType == QString( "updateNotice" ) ) processUpdateNoticeMessage( message );
}

void Service::processInfoMessage(const QVariantMap &message)
{
    if( message.contains( "serverDatabaseReady" ) ) {
        m_databaseOnline = message.value( "serverDatabaseReady" ).toBool();
        emit databaseOnlineChanged( m_databaseOnline );
    }
}

void Service::processLoginReplyMessage(const QVariantMap &message)
{
    // TODO: if error msg from server then notify user;

    if( message.contains( "error" ) ) {
        qDebug() << "[ Client ] login error:" << message.value( "error" ).toString();
        return;
    }
//    qDebug() << "[ Client ] login:" << message;
    m_userInfo = message;
    m_authenticated = true;
    emit userInfoChanged( m_userInfo );
    emit authenticatedChanged( true );
    dirtyQueue();
}

void Service::processErrorMessage(const QVariantMap &message)
{
    qDebug() << "[ Client ] Error:" << message;
}

void Service::processLogoutReplyMessage(const QVariantMap &message)
{
    Q_UNUSED(message)
    m_userInfo.clear();
    m_authenticated = false;
    emit userInfoChanged( m_userInfo );
    emit authenticatedChanged( m_authenticated );
}

void Service::processDataReplyMessage(const QVariantMap &message)
{
    if( m_model != nullptr ){
        m_model->placeData( message );
        m_model = nullptr;
        m_busy = false;
    }
    if( !m_dirtyModelQueue.isEmpty() ) emit dirtyQueue();

    //    if( m_modelNext != nullptr ){
    //        m_model = m_modelNext;
    //        m_modelNext = nullptr;
    //        onDataRequestByModel( m_model );
    //    }
}

void Service::processUpdateNoticeMessage(const QVariantMap &message)
{
    qDebug() << "[ Client ] update notice:" << message;
    emit newNotice(message);
}

void Service::onDirtyQueue()
{
    if( !authenticated() ) {
        return;
    }
    if( !m_busy ) {
        if( m_model != nullptr ) {
            qDebug() << "something is totaly wrong model should be nullptr";
            return;
        }
        if( !m_dirtyModelQueue.isEmpty() ) {
            m_model = m_dirtyModelQueue.take( m_dirtyModelQueue.keys().last() );
            m_busy = true;
            sendMessage( m_model->dataRequestMessage() );
        }
    }
}



void Service::connectToHost()
{

}

void Service::disconnectFromHost()
{
    m_userInfo.clear();
    m_authenticated = false;
    emit userInfoChanged( m_userInfo );
    emit authenticatedChanged( m_authenticated );
}

void Service::login(QString user, QString pass)
{
    QVariantMap m;
    m.insert( "messageType", "login" );
    m.insert( "login", user );
    m.insert( "pass", pass );
    sendMessage( m );
}

void Service::logout()
{
    m_userInfo.clear();
    m_authenticated = false;
    emit userInfoChanged( m_userInfo );
    emit authenticatedChanged( m_authenticated );
    QVariantMap m;
    m.insert( "messageType", "logout" );
    sendMessage( m );
}

void Service::stopServer()
{
    QVariantMap m;
    m.insert( "messageType", "stopServer" );
    sendMessage( m );
}

void Service::refreshServerSettings()
{
    QVariantMap m;
    m.insert( "messageType", "serverReloadSettings" );
    sendMessage( m );
}

void Service::onDataRequestByModel(PartModel *model)
{
    if( m_busy ) {
        //        m_dirtyModelQueue.insert( model, model );
        m_modelNext = model;
    }else{
        if( model != nullptr ) {
            //            bool safe = model->checkSafeZone();
            //            if( !safe ){
            m_busy = true;
            m_model = model;
            sendMessage( m_model->dataRequestMessage() );
            //            }
        }
    }
}

void Service::queueRequest(PartModel *model)
{
    m_dirtyModelQueue.insert( model, model );
    emit dirtyQueue();
}

