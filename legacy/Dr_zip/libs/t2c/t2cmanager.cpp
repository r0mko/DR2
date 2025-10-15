#include "t2cmanager.h"
#include "t2cworker.h"
#include <QDebug>
#include <QTextCodec>
#include <QThread>

void T2CManager::start()
{
    if( m_host.isEmpty() ) {
        qWarning() << "T2CManager: set host please";
        return;
    }
    if( m_port == 0 ) {
        qWarning() << "T2CManager: set port please";
        return;
    }

    QThread *threadAsync = new QThread;
    connect(threadAsync, &QThread::started, m_workerAsync, &T2CWorker::start);
    connect(m_workerAsync, &T2CWorker::stopped, m_workerAsync, &T2CWorker::deleteLater);
    connect(m_workerAsync, &T2CWorker::destroyed, threadAsync, &QThread::quit);
    connect(m_workerAsync, &T2CWorker::destroyed, threadAsync, &QThread::deleteLater);
    m_workerAsync->moveToThread(threadAsync);
    threadAsync->start();

//    QThread *threadSync = new QThread;
//    connect(threadSync, &QThread::started, m_workerSync, &T2CWorker::start);
//    connect(m_workerSync, &T2CWorker::stopped, m_workerSync, &T2CWorker::deleteLater);
//    connect(m_workerSync, &T2CWorker::destroyed, threadSync, &QThread::quit);
//    connect(m_workerSync, &T2CWorker::destroyed, threadSync, &QThread::deleteLater);
//    m_workerSync->moveToThread(threadSync);
//    threadSync->start();
    m_workerSync->start();
}

void T2CManager::stop()
{
    m_workerAsync->stop();
    m_workerSync->stop();
}

void T2CManager::onAsyncWorkerReady(bool ready)
{
    if( ready ) {

    }
}

void T2CManager::onGotProcessVariableMessage(const ProcessVariable pv)
{
    if( m_subscribedHash.contains( pv.idf() ) ) {
        m_subscribedHash[ pv.idf() ] = pv;
        // emit updated suscribed
    }else{
        m_cache[ pv.idf() ] = pv;
        // emit updated cache
    }
}

QString T2CManager::getName(int idf)
{
    return m_workerSync->getName(idf);
}

T2CManager::T2CManager(QObject *parent) :
    QObject(parent),
    m_workerAsync(new T2CWorker),
    m_workerSync(new T2CWorker),
    m_codec(QTextCodec::codecForName("ISO 8859-5"))
{
    connect(m_workerAsync, &T2CWorker::gotProcessVariableMessage, this, &T2CManager::onGotProcessVariableMessage);
    connect(m_workerAsync, &T2CWorker::connectedChanged, this, &T2CManager::onAsyncWorkerReady);
    connect(this, &T2CManager::subscribeSignal, m_workerAsync, &T2CWorker::subscribe);
    connect(this, &T2CManager::unsubscribeSignal, m_workerAsync, &T2CWorker::unsubscribe);
//    connect(this, &T2CManager::onSubscribe, m_worker, &T2CWorker::subscribe);
//    connect(this, &T2CManager::sendMessage, m_worker, &T2CWorker::sendMessage);
    //    connect(this, &T2CManager::setValue, m_worker, &T2CWorker::setValue);
}

void T2CManager::classBegin()
{

}

void T2CManager::componentComplete()
{
    start();
}

QString T2CManager::host() const
{
    return m_host;
}

int T2CManager::port() const
{
    return m_port;
}

QString T2CManager::encoding() const
{
    return QString(m_codec->name());
}

int T2CManager::symLevel() const
{
    return m_symLevel;
}

bool T2CManager::connected() const
{
    if(m_workerAsync == nullptr || m_workerSync == nullptr) {
        qWarning() << "T2CWorkers are null";
        return false;
    }
    return m_workerAsync->connected() && m_workerSync->connected();
}

void T2CManager::subscribe(int idf)
{
    if( !connected() ) {
        m_subscribeQueue.insert( idf );
    }else{
        if(!m_subscribedHash.contains(idf)) {
            ProcessVariable pv;
            pv.setIdf(idf);
            subscribeSignal(idf);
            m_subscribedHash.insert( idf, pv );
        }
    }
}

void T2CManager::unsubscribe(int idf)
{
    m_subscribeQueue.remove(idf);
    if( !m_subscribedHash.contains(idf)) return;
    m_subscribedHash.remove(idf);
    unsubscribeSignal(idf);
}


void T2CManager::setHost(QString arg)
{
    if(m_workerAsync == nullptr || m_workerSync == nullptr) {
        qWarning() << "T2CWorkers are null";
        return;
    }
    if (m_host == arg) return;
    m_host = arg;
    m_workerAsync->setHost(arg);
    m_workerSync->setHost(arg);
    emit hostChanged(arg);
}

void T2CManager::setPort(int arg)
{
    if(m_workerAsync == nullptr || m_workerSync == nullptr) {
        qWarning() << "T2CWorkers are null";
        return;
    }
    if (m_port == arg) return;
    m_port = arg;
    m_workerAsync->setPort(arg);
    m_workerSync->setPort(arg);
    emit portChanged(arg);
}

void T2CManager::setEncoding(QString arg)
{
    if(m_workerAsync == nullptr || m_workerSync == nullptr) {
        qWarning() << "T2CWorkers are null";
        return;
    }
    if( arg == encoding() ) return;
    m_codec = QTextCodec::codecForName(arg.toLocal8Bit());
    m_workerAsync->setEncoding(arg);
    m_workerSync->setEncoding(arg);
    emit encodingChanged(arg);
}

void T2CManager::setSymLevel(int arg)
{
    if(m_workerAsync == nullptr || m_workerSync == nullptr) {
        qWarning() << "T2CWorkers are null";
        return;
    }
    if (m_symLevel == arg)
        return;

    m_symLevel = arg;
//    m_workerAsync->setPort(arg);
//    m_workerSync->setPort(arg);
    emit symLevelChanged(arg);
}
