#include "t2cmanager2.h"
#include <QThread>
#include "t2cworker2.h"
#include <QTextCodec>
T2CManager2::T2CManager2(QObject *parent) :
    QObject(parent),
    m_worker(new T2CWorker2)
{
    connect(m_worker, &T2CWorker2::gotMessage, this, &T2CManager2::gotMessage);
    connect(this, &T2CManager2::onSubscribe, m_worker, &T2CWorker2::subscribe);
    connect(this, &T2CManager2::sendMessage, m_worker, &T2CWorker2::sendMessage);
    connect(this, &T2CManager2::setValue, m_worker, &T2CWorker2::setValue);
}

QString T2CManager2::host() const
{
    if(m_worker == nullptr) return QString();
    return m_worker->host();
}

int T2CManager2::port() const
{
    if(m_worker == nullptr) return 0;
    return m_worker->port();
}

QString T2CManager2::encoding() const
{
    return QString(m_codec->name());
}

bool T2CManager2::ready() const
{
    return m_ready;
}

void T2CManager2::setHost(QString host)
{
    if (m_worker->host() != host)
        m_worker->setHost(host);
}

void T2CManager2::setPort(int port)
{
    if (m_worker->port() != port)
        m_worker->setPort(port);
}

void T2CManager2::start()
{
    QThread *thread = new QThread;
    connect(thread, &QThread::started, m_worker, &T2CWorker2::start);
    connect(m_worker, &T2CWorker2::stopped, m_worker, &T2CWorker2::deleteLater);
    connect(m_worker, &T2CWorker2::destroyed, thread, &QThread::quit);
    connect(m_worker, &T2CWorker2::destroyed, thread, &QThread::deleteLater);
    m_worker->moveToThread(thread);
    thread->start();
}

void T2CManager2::stop()
{

}

void T2CManager2::subscribe(int idf)
{
    emit onSubscribe(idf);
}

void T2CManager2::setEncoding(QString arg)
{
    if (encoding() == arg) return;
    m_codec = QTextCodec::codecForName(arg.toLocal8Bit());
    emit encodingChanged(arg);
}

